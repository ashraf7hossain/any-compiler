#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *escape_for_json(const char *input) {
  if (input == NULL) {
    return NULL;
  }
  char *escaped = (char *)malloc(strlen(input) * 4 + 1);
  if (escaped == NULL) {
    return NULL;
  }

  char *p = escaped;
  while (*input) {
    if (*input == '"') {
      strcpy(p, "\\\"");
      p += 2;
    } else if (*input == '\\') {
      strcpy(p, "\\\\");
      p += 2;
    } else if (*input == '\n') {
      strcpy(p, "\\n");
      p += 2;
    } else if (*input == '\r') {
      strcpy(p, "\\r");
      p += 2;
    } else {
      *p++ = *input;
    }
    input++;
  }
  *p = '\0';
  return escaped;
}
