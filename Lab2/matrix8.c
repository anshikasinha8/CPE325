/*-------------------------------------------------------------------------
 * File:        Lab02_PB.c
 * Function:    Multiply two 8x8 matrices
 * Description: This program multiplies two 8x8 matrices and displays the result.
 * Input:       Two matrices of size 8x8
 * Output:      The resulting matrix after multiplying the two input matrices
 * Author(s):   Anshika Sinha
 * Date:        01/19/2025
 *-----------------------------------------------------------------------*/
#include <msp430.h>
#include <stdio.h>

#define SIZE 8

void displayMatrix(int matrix[SIZE][SIZE], const char *name);

void multiplyMatrices(int m1[SIZE][SIZE], int m2[SIZE][SIZE], int result[SIZE][SIZE]);

int main()
{
    int m1[SIZE][SIZE];
    int m2[SIZE][SIZE];
    int result[SIZE][SIZE];

    // Initialize the matrices
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            m1[i][j] = i + j;            // Example values for matrix1
            m2[i][j] = (i == j) ? 1 : 0; // Identity matrix for matrix2
        }
    }

    // Display input matrices
    displayMatrix(m1, "Matrix 1");
    displayMatrix(m2, "Matrix 2");

    // Perform matrix multiplication
    multiplyMatrices(m1, m2, result);

    // Display the result matrix
    displayMatrix(result, "Resulting Matrix");

    return 0;
}

void displayMatrix(int matrix[SIZE][SIZE], const char *name)
{
    printf("%s:\n", name);
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            printf("%4d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void multiplyMatrices(int m1[SIZE][SIZE], int m2[SIZE][SIZE], int result[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            result[i][j] = 0;
            for (int k = 0; k < SIZE; k++)
            {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}