#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>

#define BUFFER_SIZE 0x100

// std::unordered_map<std::string, std::string> extension_to_language = {
//     {"c", "C"},       {"cpp", "C++"}, {"py", "Python"}, {"js", "JavaScript"},
//     {"java", "Java"}, {"rb", "Ruby"}, {"go", "Go"},     {"rs", "Rust"},
//     {"php", "PHP"},   {"cs", "C#"},
// };

// read_file(const char *filename)
// {
//   FILE *file = fopen(filename, "r");
//   if (file == NULL)
//   {
//     perror("Could not open file");
//     return;
//   }

//   char buffer[BUFFER_SIZE];
//   char *ret while (fgets(buffer, sizeof(buffer), file))
//   {
//     printf("%s", buffer);
//   }

//   fclose(file);
// }

#include <stdio.h>
#include <stdlib.h>

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

  printf("Read %zu bytes from file\n", bytes_read);
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
