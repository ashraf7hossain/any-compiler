#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>

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
