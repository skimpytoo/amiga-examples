#include <stdio.h>
#include <stdlib.h>

/*
 * Define a function pointer
 * Syntax: return_type (*pointer_name)(parameter_types);
 * the pointer_name can be named as desired and is used to reference the function
 * the parameter_types must match the function signature and can be any valid type
 */
typedef int (*FunctionName)(int, int);

/*
 * Example Function implementations - Example 1
 * These functions match the signature defined by the FunctionName typedef on line 9
 */
int add(int a, int b) {
    return a + b;
}

/*
 * Another example function - Example 1
 * This function also matches the signature defined by the FunctionName typedef on line 9
 */
int sub(int a, int b) {
    return a - b;
}

/*
 * Function that uses a function pointer - Example 2
 *  Helper function that takes a function pointer as an argument
 *  This function can call any function that matches the FunctionName signature
 */
int performOperation(int x, int y, FunctionName op) {
    return op(x, y);
}

/* Main program */
int main(void) {
    int parameter1, parameter2;  // Parameters for the functions
    int result;
    FunctionName operation;  // Declare a function pointer variable

    printf("Function Pointer Example\n");
    printf("========== Example 1 - Direct Function Pointer Calls ========\n\n");

    operation = add;
    parameter1 = 20;
    parameter2 = 5;

    /* Use function pointers directly */
    printf("Function pointer call - add:\n");
    result = operation(parameter1, parameter2);
    printf("%d + %d = %d\n", parameter1, parameter2, result);

    printf("Switching function pointer to sub function.\n");
    operation = sub;

    printf("Function pointer call - sub:\n");
    result = operation(parameter1, parameter2);
    printf("%d - %d = %d\n", parameter1, parameter2, result);

    printf("========== Example 2 - HelperFunction ========\n\n");
    /* Use function pointers with a helper function */
    printf("Using function pointers with helper:\n");
    result = performOperation(10, 3, add);
    printf("performOperation(10, 3, add) = %d\n", result);

    printf("\nDone!\n");
    return 0;
}
