# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a **hardware design project** for the "Claude Ghost Macro Pad" (V2.0 Relegendable Keycaps Edition) — a left-hand programmable macro keyboard optimized for the Claude Code development workflow. It contains no source code or build system; all assets are design documents, diagrams, and icon images.

## Repository Contents

- **README.md** — Full hardware spec in Traditional Chinese (BOM, layout, LED wiring, assembly notes)
- **按鈕佈局.drawio** — Draw.io diagram of the physical button layout (edit with Draw.io desktop or diagrams.net)
- **按鈕佈局.jpg** — Exported image of the current layout
- **buttonIcon/** — PNG icon assets for each key function

## Hardware Specification Summary

**Controller:** YD-RP2040 (Raspberry Pi Pico-compatible, USB Type-C)

**Key Layout (all 1U keycaps):**
- Top-left standalone: `Terminate` (Ctrl+C)
- Top-right standalone: `Knob / ESC` (rotary encoder — scroll or press to escape)
- Central 2×4 matrix:
  - Upper row: `Change Permission Mode` | `Explain` (Ctrl+E) | `UP` | `Enter`
  - Lower row: `Show Todo` (Ctrl+T) | `/` (Slash Command) | `DOWN` | `Switch Window` (Win+Tab)

**LED (RGB, common-anode):** Longest pin → 3.3V; R/G/B pins each through 220Ω resistor to GPIO. Logic: `False` (LOW) = on, `True` (HIGH) = off.

**Critical dimension:** Knob center to Enter key center must be >27mm to avoid collision with the 35mm-diameter aluminum knob cap.

## Working with Design Files

- Edit the layout in **按鈕佈局.drawio** using [diagrams.net](https://diagrams.net) (free, no install required) or the Draw.io desktop app.
- After editing, export a new `.jpg` to replace 按鈕佈局.jpg for documentation accuracy.
- Keycap label size: **14mm × 14mm** — function name on top, shortcut below in smaller text.
