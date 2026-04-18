"""
Claude Code Hook — LED Status Reporter
Reads hook event JSON from stdin, sends a single-byte command
to the RP2040 macro pad via USB CDC serial.

Commands:
  T = Thinking  (UserPromptSubmit, PreToolUse, PostToolUse)
  C = Completed (Stop with reason=completed)
  E = Error     (PostToolUseFailure, StopFailure, PermissionDenied, permission_prompt)

Setup:
  pip install pyserial
  Set PORT below to match your device (e.g. COM3, COM7).
"""

import sys
import json

PORT = "COM4"   # <-- Change this to match your RP2040's CDC serial port
BAUD = 9600

EVENT_MAP = {
    "UserPromptSubmit":    "T",
    "PreToolUse":          "T",
    "PostToolUse":         "T",   # stay blue until fully stopped
    "Stop":                "C",   # refined below for stop_reason
    "PostToolUseFailure":  "E",
    "StopFailure":         "E",
    "PermissionDenied":    "E",
}

LOG = "d:/projects/claude_microPad/hooks/debug.log"

def main():
    try:
        data = json.load(sys.stdin)
    except Exception:
        sys.exit(0)

    # DEBUG: log all incoming payloads
    with open(LOG, "a", encoding="utf-8") as f:
        f.write(json.dumps(data) + "\n")

    event = data.get("hook_event_name", "")

    # Notification hook: permission_prompt → red (waiting for approval)
    if event == "Notification":
        if data.get("notification_type") == "permission_prompt":
            cmd = "E"
        else:
            sys.exit(0)
    else:
        cmd = EVENT_MAP.get(event)

    # Distinguish user-interrupted Stop from completed Stop
    if event == "Stop":
        stop_reason = data.get("stop_reason", "completed")
        if stop_reason == "user_interrupt":
            cmd = "I"  # Return to idle on manual interrupt

    # Tool interrupted by user (not a real error) → green
    if event == "PostToolUseFailure" and data.get("is_interrupt"):
        cmd = "C"

    if cmd is None:
        sys.exit(0)

    try:
        import serial
        with serial.Serial(PORT, BAUD, timeout=1) as s:
            s.write(cmd.encode())
    except Exception as e:
        # Never block Claude Code due to serial errors
        sys.stderr.write(f"[led_hook] serial error: {e}\n")

    sys.exit(0)


if __name__ == "__main__":
    main()
