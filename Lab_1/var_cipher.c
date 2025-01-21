/*-------------------------------------------------------------------------
 * File:        var_caesar.c
 * Function:    Variable Caesar Cipher
 * Description: ...
 * Input:       msg[]
 * Output:      Encrypted message using variable caesar cipher
 * Author(s):   Anshika Sinha
 * Date:        01/13/2025
 *-----------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

void caesarCipher(char *msg); // Prototype of caesarCipher()

int main(){
    char msg[]= "Hello all, welcome to CPE325 Spring 2025!"; // Hard-coded message
    printf("Original message: %s\n", msg); // Output original message
    caesarCipher(msg); // Call function to encrypt message with variable Caesar cipher
    printf("Encrypted message: %s\n", msg); // Output encrypted message

    return 0;
}

// This function encrypts the given string using a variable caesar cipher
void caesarCipher(char *msg) {
    // Loop through each character in the given string
    for (int i = 0; msg[i] != '\0'; i++) {
        // Calculate shift
        int shift = i + 1;

        // Check if the character at index is a letter
        if (isalpha(msg[i])) {
            // Find base according to ASCII hex value
            char base = islower(msg[i]) ? 'a' : 'A';
            // Execute shift
            msg[i] = (msg[i] - base + shift) % 26 + base;
        }
    }
}
