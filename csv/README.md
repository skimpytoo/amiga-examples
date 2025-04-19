# Read CSV Demo

Simple example: how to read a CSV file (';' as separator).

## Drawback

In this example, the csv-file is read twice.

1. To determ the number of entries. To know how much memory, we need to allocate for the parsed content.
2. Read the actual entries

## Better solution

Allocate the required memory incrementally and store the entries in a linked list.

# Compile

As compiler SAS/C 6.51 is used

```cmd
sc link readcsv.c
```
