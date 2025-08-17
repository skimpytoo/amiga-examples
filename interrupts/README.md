# Interrupts

Here are some examples of using timer interrupts.
The files with the prefix 'lowlevel' are using the lowlevel.library for handling of the interrupt.

## Types

There are two kinds of timer interrupts.
VBLANK and Timer Interrupts.

### VBLANK-Interrupt

The registered interrupt routine is triggered with each vertical blank (50/60 Hz).
VBlankInterrupt is registered via AddIntServer(INTB_VERTB, &VBlankInterrupt).

#### Pros

- Synchronous to 50 Hz (PAL) or 60 Hz (NTSC) → perfect for image generation.
- You automatically get exactly one tick per screen refresh.
- Easy for frame-rate-locked games.

#### Cons

- Only one fixed frequency (50/60 Hz).

### Timer Interrupts

#### Pros

-Very precise timing
-Can be adjusted to any desired frequency.

#### Cons

- Somewhat more complicated in structure.

## Interrupt Service Routine

There are some points to consider for using ISRs:

- Data must always remain valid as long as the interrupt is registered → No local variables that disappear from the stack after main() is called. Ideally, they should be static or global or allocated via AllocMem().

- No OS functions with a Forbid() risk should be called in the ISR → Only use interrupt-safe functions in an ISR (i.e., no DOS, graphics, or Intuition functions).

- Pointers are allowed, but they must point to memory that is not currently being modified by another task without protection (semaphore/disable/forbid).

## Compile

As compiler SAS/C 6.51 is used

```cmd
sc link lowlevel_timer.c
sc link lowlevel_vblank.c
sc link vblank.c
```
