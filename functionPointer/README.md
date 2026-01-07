# Function Pointers

Function pointers allow you to store the address of a function and call it dynamically at runtime. This is useful for implementing callbacks, dispatch tables, and flexible function selection.

## Examples

This example demonstrates how to use function pointers in SAS/C for Amiga.

### Direct function call

Demonstrates assigning a function to a function pointer variable and calling it directly. The pointer is reassigned to point to different functions (`add` and `sub`), showing how the same pointer variable can dynamically call different functions at runtime.

### Function call with helper

Demonstrates passing a function pointer as a parameter to another function. The `performOperation()` helper function accepts a function pointer and calls it with the provided arguments. This pattern is useful for creating flexible APIs where the caller can specify which function to execute.

## Compilation

```cmd
sc link functionPointer.c
```
