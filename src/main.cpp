#include "include/compiler.cpp"
#include <iostream>
#include <cstring>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: any-compiler <source-file>\n");
    printf("       any-compiler --version\n");
    printf("       any-compiler --help\n");
    return 1;
  }
  
  if (argc == 2) {
    char *arg = argv[1];
    
    // Handle flags
    if (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0) {
      printf("any-compiler version 1.0.0\n");
      return 0;
    }
    
    if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
      printf("any-compiler - Compile code in any language\n\n");
      printf("Usage:\n");
      printf("  any-compiler <source-file>    Compile and execute source file\n");
      printf("  any-compiler --version        Show version information\n");
      printf("  any-compiler --help           Show this help message\n\n");
      printf("Supported languages:\n");
      printf("  C, C++, Python, JavaScript, Java, Ruby, Go, Rust, PHP, C#\n\n");
      printf("Examples:\n");
      printf("  any-compiler hello.py\n");
      printf("  any-compiler main.rs\n");
      printf("  any-compiler script.js\n");
      return 0;
    }
    
    // Treat as source file
    char *source_file = arg;
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
