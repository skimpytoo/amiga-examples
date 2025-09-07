# System Demos

Several demos about controlling/manipulating the system.

## System Overtake Demo

With the function SystemControl, you can take over the system to give your program all CPU cycles.

### Example

In this example we start a timer and increase a counter in a loop for 3 interrupts.

After that we do the same with overtaking the system.

The result is that you have more counts with overtaking the system than without overtaking the system.

### Compile

As compiler SAS/C 6.51 is used

```cmd
sc link system_control.c
```

## Hide mouse pointer Demo

Sometimes, especially in games, you want to hide the mouse pointer of the system.
There are two possibilities.

- Hide pointer by OS
- Hide pointer by accessing the hardware registers

### Example - OS

This is an example of hiding the pointer with the help of the OS functions.
As there is no function to access the pointer only with a screen, we need to create a hidden window and attach it to our screen.
With the help of this window, we can hide the pointer.

As a movement of the mouse will lead to the reappearing of the pointer, we need to stop reporting mouse events.
For that we could use:

```c
SystemControl(SCON_TakeOverSys, TRUE, TAG_DONE)
```

But be aware that all other programs would freeze until you give the control back to AmigaOS.

### Compile

As compiler SAS/C 6.51 is used

```cmd
sc link hidepointer.c
```