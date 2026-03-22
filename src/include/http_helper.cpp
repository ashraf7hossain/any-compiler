#include "json_helper.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 0xFFFF
#define MAX_PAYLOAD_SIZE 0x20000
#define BUFFER_SIZE 0x20000

const char *BASE_URL = "https://onecompiler.com/api/code/exec";

void make_http_call(const char *request) {

  // printf("Executing command:\n%s\n", request);

  FILE *f_curl = popen(request, "r");

  if (f_curl == NULL) {
    fprintf(stderr, "Error opening pipe to curl\n");
    exit(1);
  }

  char buffer[BUFFER_SIZE];
  while (fgets(buffer, sizeof(buffer), f_curl)) {
    printf("%s", buffer);
  }

  pclose(f_curl);
}

int write_text_file(const char *path, const char *content) {
  FILE *fp = fopen(path, "wb");
  if (fp == NULL) {
    return 0;
  }

  size_t content_len = strlen(content);
  size_t written = fwrite(content, 1, content_len, fp);
  fclose(fp);

  return written == content_len;
}

void post_source_code(const char *source_code, const char *language,
                      const char *extension) {
  if (source_code == NULL) {
    fprintf(stderr, "Source code is null\n");
    return;
  }

  char payload[MAX_PAYLOAD_SIZE];
  char *escaped_code = escape_for_json(source_code);
  if (escaped_code == NULL) {
    fprintf(stderr, "Failed to escape source code\n");
    return;
  }

  snprintf(payload, sizeof(payload),
           "{\"properties\":{\"language\":\"%s\","
           "\"files\":[{\"name\":\"main.%s\","
           "\"content\":\"%s\"}],"
           "\"stdin\":\"202\"}}",
           language, extension, escaped_code);

  const char *payload_file = "request_payload.json";
  if (!write_text_file(payload_file, payload)) {
    fprintf(stderr, "Failed to write payload file\n");
    free(escaped_code);
    return;
  }

  char request_curl[MAX_COMMAND_LENGTH];
  snprintf(request_curl, sizeof(request_curl),
           "curl -s -X POST \"%s\" "
           "-H \"Content-Type: application/json\" "
           "--data-binary \"@%s\"",
           BASE_URL, payload_file);

  make_http_call(request_curl);

  remove(payload_file);

  free(escaped_code);
}