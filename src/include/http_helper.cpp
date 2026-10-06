#include "json_helper.cpp"
#include <stdio.h>
#include <stdlib.h>

// #define MAX_COMMAND_LENGTH 0x100
#define MAX_COMMAND_LENGTH 0xFFFF
#define MAX_PAYLOAD_SIZE 0x20000
#define BUFFER_SIZE 0x20000

const char *BASE_URL = "https://onecompiler.com/api/code/exec";

const char *REQUEST_STR = "";

int make_http_call(const char *request) {
  FILE *f_curl = popen(request, "r");

  if (f_curl == NULL) {
    fprintf(stderr, "Error opening pipe to curl\n");
    return 1;
  }

  int response_received = 0;
  char buffer[BUFFER_SIZE];
  while (fgets(buffer, sizeof(buffer), f_curl)) {
    if (buffer[0] == '{') {
      SampleStruct *sample = parse_sample_struct(buffer);
      if (sample != NULL) {
        response_received = 1;
        printf("parsed status: %d (%s)\n", sample->status.id,
               sample->status.description ? sample->status.description
                                          : "unknown");
        printf("parsed stdout: %s\n",
               sample->stdout_text ? sample->stdout_text : "(null)");
        printf("parsed stderr: %s\n",
               sample->stderr_text ? sample->stderr_text : "(null)");
        printf("parsed compile_output: %s\n",
               sample->compile_output ? sample->compile_output : "(null)");
        free_sample_struct(sample);
      }
    }
  }

  int curl_status = pclose(f_curl);
  if (!response_received) {
    fprintf(stderr,
            "No usable response from OneCompiler. Check your internet "
            "connection and try again.\n");
  }
  if (curl_status != 0) {
    fprintf(stderr, "The OneCompiler request failed (curl status %d).\n",
            curl_status);
    return 1;
  }
  return response_received ? 0 : 1;
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

int post_source_code(const char *source_code, const char *language,
                     const char *extension) {
  if (source_code == NULL) {
    fprintf(stderr, "Source code is null\n");
    return 1;
  }

  char payload[MAX_PAYLOAD_SIZE];
  char *escaped_code = escape_for_json(source_code);
  if (escaped_code == NULL) {
    fprintf(stderr, "Failed to escape source code\n");
    return 1;
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
    return 1;
  }

  char request_curl[MAX_COMMAND_LENGTH];
  snprintf(request_curl, sizeof(request_curl),
           "curl -sS -X POST \"%s\" "
           "-H \"Content-Type: application/json\" "
           "--data-binary \"@%s\"",
           BASE_URL, payload_file);

  int result = make_http_call(request_curl);

  remove(payload_file);

  free(escaped_code);
  return result;
}
