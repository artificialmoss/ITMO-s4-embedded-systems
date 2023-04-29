/// @file
/// @brief PE headers reader and section extractor implementation

#include "error_handler.h"
#include "PE_file.h"
#include "pe_reader.h"

#include <malloc.h>
#include <string.h>

/// @brief Validates the signature of PE file
/// @param[in] peFile Structure containing PE file info
/// @return True if the signature is valid, false otherwise
static inline bool validate_signature(struct PEFile* peFile) {
    return peFile->magic == SIGNATURE;
}

/// @brief Sets absolute values of header offsets. Requires a valid main header
/// @param[in] peFile Structure containing PE file info
/// @return The resulting PEFile structure
static inline struct PEFile* build_offsets(struct PEFile* peFile) {
    peFile->magic_offset = DDOS_OFFSET;
    peFile->optional_header_offset = peFile->header_offset + sizeof(peFile->magic) + sizeof(struct PEHeader);
    peFile->section_header_offset = peFile->optional_header_offset + peFile->header.opt_header_size;
    return peFile;
}

/// @brief Reads the main (COFF) header
/// @param[in] peFile Structure containing PE file info
/// @param[in] in Input file
/// @return True if the read has been successful, false if an error occurred
static inline bool read_main_header(FILE * in, struct PEFile* peFile) {
    if (fseek(in, DDOS_OFFSET, SEEK_SET)) {
        return false;
    }
    if (!fread(&peFile->header_offset, sizeof(peFile->header_offset), 1, in)) {
        return false;
    }
    if (fseek(in, peFile->header_offset, SEEK_SET)) {
        return false;
    }
    if (!fread(&peFile->magic, sizeof (peFile->magic), 1, in)) {
        return false;
    }
    if (!fread(&peFile->header, sizeof (peFile->header), 1, in)) {
        return false;
    }
    return true;
}

/// @brief Reads the optional header
/// @param[in] peFile Structure containing PE file info
/// @param[in] in Input file
/// @return True if the read has been successful, false if an error occurred
static inline bool read_optional_header(FILE* in, struct PEFile* peFile) {
    return fseek(in, peFile->optional_header_offset, SEEK_SET) == 0;
}

/// @brief Reads the section headers
/// @param[in] peFile Structure containing PE file info
/// @param[in] in Input file
/// @return True if the read has been successful, false if an error occurred
bool read_section_headers(FILE * in, struct PEFile* peFile) {
    size_t section_headers_size = sizeof(struct SectionHeader) * peFile->header.section_number;
    peFile->section_headers = malloc(section_headers_size);
    if (fseek(in, peFile->section_header_offset, SEEK_SET)) {
        free(peFile->section_headers);
        return false;
    }
    if (fread(peFile->section_headers, section_headers_size, 1, in) != 1) {
        free(peFile->section_headers);
        return false;
    }
    return true;
}

/// @brief Reads the PE file headers, checks if they are valid
/// @param[in] peFile Structure containing PE file info
/// @param[in] in Input file
/// @return The result of read (READ_OK or 0 if the read has been successful)
enum read_pe_result read_headers(FILE* in, struct PEFile* PEFile) {
    if (!read_main_header(in, PEFile)) {
        print_error("Error reading the main header.");
        return READ_ERROR;
    }
    if (!validate_signature(PEFile)) {
        print_error("Invalid signature.");
        fprintf(stderr, "Expected %d, received %" PRIu32 "\n", SIGNATURE, PEFile->magic);
        return INVALID_SIGNATURE;
    }
    PEFile = build_offsets(PEFile);
    if (!read_optional_header(in, PEFile)) {
        print_error("Error reading the optional header.");
        return READ_ERROR;
    }
    if (!read_section_headers(in, PEFile)) {
        print_error("Error reading the section headers.");
        return READ_ERROR;
    }
    return READ_OK;
}

/// @brief Looks for the header of the specified section
/// @param[in] peFile Structure containing PE file info
/// @param[in] section_name The name of the section
/// @return The section header that corresponds to the specified section. NULL if the section hasn't been found
static inline struct SectionHeader* find_section(char const* section_name, struct PEFile const* peFile) {
    for (size_t i = 0; i < peFile->header.section_number; i++) {
        if (!strcmp(peFile->section_headers[i].section_name, section_name)) {
            return &peFile->section_headers[i];
        }
    }
    return NULL;
}

/// @brief Writes section data to output from buffer
/// @param[in] out Output file
/// @param[in] sectionHeader The header of the section
/// @param[in] section_data Buffer of the size specified in the section header, contains section data
/// @return True if the write has been successful, false if an error occurred
static inline bool write_section_data(FILE* out, char* section_data, struct SectionHeader* sectionHeader) {
    return fwrite(section_data, sectionHeader->raw_data_size, 1, out) == 1;
}

/// @brief Reads section data from input from buffer
/// @param[in] in Input file
/// @param[in] sectionHeader The header of the section
/// @param[in] section_data Buffer of the size specified in the section header
/// @return True if the read has been successful, false if an error occurred
static inline bool read_section_data(struct SectionHeader* sectionHeader, char* section_data, FILE * in) {
    if (fseek(in, sectionHeader->raw_data_ptr, SEEK_SET)) {
        return false;
    }
    return fread(section_data, sectionHeader->raw_data_size, 1, in) == 1;
}

/// @brief Writes the specified section of PE file to another file.
/// @param[in] in Input file
/// @param[in] out Output file
/// @param[in] PEFile PE file info
/// @param[in] section_name The name of the section
/// @return The result of writing the section (WRITE_OK or 0 if the writing has been successful)
enum write_section_status write_section(FILE* in, FILE* out, struct PEFile* peFile, char* section_name) {
    struct SectionHeader* section_header = find_section(section_name, peFile);
    if (!section_header) {
        print_error("No section with this name found.");
        return NO_SUCH_SECTION;
    }
    char* section_data = malloc(section_header->raw_data_size);
    if (!section_data) {
        print_error("Not enough free memory to allocate section data.");
        return NO_MEMORY;
    }
    if (!read_section_data(section_header, section_data, in)) {
        print_error("Read section data error.");
        free(section_data);
        return READ_SECTION_ERROR;
    }
    if (!write_section_data(out, section_data, section_header)) {
        print_error("Write section data error.");
        free(section_data);
        return WRITE_ERROR;
    }
    free(section_data);
    return WRITE_OK;
}
