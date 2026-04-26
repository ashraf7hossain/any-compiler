#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* Keep original escape_for_json function (used when producing JSON) */
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


/* Skip whitespace characters, return pointer to first non-space (or '\0') */
static const char *skip_ws(const char *p) {
  while (p && *p && isspace((unsigned char)*p)) p++;
  return p;
}

/*
  Find the position of the value for a given key in a flat JSON object.

  Returns pointer to the character immediately after the ':' that separates key and value,
  or NULL if not found.

  The function searches for "key" (with quotes) and ensures the next non-space char after
  the closing quote is ':' (allowing whitespace between them).
*/
static const char *find_key_value_pos(const char *json, const char *key) {
  if (!json || !key) return NULL;
  size_t keylen = strlen(key);
  const char *p = json;

  while ((p = strchr(p, '\"')) != NULL) {
    p++; /* point after opening quote */
    if (strncmp(p, key, keylen) == 0 && p[keylen] == '\"') {
      /* candidate match; make sure it's a key (i.e., next non-space char after the closing quote is ':') */
      const char *after_quote = p + keylen + 1;
      after_quote = skip_ws(after_quote);
      if (*after_quote == ':') {
        return after_quote + 1;
      }
    }
    /* continue searching after this quote */
    p++;
  }
  return NULL;
}

/* Unescape a JSON string fragment (handles a few common escapes). Returns newly allocated string. */
static char *json_unescape(const char *start, size_t len) {
  if (!start) return NULL;
  char *out = (char *)malloc(len + 1); /* worst-case */
  if (!out) return NULL;
  char *o = out;
  const char *end = start + len;
  for (const char *s = start; s < end; ++s) {
    if (*s == '\\' && (s + 1) < end) {
      s++;
      switch (*s) {
        case '\"': *o++ = '\"'; break;
        case '\\': *o++ = '\\'; break;
        case '/':  *o++ = '/';  break;
        case 'b':  *o++ = '\b'; break;
        case 'f':  *o++ = '\f'; break;
        case 'n':  *o++ = '\n'; break;
        case 'r':  *o++ = '\r'; break;
        case 't':  *o++ = '\t'; break;
        case 'u':
          /* Very small support: skip the \uXXXX sequence and output '?' as placeholder */
          if ((end - s) >= 5) {
            s += 4; /* skip XXXX */
            *o++ = '?';
          } else {
            *o++ = '?';
          }
          break;
        default:
          /* unknown escape -> keep as-is */
          *o++ = *s;
          break;
      }
    } else {
      *o++ = *s;
    }
  }
  *o = '\0';
  /* shrink allocation (optional) */
  char *shrunk = (char *)realloc(out, (size_t)(o - out) + 1);
  return shrunk ? shrunk : out;
}

/* Parse a JSON string value starting at quote. Returns allocated C string. */
static char *parse_json_string_value(const char *p) {
  p = skip_ws(p);
  if (!p || *p != '\"') return NULL;
  p++; /* skip opening quote */
  const char *start = p;
  while (*p) {
    if (*p == '\\') {
      /* escape - skip next char */
      if (*(p + 1)) p += 2;
      else break;
    } else if (*p == '\"') {
      /* end of string */
      size_t len = (size_t)(p - start);
      char *result = json_unescape(start, len);
      return result;
    } else {
      p++;
    }
  }
  return NULL; /* unterminated string */
}

/* Parse a JSON object value starting at '{'. Returns allocated JSON substring. */
static char *parse_json_object_value(const char *p) {
  p = skip_ws(p);
  if (!p || *p != '{') return NULL;

  const char *start = p;
  int depth = 0;
  int in_string = 0;
  int escaped = 0;

  while (*p) {
    if (in_string) {
      if (escaped) {
        escaped = 0;
      } else if (*p == '\\') {
        escaped = 1;
      } else if (*p == '"') {
        in_string = 0;
      }
    } else {
      if (*p == '"') {
        in_string = 1;
      } else if (*p == '{') {
        depth++;
      } else if (*p == '}') {
        depth--;
        if (depth == 0) {
          size_t len = (size_t)(p - start + 1);
          char *result = (char *)malloc(len + 1);
          if (!result) return NULL;
          memcpy(result, start, len);
          result[len] = '\0';
          return result;
        }
      }
    }
    p++;
  }

  return NULL;
}

/* Parse an unquoted JSON value (number, true, false, null). Returns pointer to start and length via out parameters */
static void parse_json_literal(const char *p, const char **out_start, size_t *out_len) {
  p = skip_ws(p);
  const char *start = p;
  if (!p || !*p) {
    *out_start = NULL;
    *out_len = 0;
    return;
  }

  if (*p == 't') { /* true */
    if (strncmp(p, "true", 4) == 0) {
      *out_start = p;
      *out_len = 4;
      return;
    }
  } else if (*p == 'f') { /* false */
    if (strncmp(p, "false", 5) == 0) {
      *out_start = p;
      *out_len = 5;
      return;
    }
  } else if (*p == 'n') { /* null */
    if (strncmp(p, "null", 4) == 0) {
      *out_start = p;
      *out_len = 4;
      return;
    }
  } else {
    /* number: parse until we hit a non-number character that can't be part of numeric literal */
    const char *q = p;
    /* optional sign */
    if (*q == '-' || *q == '+') q++;
    bool seen_digit = false;
    while (isdigit((unsigned char)*q)) { seen_digit = true; q++; }
    if (*q == '.') {
      q++;
      while (isdigit((unsigned char)*q)) { seen_digit = true; q++; }
    }
    if (*q == 'e' || *q == 'E') {
      q++;
      if (*q == '+' || *q == '-') q++;
      while (isdigit((unsigned char)*q)) q++;
    }
    if (seen_digit) {
      *out_start = start;
      *out_len = (size_t)(q - start);
      return;
    }
  }

  /* fallback: single token (until comma or brace) */
  const char *q = p;
  while (*q && *q != ',' && *q != '}' && !isspace((unsigned char)*q)) q++;
  *out_start = p;
  *out_len = (size_t)(q - p);
}

/* ---------- Public extraction functions ---------- */

/* Extract a string value for `key`. Caller must free returned pointer. Returns NULL if not found. */
char *json_get_string(const char *json, const char *key) {
  const char *vpos = find_key_value_pos(json, key);
  if (!vpos) return NULL;
  vpos = skip_ws(vpos);
  if (!vpos || *vpos != '\"') return NULL;
  return parse_json_string_value(vpos);
}

/* Extract an integer value for `key`. If out_found != NULL, set to 1 when found else 0. */
int json_get_int(const char *json, const char *key, int *out_found) {
  if (out_found) *out_found = 0;
  const char *vpos = find_key_value_pos(json, key);
  if (!vpos) return 0;
  const char *start;
  size_t len;
  parse_json_literal(vpos, &start, &len);
  if (!start || len == 0) return 0;
  /* copy into temporary buffer so strtol can be used */
  char *buf = (char *)malloc(len + 1);
  if (!buf) return 0;
  memcpy(buf, start, len);
  buf[len] = '\0';
  long val = strtol(buf, NULL, 10);
  free(buf);
  if (out_found) *out_found = 1;
  return (int)val;
}

/* Extract a double value for `key`. If out_found != NULL, set to 1 when found else 0. */
double json_get_double(const char *json, const char *key, int *out_found) {
  if (out_found) *out_found = 0;
  const char *vpos = find_key_value_pos(json, key);
  if (!vpos) return 0.0;
  const char *start;
  size_t len;
  parse_json_literal(vpos, &start, &len);
  if (!start || len == 0) return 0.0;
  char *buf = (char *)malloc(len + 1);
  if (!buf) return 0.0;
  memcpy(buf, start, len);
  buf[len] = '\0';
  double val = strtod(buf, NULL);
  free(buf);
  if (out_found) *out_found = 1;
  return val;
}

/* Extract a bool value for `key`. Returns 1 for true, 0 for false. If out_found != NULL set accordingly. */
int json_get_bool(const char *json, const char *key, int *out_found) {
  if (out_found) *out_found = 0;
  const char *vpos = find_key_value_pos(json, key);
  if (!vpos) return 0;
  const char *start;
  size_t len;
  parse_json_literal(vpos, &start, &len);
  if (!start || len == 0) return 0;
  if (len == 4 && strncmp(start, "true", 4) == 0) {
    if (out_found) *out_found = 1;
    return 1;
  }
  if (len == 5 && strncmp(start, "false", 5) == 0) {
    if (out_found) *out_found = 1;
    return 0;
  }
  return 0;
}

/* Extract an object value for `key`. Caller must free returned pointer. Returns NULL if not found. */
char *json_get_object(const char *json, const char *key) {
  const char *vpos = find_key_value_pos(json, key);
  if (!vpos) return NULL;
  return parse_json_object_value(vpos);
}

/* ---------- Sample struct parser ---------- */

typedef struct SampleStatus {
  int id;
  char *description; /* dynamically allocated; must be freed */
} SampleStatus;

/* Example struct that matches the compiler execution response JSON */
typedef struct SampleStruct {
  char *stdout_text;      /* JSON field: stdout */
  char *time;
  int memory;
  char *stderr_text;      /* JSON field: stderr */
  char *token;
  char *compile_output;
  char *message;
  SampleStatus status;
  int execution_time;     /* JSON field: executionTime */
  char *new_visibility;   /* JSON field: newVisibility */
} SampleStruct;

/* Parse a SampleStruct from JSON. Returns allocated SampleStruct* or NULL on allocation failure.
   The caller must free the struct and its nested allocations using `free_sample_struct`. */
SampleStruct *parse_sample_struct(const char *json) {
  if (!json) return NULL;
  SampleStruct *s = (SampleStruct *)malloc(sizeof(SampleStruct));
  if (!s) return NULL;

  int found = 0;
  s->stdout_text = json_get_string(json, "stdout");
  s->time = json_get_string(json, "time");
  s->memory = json_get_int(json, "memory", &found);
  s->stderr_text = json_get_string(json, "stderr");
  s->token = json_get_string(json, "token");
  s->compile_output = json_get_string(json, "compile_output");
  s->message = json_get_string(json, "message");
  s->execution_time = json_get_int(json, "executionTime", &found);
  s->new_visibility = json_get_string(json, "newVisibility");
  s->status.id = 0;
  s->status.description = NULL;

  char *status_json = json_get_object(json, "status");
  if (status_json) {
    s->status.id = json_get_int(status_json, "id", &found);
    s->status.description = json_get_string(status_json, "description");
    free(status_json);
  }

  return s;
}

/* Free a SampleStruct produced by parse_sample_struct */
void free_sample_struct(SampleStruct *s) {
  if (!s) return;
  if (s->stdout_text) free(s->stdout_text);
  if (s->time) free(s->time);
  if (s->stderr_text) free(s->stderr_text);
  if (s->token) free(s->token);
  if (s->compile_output) free(s->compile_output);
  if (s->message) free(s->message);
  if (s->status.description) free(s->status.description);
  if (s->new_visibility) free(s->new_visibility);
  free(s);
}

/* ---------- Example usage (commented) ----------

#include <stdio.h>

int main(void) {
  const char *json =
      "{\"stdout\":\"Hello, World from Rust!\\n\",\"time\":\"0.002\",\"memory\":3588,"
      "\"stderr\":null,\"token\":\"178996f0-7261-4e2e-a758-21710a7736a0\","
      "\"compile_output\":null,\"message\":null,\"status\":{\"id\":3,\"description\":\"Accepted\"},"
      "\"executionTime\":355,\"newVisibility\":null}";
  SampleStruct *s = parse_sample_struct(json);
  if (s) {
    printf("stdout=%s time=%s memory=%d token=%s status=%d:%s executionTime=%d\n",
           s->stdout_text ? s->stdout_text : "(null)",
           s->time ? s->time : "(null)",
           s->memory,
           s->token ? s->token : "(null)",
           s->status.id,
           s->status.description ? s->status.description : "(null)",
           s->execution_time);
    free_sample_struct(s);
  }
  return 0;
}

------------------------------------------- */
