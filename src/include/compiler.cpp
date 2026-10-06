#include <iostream>
#include "languages.cpp"
#include "file_helper.cpp"
#include "http_helper.cpp"

int compile_code(const char *source_file, const char *language,
                 const char *extension) {
  char *source_code = read_file_to_string(source_file);
  if (source_code) {
    int result = post_source_code(source_code, language, extension);
    free(source_code);
    return result;
  } else {
    fprintf(stderr, "Failed to read source file: %s\n", source_file);
    return 1;
  }
}