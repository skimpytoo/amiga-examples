# File I/O Examples

This program demonstrates how to list the contents of a directory on AmigaOS using the ExAll() function.

## How it works

Listing a directory with ExAll() involves:

- Lock the directory with `Lock()`
- Allocate a buffer for directory entries
- Create an `ExAllControl` structure
- Call `ExAll()` repeatedly to fetch batches of entries
- Iterate through the linked list of entries in each batch
- Display the information (name, size, type)
- Free resources

## Compile

As compiler SAS/C 6.51 is used

```cmd
sc link listdir.c
```
