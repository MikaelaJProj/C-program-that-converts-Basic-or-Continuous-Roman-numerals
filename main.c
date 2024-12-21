#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function prototypes
int romanToDecimal(char *roman);           // Function to convert Roman numeral to decimal
int performOperation(int num1, int num2, char operation); // Function to perform arithmetic operations
void numberToWords(int num, char *output); // Function to convert numbers to words
void processInputFile(const char *inputFile, const char *outputFile); // Function to process input and generate output
void writeOutput(FILE *outputFile, int result); // Function to write the output result in words
int isValidRoman(char *roman); // Function to validate if the Roman numeral is valid
void cleanString(char *str);  // Function to clean the input string

// Main function
int main() {
    const char *inputFile = "Input.txt";  // Name of the input file
    const char *outputFile = "Output.txt"; // Name of the output file

    // Process the input file and generate the output file
    processInputFile(inputFile, outputFile);

    printf("Processing Completed. Check %s for Results.\n", outputFile); // Print completion message

    return 0;  // End the program
}

// Function to convert Roman numeral to decimal
int romanToDecimal(char *roman) {
    int total = 0, current = 0, next = 0;  // Variables to hold the total, current and next Roman numeral values
    int length = strlen(roman); // Get the length of the Roman numeral string

    for (int i = 0; i < length; i++) { // Loop through each character in the Roman numeral string
        switch (toupper(roman[i])) {  // Convert character to uppercase and check its value
            case 'I': current = 1; break;
            case 'V': current = 5; break;
            case 'X': current = 10; break;
            case 'L': current = 50; break;
            case 'C': current = 100; break;
            case 'D': current = 500; break;
            case 'M': current = 1000; break;
            default: return -1; // Return -1 if an invalid character is encountered
        }

        // Check the value of the next Roman numeral character (if any)
        if (i + 1 < length) {
            switch (toupper(roman[i + 1])) {
                case 'I': next = 1; break;
                case 'V': next = 5; break;
                case 'X': next = 10; break;
                case 'L': next = 50; break;
                case 'C': next = 100; break;
                case 'D': next = 500; break;
                case 'M': next = 1000; break;
                default: next = 0;
            }
        } else {
            next = 0; // End of string, set next to 0
        }

        // If current numeral is smaller than the next one, subtract its value, otherwise add it
        if (current < next) {
            total -= current;
        } else {
            total += current;
        }
    }
    return total; // Return the decimal value
}

// Function to perform arithmetic operations
int performOperation(int num1, int num2, char operation) {
    switch (operation) {  // Check which operation to perform
        case '+': return num1 + num2;  // Addition
        case '-': return num1 - num2;  // Subtraction
        case '*': return num1 * num2;  // Multiplication
        case '/': return (num2 != 0) ? num1 / num2 : -1; // Division (check for division by zero)
        default: return -1; // Invalid operation
    }
}

// Function to convert number to words
void numberToWords(int num, char *output) {
    // Arrays for units, tens, teens, and large place values
    const char *units[] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    const char *teens[] = {"Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    const char *tens[] = {"", "Ten", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    const char *thousands[] = {"", "Thousand", "Million", "Billion"};

    if (num == 0) {  // If the number is 0, return "Zero"
        strcpy(output, "Zero");
        return;
    }

    char buffer[256] = "";  // Buffer to hold the word representation
    char temp[256] = "";  // Temporary buffer
    int isNegative = (num < 0);  // Check if the number is negative
    num = abs(num); // Convert number to positive for processing

    int chunk, place = 0; // Chunk to hold groups of thousands, and place value (e.g., thousand, million)

    // Process the number in chunks of 1000
    while (num > 0) {
        chunk = num % 1000;  // Get the last 3 digits

        if (chunk > 0) {  // If chunk is non-zero, convert it to words
            char chunkWords[256] = "";

            if (chunk / 100 > 0) {  // If there's a hundreds place, add it to the words
                strcat(chunkWords, units[chunk / 100]);
                strcat(chunkWords, " Hundred ");
            }

            int remainder = chunk % 100;
            if (remainder > 10 && remainder < 20) {  // Handle numbers between 11-19
                strcat(chunkWords, teens[remainder - 11]);
            } else {
                if (remainder / 10 > 0) {  // Handle tens place
                    strcat(chunkWords, tens[remainder / 10]);
                    strcat(chunkWords, " ");
                }
                if (remainder % 10 > 0) {  // Handle ones place
                    strcat(chunkWords, units[remainder % 10]);
                }
            }

            if (place > 0) {  // Add the place value (e.g., thousand, million)
                strcat(chunkWords, " ");
                strcat(chunkWords, thousands[place]);
            }

            snprintf(temp, sizeof(temp), "%s %s", chunkWords, buffer);  // Append current chunk to buffer
            strcpy(buffer, temp);
        }

        num /= 1000;  // Move to the next chunk (thousands, millions, etc.)
        place++;  // Move to the next place value
    }

    snprintf(output, 256, "%s%s", isNegative ? "Negative " : "", buffer); // If negative, prepend "Negative"
    output[strcspn(output, "\n")] = '\0';  // Remove any trailing newline character
}

// Function to process input file
void processInputFile(const char *inputFile, const char *outputFile) {
    FILE *input = fopen(inputFile, "r");  // Open the input file for reading
    FILE *output = fopen(outputFile, "w"); // Open the output file for writing

    if (!input || !output) {  // If file opening fails, print an error and exit
        printf("Error opening file(s).\n");
        exit(1);
    }

    char line[256];  // Array to hold each line from the input file
    while (fgets(line, sizeof(line), input)) {  // Read each line from the input file
        cleanString(line);  // Clean the line to remove any invalid characters
        char roman1[50], roman2[50], operation;  // Variables to hold the two Roman numerals and the operator
        int num1, num2, result;  // Variables to hold the decimal values and result

        // Try to extract the Roman numerals and operator from the line
        if (sscanf(line, "%49s %c %49s", roman1, &operation, roman2) == 3) {
            if (!isValidRoman(roman1) || !isValidRoman(roman2)) {  // If Roman numerals are invalid, log error
                fprintf(output, "Invalid Roman numeral in line: %s\n", line);
                continue;  // Skip to the next line
            }

            // Convert Roman numerals to decimal
            num1 = romanToDecimal(roman1);
            num2 = romanToDecimal(roman2);

            // Perform the arithmetic operation
            result = performOperation(num1, num2, operation);

            if (result == -1) {  // If the operation is invalid (e.g., division by zero), log error
                fprintf(output, "Invalid operation in line: %s\n", line);
            } else {
                writeOutput(output, result);  // Write the result in words
            }
        } else {
            fprintf(output, "Invalid input format in line: %s\n", line);  // Log format error
        }
    }

    fclose(input);  // Close the input file
    fclose(output); // Close the output file
}

// Function to validate Roman numerals
int isValidRoman(char *roman) {
    for (int i = 0; roman[i]; i++) {  // Loop through each character in the Roman numeral
        char c = toupper(roman[i]);  // Convert character to uppercase
        if (c != 'I' && c != 'V' && c != 'X' && c != 'L' && c != 'C' && c != 'D' && c != 'M') {
            return 0;  // Return 0 if the character is invalid
        }
    }
    return 1;  // Return 1 if the Roman numeral is valid
}

// Function to clean input strings
void cleanString(char *str) {
    char *ptr = str;
    while (*ptr) {  // Loop through each character in the string
        if (isprint(*ptr) || isspace(*ptr)) {  // If the character is printable or whitespace, move to the next character
            ptr++;
        } else {
            *ptr = '\0';  // If the character is invalid, terminate the string
            break;
        }
    }
}

// Function to write output
void writeOutput(FILE *outputFile, int result) {
    char resultInWords[256];  // Buffer to hold the result in words
    numberToWords(result, resultInWords);  // Convert the result to words
    fprintf(outputFile, "%s\n", resultInWords);  // Write the result to the output file
}
