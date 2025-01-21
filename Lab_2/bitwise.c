/*-------------------------------------------------------------------------------------------
 * File:        Lab02_P3.c
 * Function:    Perform bitwise OR and AND operations on elements of two arrays
 * Description: This program performs bitwise OR and AND operations on the corresponding elements of two arrays.
 * Input:       Two arrays with at least 5 elements and at least one element in each array 
 *              should be a negative integer and at least two elements in each array should 
 *              be greater than or equal to 0xA.
 * Output:      The result of bitwise OR and AND operations on the corresponding elements of the two arrays.
 * Author(s):   Anshika Sinha
 * Date:        01/16/2025
 *-------------------------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdio.h>

int main() {
    // Initialize the arrays with hexadecimal values
    int X[] = {0xA, 0x2, 0xB, 0x3, -0x5, 0x6};
    int Y[] = {-0x7, 0x8, 0x23, 0x13, 0x23, 0xE};

    // Determine the size of the arrays
    int size = sizeof(X) / sizeof(X[0]);

    // Loop through the arrays and perform bitwise operations
    for (int i = 0; i < size; i++) {
        int or_result = X[i] | Y[i];
        int and_result = X[i] & Y[i];

        // Print the results in octal format
        printf("The OR operation for %dth elements is: %012o\n", i, or_result);
        printf("The AND operation for %dth elements is: %012o\n", i, and_result);
    }

    return 0;
}
