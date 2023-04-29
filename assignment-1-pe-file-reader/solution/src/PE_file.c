/// @file
/// @brief PE file utils

#include "PE_file.h"

#include <malloc.h>

/// @brief Free all memory allocated while creating/reading PE headers
/// @param[in] peFile Structure that contains headers info
void destroy_pe(struct PEFile* peFile) {
    free(peFile->section_headers);
}

