"""
Claude Code Hook — LED Status Reporter
Reads hook event JSON from stdin, sends a single-byte command
to the RP2040 macro pad via USB CDC serial.

Commands:
  T = Thinking  (PreToolUse)
  C = Completed (PostToolUse, Stop with reason=completed)
  E = Error     (PostToolUseFailure, StopFailure, PermissionDenied)

Setup:
  pip install pyserial
  Set PORT below to match your device (e.g. COM3, COM7).
"""

import sys
import json

PORT = "COM3"   # <-- Change this to match your RP2040's CDC serial port
BAUD = 9600

EVENT_MAP = {
    "PreToolUse":          "T",
    "PostToolUse":         "C",
    "Stop":                "C",   # refined below for stop_reason
    "PostToolUseFailure":  "E",
    "StopFailure":         "E",
    "PermissionDenied":    "E",
}

def main():
    try:
        data = json.load(sys.stdin)
    except Exception:
        sys.exit(0)

    event = data.get("hook_event_name", "")

    cmd = EVENT_MAP.get(event)

    # Distinguish user-interrupted Stop from completed Stop
    if event == "Stop":
        stop_reason = data.get("stop_reason", "completed")
        if stop_reason == "user_interrupt":
            cmd = "I"  # Return to idle on manual interrupt

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
