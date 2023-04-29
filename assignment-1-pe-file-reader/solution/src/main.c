/// @file 
/// @brief Main application file

#include "error_handler.h"
#include "PE_file.h"
#include "pe_reader.h"

#include <stdio.h>

/// Application name string
#define APP_NAME "section-extractor"

/// @brief Print usage test
/// @param[in] f File to print to (e.g., stdout)
void usage(FILE *f)
{
  fprintf(f, "Usage: " APP_NAME " <in_file> <section_name> <out_file>\n");
}

/// @brief Application entry point
/// @param[in] argc Number of command line arguments
/// @param[in] argv Command line arguments
/// @return 0 in case of success or error code
int main(int argc, char** argv)
{
  (void) argc; (void) argv; // supress 'unused parameters' warning

  if (argc != 4) {
      usage(stdout);
      return WRONG_NUMBER_OF_ARGS;
  }
  char* input_filepath = argv[1];
  char* section = argv[2];
  char* output_filepath = argv[3];
  FILE *input = fopen(input_filepath, "rb");
  if (!input) {
      print_error("Wrong input path: the first argument must specify a readable file.");
      return WRONG_INPUT_PATH;
  }

  struct PEFile peFile = {0};

  if (read_headers(input, &peFile) != READ_OK) {
      print_error("An error occured while reading PE headers.");
      fclose(input);
      return READ_HEADERS_ERROR;
  }

  FILE* output = fopen(output_filepath, "wb");
  if (!output) {
      print_error("Wrong output path: the third argument must specify a path to a writable file.");
      destroy_pe(&peFile);
      fclose(input);
      return WRONG_OUTPUT_PATH;
  }

  if (write_section(input, output, &peFile, section) != WRITE_OK) {
      print_error("Couldn't write the section to the file.");
      destroy_pe(&peFile);
      fclose(input);
      fclose(output);
      return WRITE_SECTION_ERROR;
  }

  destroy_pe(&peFile);

  if (fclose(input)) {
      print_error("Couldn't close the input file.");
      return CLOSE_INPUT_ERROR;
  }

  if (fclose(output)) {
      print_error("Couldn't close the output file.");
      return CLOSE_OUTPUT_ERROR;
  }

  return 0;
}
