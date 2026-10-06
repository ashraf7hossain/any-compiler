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

  if (length >= 2 &&
      (((unsigned char)buffer[0] == 0xFF &&
        (unsigned char)buffer[1] == 0xFE) ||
       ((unsigned char)buffer[0] == 0xFE &&
        (unsigned char)buffer[1] == 0xFF))) {
    int little_endian = (unsigned char)buffer[0] == 0xFF;
    if ((length - 2) % 2 != 0) {
      fprintf(stderr, "Invalid UTF-16 source file: %s\n", filename_with_path);
      free(buffer);
      fclose(fp);
      return NULL;
    }

    char *utf8 = (char *)malloc((size_t)length * 2 + 1);
    if (!utf8) {
      free(buffer);
      fclose(fp);
      return NULL;
    }

    size_t output_length = 0;
    size_t input_index = 2;
    while (input_index < (size_t)length) {
      unsigned int first = (unsigned char)buffer[input_index++];
      unsigned int second = (unsigned char)buffer[input_index++];
      unsigned int codepoint = little_endian ? first | (second << 8)
                                            : (first << 8) | second;

      if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
        if (input_index >= (size_t)length) {
          output_length = 0;
          break;
        }
        first = (unsigned char)buffer[input_index++];
        second = (unsigned char)buffer[input_index++];
        unsigned int low = little_endian ? first | (second << 8)
                                         : (first << 8) | second;
        if (low < 0xDC00 || low > 0xDFFF) {
          output_length = 0;
          break;
        }
        codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
      } else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
        output_length = 0;
        break;
      }

      if (codepoint <= 0x7F) {
        utf8[output_length++] = (char)codepoint;
      } else if (codepoint <= 0x7FF) {
        utf8[output_length++] = (char)(0xC0 | (codepoint >> 6));
        utf8[output_length++] = (char)(0x80 | (codepoint & 0x3F));
      } else if (codepoint <= 0xFFFF) {
        utf8[output_length++] = (char)(0xE0 | (codepoint >> 12));
        utf8[output_length++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        utf8[output_length++] = (char)(0x80 | (codepoint & 0x3F));
      } else {
        utf8[output_length++] = (char)(0xF0 | (codepoint >> 18));
        utf8[output_length++] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        utf8[output_length++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        utf8[output_length++] = (char)(0x80 | (codepoint & 0x3F));
      }
    }

    if (output_length == 0 && length > 2) {
      fprintf(stderr, "Invalid UTF-16 source file: %s\n", filename_with_path);
      free(utf8);
      free(buffer);
      fclose(fp);
      return NULL;
    }
    utf8[output_length] = '\0';
    free(buffer);
    fclose(fp);
    return utf8;
  }

  if (length >= 3 && (unsigned char)buffer[0] == 0xEF &&
      (unsigned char)buffer[1] == 0xBB &&
      (unsigned char)buffer[2] == 0xBF) {
    memmove(buffer, buffer + 3, (size_t)length - 3);
    length -= 3;
  }

  // printf("Read %zu bytes from file\n", bytes_read);
  buffer[length] = '\0'; // Null-terminate

  fclose(fp);
  return buffer;
}
const char *read_extension(char *filename) {
  char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}
