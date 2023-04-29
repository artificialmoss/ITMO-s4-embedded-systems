/// @file
/// @brief Program status options and print error util

#ifndef SECTION_EXTRACTOR_ERROR_HANDLER_H
#define SECTION_EXTRACTOR_ERROR_HANDLER_H


/// Program status
enum program_status {
    /// Success
    OK = 0,
    /// Wrong number of arguments
    WRONG_NUMBER_OF_ARGS,
    /// The first argument is not a valid (existing/readable) input path
    WRONG_INPUT_PATH,
    /// Couldn't close the input file
    CLOSE_INPUT_ERROR,
    /// The third argument is not a valid (writable) output path
    WRONG_OUTPUT_PATH,
    /// Couldn't close the output file
    CLOSE_OUTPUT_ERROR,
    /// Error occurred while reading headers
    READ_HEADERS_ERROR,
    /// Error occurred while extracting and writing the section of PE file
    WRITE_SECTION_ERROR
};

/// @brief Prints the error message to stderr
/// @param[in] error_message Error message
void print_error(char* error_message);

#endif //SECTION_EXTRACTOR_ERROR_HANDLER_H
