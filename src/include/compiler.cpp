#include "file_helper.cpp"
#include "http_helper.cpp"
#include <iostream>

void compile_code(const char *source_file, const char *language,
                  const char *extension) {
  char *source_code = read_file_to_string(source_file);
  if (source_code) {
    post_source_code(source_code, language, extension);
    free(source_code);
  } else {
    printf("Failed to read source file: %s\n", source_file);
  }
}