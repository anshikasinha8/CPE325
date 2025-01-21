/*-------------------------------------------------------------------------------------------
 * File:        Lab02_P1.c
 * Function:    Data types and their sizes
 * Description: This program displays the sizes and ranges of various data types.
 * Input:       None
 * Output:      The sizes and ranges of the various data types
 * Author(s):   Anshika Sinha
 * Date:        01/15/2025
 *-------------------------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>

int main() {
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("| Data Type               | Value       | Size (in bytes) | Min                   | Max                  |\n");
    printf("----------------------------------------------------------------------------------------------------------\n");

    printf("| signed char             | 'F'         | %2zu              | %d                  | %d                  |\n", sizeof(signed char), SCHAR_MIN, SCHAR_MAX);
    printf("| short int               | 200         | %2zu              | %d                | %d                |\n", sizeof(short int), SHRT_MIN, SHRT_MAX);
    printf("| int                     | 1000        | %2zu              | %d           | %d           |\n", sizeof(int), INT_MIN, INT_MAX);
    printf("| long int                | 10000       | %2zu              | %ld  | %ld  |\n", sizeof(long int), LONG_MIN, LONG_MAX);
    printf("| long long int           | 100000      | %2zu              | %lld  | %lld  |\n", sizeof(long long int), LLONG_MIN, LLONG_MAX);
    printf("| unsigned char           | 'G'         | %2zu              | 0                     | %u                  |\n", sizeof(unsigned char), UCHAR_MAX);
    printf("| unsigned short int      | 40000       | %2zu              | 0                     | %u                |\n", sizeof(unsigned short int), USHRT_MAX);
    printf("| unsigned int            | 500000      | %2zu              | 0                     | %u           |\n", sizeof(unsigned int), UINT_MAX);
    printf("| unsigned long int       | 1000000     | %2zu              | 0                     | %lu |\n", sizeof(unsigned long int), ULONG_MAX);
    printf("| unsigned long long int  | 10000000    | %2zu              | 0                     | %llu |\n", sizeof(unsigned long long int), ULLONG_MAX);
    printf("| float                   | 3.14        | %2zu              | %e          | %e         |\n", sizeof(float), FLT_MIN, FLT_MAX);
    printf("| double                  | 3.14159     | %2zu              | %e         | %e        |\n", sizeof(double), DBL_MIN, DBL_MAX);

    printf("----------------------------------------------------------------------------------------------------------\n");
    return 0;
}

