#include <stdio.h>
#include <stdlib.h>

#define MAX_COMMAND_LENGTH 0x100

const char *BASE_URL = "https://api.github.com/zen";

void make_http_call()
{
  char curl_command[256];
  sprintf(curl_command, "curl -s %s", BASE_URL);
  FILE *f_curl = popen(curl_command, "r");

  if (f_curl == NULL)
  {
    fprintf(stderr, "Error opening pipe to curl\n");
    exit(1);
  }

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), f_curl))
  {
    printf("%s", buffer);
  }

  pclose(f_curl);
}
