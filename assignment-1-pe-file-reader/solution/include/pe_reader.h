/// @file
/// @brief PE reader header file

#ifndef SECTION_EXTRACTOR_PE_READER_H
#define SECTION_EXTRACTOR_PE_READER_H

#include "PE_file.h"

#include <stdio.h>

/// Read PEFile result
enum read_pe_result {
    /// Success
    READ_OK = 0,
    /// Unknown error occurred while reading a file
    READ_ERROR,
    /// Invalid file signature
    INVALID_SIGNATURE
};

/// Write selected section result
enum write_section_status {
    /// Success
    WRITE_OK = 0,
    /// Unknown error occurred while writing to a file
    WRITE_ERROR,
    /// The specified section was not found
    NO_SUCH_SECTION,
    /// The program ran out of memory
    NO_MEMORY,
    /// Unknown error occurred while reading from a file or
    READ_SECTION_ERROR
};

/// @brief Reads the PE file headers, checks if they are valid
/// @param[in] peFile Structure containing PE file info
/// @param[in] in Input file
/// @return The result of read (READ_OK or 0 if the read has been successful)
enum read_pe_result read_headers(FILE* in, struct PEFile* PEFile);

/// @brief Writes the specified section of PE file to another file.
/// @param[in] in Input file
/// @param[in] out Output file
/// @param[in] PEFile PE file info
/// @param[in] section_name The name of the section
/// @return The result of writing the section (WRITE_OK or 0 if the writing has been successful)
enum write_section_status write_section(FILE* in, FILE* out, struct PEFile* peFile, char* section_name);


#endif //SECTION_EXTRACTOR_PE_READER_H
