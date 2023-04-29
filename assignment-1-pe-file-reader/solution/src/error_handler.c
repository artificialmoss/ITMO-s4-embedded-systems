///@file
/// @brief Print error util

#include "error_handler.h"

#include <stdio.h>

/// @brief Prints the error message to stderr
/// @param[in] error_message Error message
void print_error(char* error_message) {
    fprintf(stderr, "%s\n", error_message);
}
