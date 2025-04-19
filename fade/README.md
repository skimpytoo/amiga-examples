# Fade Demo

An example to fade from one color palette to another palette.

Increasing the number of partitions, will make the fading slower, but finer.

If the number of partitions is equal to the number of steps, you will get a full fade to the end palette.

If the fading steps are lower, you will get a darkening effect of the end palette.

# Compile

As compiler SAS/C 6.51 is used

```cmd
sc link fade.c
```
