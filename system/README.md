# System Overtake Demo

With the function SystemControl, you can take over the system to give your program all CPU cycles.

## Example

In this example we start a timer and increase a counter in a loop for 3 interrupts.

After this we do the same with overtaking the system.

The result is that you have more counts with overtaking the system than without overtaking the system.

## Compile

As compiler SAS/C 6.51 is used

```cmd
sc link system_control.c
```