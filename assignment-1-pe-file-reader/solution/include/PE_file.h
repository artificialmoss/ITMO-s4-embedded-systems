/// @file
/// @brief PE file headers content

#ifndef SECTION_EXTRACTOR_PE_FILE_H
#define SECTION_EXTRACTOR_PE_FILE_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

/// Offset to the signature
#define DDOS_OFFSET 0x3c
/// Signature ('PE\0\0')
#define SIGNATURE 0x4550

#ifdef _MSC_VER
    #pragma pack(push, 1)
#endif

/// Structure containing COFF file header data
struct
#ifdef __GNUC__
        __attribute__((packed))
#endif
PEHeader
{
    /// The number that identifies the type of target machine
    uint16_t machine;
    /// The number of sections. This indicates the size of the section table, which immediately follows the headers
    uint16_t section_number;
    /// The low 32 bits of the number of seconds since 00:00 January 1, 1970 (a C run-time time_t value), which indicates when the file was created
    uint32_t timestamp;
    /// The file offset of the COFF symbol table, or zero if no COFF symbol table is present. Should be zero for images
    uint32_t sym_table_pointer;
    /// The number of entries in the symbol table
    uint32_t symbols_number;
    /// The size of the optional header, which is required for executable files but not for object files
    uint16_t opt_header_size;
    /// The flags that indicate the attributes of the file
    uint16_t characteristics;
};

/// Structure containing optional header data (standard fields only)
struct
#ifdef __GNUC__
        __attribute__((packed))
#endif
OptionalHeader
{
    /// The unsigned integer that identifies the state of the image file
    uint16_t magic;
    /// The linker major version number
    uint8_t linker_major_ver;
    /// The linker minor version number
    uint8_t linker_minor_ver;
    /// The size of the code (text) section, or the sum of all code sections if there are multiple sections
    uint32_t code_size;
    /// The size of the initialized data section, or the sum of all such sections if there are multiple data sections
    uint32_t data_size;
    /// The size of the uninitialized data section (BSS), or the sum of all such sections if there are multiple BSS sections
    uint32_t bss_size;
    /// The address of the entry point relative to the image base when the executable file is loaded into memory
    uint32_t entry_point_addr;
    /// The address that is relative to the image base of the beginning-of-code section when it is loaded into memory
    uint32_t code_base_addr;
};


/// Structure containing a single section header data
struct
#ifdef __GNUC__
        __attribute__((packed))
#endif
SectionHeader
{
    /// Section name. Must be An 8-byte, null-padded UTF-8 encoded string
    char section_name[8];
    /// The total size of the section when loaded into memory
    uint32_t section_virtual_size;
    /// For executable images, the address of the first byte of the section relative to the image base when the section is loaded into memory
    uint32_t virtual_addr;
    /// The size of the section (for object files) or the size of the initialized data on disk (for image files)
    uint32_t raw_data_size;
    /// The file pointer to the first page of the section within the COFF file
    uint32_t raw_data_ptr;
    /// The file pointer to the beginning of relocation entries for the section
    uint32_t reloc_ptr;
    /// The file pointer to the beginning of line-number entries for the section
    uint32_t line_num_ptr;
    /// The number of relocation entries for the section
    uint16_t reloc_number;
    /// The number of line-number entries for the section
    uint16_t line_num_number;
    /// The flags that describe the characteristics of the section
    uint32_t characteristics;
};


/// Structure containing PE file data
struct
#ifdef __GNUC__
        __attribute__((packed))
#endif
PEFile
{
    /// @name Offsets within file
    ///@{

    /// Offset to a file magic
    uint32_t magic_offset;
    /// Offset to a main PE header
    uint32_t header_offset;
    /// Offset to an optional header
    uint32_t optional_header_offset;
    /// Offset to a section table
    uint32_t section_header_offset;
    ///@}

    /// @name File headers
    ///@{

    /// File magic
    uint32_t magic;
    /// Main header
    struct PEHeader header;
    /// Optional header
    struct OptionalHeader optional_header;
    /// Array of section headers with the size of header.sections_number
    struct SectionHeader *section_headers;
    ///@}
};

#ifdef _MSK_VER
    #pragma pack(pop)
#endif

/// @brief Free all memory allocated while creating/reading PE headers
/// @param[in] peFile Structure that contains headers info
void destroy_pe(struct PEFile* peFile);

#endif //SECTION_EXTRACTOR_PE_FILE_H
