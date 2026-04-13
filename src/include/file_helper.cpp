#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>

#define c 1
#define cpp 2
#define py 3
#define js 4
#define java 5
#define rb 6
#define go 7
#define rs 8
#define php 9
#define cs 10

const char *LANGUAGES[11] = {"",           "c",    "cpp",  "python",
                             "javascript", "java", "ruby", "go",
                             "rust",       "php",  "c#"};

const int BUFFER_SIZE = 0x100;

char *read_file_to_string(const char *filename_with_path) {
  FILE *fp = fopen(filename_with_path, "rb");
  if (!fp) {
    perror("Error opening file");
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  long length = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *buffer = (char *)malloc(length + 1);
  if (!buffer) {
    fclose(fp);
    return NULL;
  }

  size_t bytes_read = fread(buffer, 1, length, fp);
  if (bytes_read != (size_t)length) {
    free(buffer);
    fclose(fp);
    return NULL;
  }

  // printf("Read %zu bytes from file\n", bytes_read);
  buffer[length] = '\0'; // Null-terminate

  fclose(fp);
  return buffer;
}
char *read_extension(char *filename) {
  char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}

int extension_to_language_id(char *extension) {
  if (strcmp(extension, "c") == 0)
    return c;
  else if (strcmp(extension, "cpp") == 0 || strcmp(extension, "cc") == 0 ||
           strcmp(extension, "cxx") == 0)
    return cpp;
  else if (strcmp(extension, "py") == 0)
    return py;
  else if (strcmp(extension, "js") == 0)
    return js;
  else if (strcmp(extension, "java") == 0)
    return java;
  else if (strcmp(extension, "rb") == 0)
    return rb;
  else if (strcmp(extension, "go") == 0)
    return go;
  else if (strcmp(extension, "rs") == 0)
    return rs;
  else if (strcmp(extension, "php") == 0)
    return php;
  else if (strcmp(extension, "cs") == 0)
    return cs;
  else
    return -1; // Unknown language
}

const char *get_language_from_extension(char *extension) {
  if (extension == NULL || extension_to_language_id(extension) == -1) {
    return "Unknown";
  }
  return LANGUAGES[extension_to_language_id(extension)];
}
