#include "include/compiler.cpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Provide source file");
    return 1;
  }
  if (argc == 2) {
    char *source_file = argv[1];
    char *source_code = read_file_to_string(source_file);

    if (source_code) {
      // printf("Source code:\n%s\n", source_code);
      compile_code(source_file,
                   get_language_from_extension(read_extension(source_file)),
                   read_extension(source_file));
      free(source_code);
    } else {
      printf("Failed to read source file: %s\n", source_file);
    }
  }
}
