#include <string.h>

// const char *LANGUAGE_KEYS[11] = {"",           "c",    "cpp",  "python",
//                                  "javascript", "java", "ruby", "go",
//                                  "rust",       "php",  "c#"};
// const char *LANGUAGE_VALUES[11] = {"",   "c",  "cpp", "py",  "js", "java",
//                                    "rb", "go", "rs",  "php", "cs"};

struct Language {
  int id;
  const char *name;
  const char *extension;
  Language(int id, const char *name, const char *extension)
      : id(id), name(name), extension(extension) {}
};

Language language_list[11] = {
    {0, "", ""},         {1, "c", "c"},           {2, "cpp", "cpp"},
    {3, "python", "py"}, {4, "javascript", "js"}, {5, "java", "java"},
    {6, "ruby", "rb"},   {7, "go", "go"},         {8, "rust", "rs"},
    {9, "php", "php"},   {10, "c#", "cs"},
};

const char *get_extension_from_language_name(const char *language_name) {
  for (int i = 0; i < 11; i++) {
    if (strcmp(language_list[i].name, language_name) == 0) {
      return language_list[i].extension;
    }
  }
  return "";
}

const char *get_language_name_from_extension(const char *extension) {
  for (int i = 0; i < 11; i++) {
    if (strcmp(language_list[i].extension, extension) == 0) {
      return language_list[i].name;
    }
  }
  return "";
}