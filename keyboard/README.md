# Keyboard Demo

Reading keyboard input on AmigaOS using the lowlevel.library.

This example demonstrates how to:

- Open the lowlevel.library
- Read keyboard state with `GetKey()`
- Detect modifier keys (Shift, Control, Alt, Amiga)
- Detect arrow keys
- Track state changes to avoid output flooding

## How GetKey() works

The `GetKey()` function from lowlevel.library returns the current keyboard state as a 32-bit value:

- **Lower 8 bits (0x00FF)**: Raw key code (e.g., arrow keys, ESC)
- **Upper 24 bits**: Modifier key flags (e.g., Shift, Control, Alt)

## Compile

As compiler SAS/C 6.51 is used

```cmd
sc link keyboard.c
```
