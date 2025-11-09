#include "utils/ini_parser.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define INITIAL_SECTION_CAPACITY 8
#define INITIAL_ENTRY_CAPACITY 16
#define MAX_LINE_LENGTH 1024

// 内部函数声明
static ini_section_t* find_or_create_section(ini_file_t* ini, const char* section_name);
static ini_section_t* find_section(const ini_file_t* ini, const char* section_name);
static ini_entry_t* find_entry(const ini_section_t* section, const char* key);
static char* trim_string(char* str);
static bool parse_line(char* line, char** section, char** key, char** value);
static bool is_comment_line(const char* line);
static bool is_section_line(const char* line);
static char* extract_section_name(char* line);
static char* strdup_safe(const char* str);

ini_file_t* ini_create(void) {
    ini_file_t* ini = (ini_file_t*)malloc(sizeof(ini_file_t));
    if (!ini) return NULL;
    
    ini->sections = (ini_section_t*)malloc(sizeof(ini_section_t) * INITIAL_SECTION_CAPACITY);
    if (!ini->sections) {
        free(ini);
        return NULL;
    }
    
    ini->section_count = 0;
    ini->section_capacity = INITIAL_SECTION_CAPACITY;
    
    return ini;
}

ini_file_t* ini_load(const char* filename) {
    if (!filename) return NULL;
    
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;
    
    ini_file_t* ini = ini_create();
    if (!ini) {
        fclose(file);
        return NULL;
    }
    
    char line[MAX_LINE_LENGTH];
    char* current_section = NULL;
    
    while (fgets(line, sizeof(line), file)) {
        char* trimmed_line = trim_string(line);
        if (!trimmed_line[0]) continue; // 跳过空行
        
        if (is_comment_line(trimmed_line)) continue; // 跳过注释行
        
        if (is_section_line(trimmed_line)) {
            current_section = extract_section_name(trimmed_line);
            if (!current_section) continue;
            
            // 创建或找到对应的节
            ini_section_t* section = find_or_create_section(ini, current_section);
            if (!section) {
                free(current_section);
                fclose(file);
                ini_free(ini);
                return NULL;
            }
            continue;
        }
        
        // 解析键值对
        char* key = NULL;
        char* value = NULL;
        char* section_name = current_section ? current_section : "";
        
        if (parse_line(trimmed_line, &section_name, &key, &value)) {
            ini_section_t* section = find_or_create_section(ini, section_name);
            if (section && key && value) {
                ini_set_string(ini, section_name, key, value);
            }
            free(key);
            free(value);
        }
    }
    
    if (current_section) free(current_section);
    fclose(file);
    return ini;
}

ini_file_t* ini_load_from_string(const char* data) {
    if (!data) return NULL;
    
    ini_file_t* ini = ini_create();
    if (!ini) return NULL;
    
    char* copy = strdup_safe(data);
    if (!copy) {
        ini_free(ini);
        return NULL;
    }
    
    char* current_section = NULL;
    char* line = copy;
    char* next_line;
    
    while (line && *line) {
        // 找到下一行
        next_line = strchr(line, '\n');
        if (next_line) {
            *next_line = '\0';
            next_line++;
        }
        
        char* trimmed_line = trim_string(line);
        if (!trimmed_line[0]) {
            line = next_line;
            continue; // 跳过空行
        }
        
        if (is_comment_line(trimmed_line)) {
            line = next_line;
            continue; // 跳过注释行
        }
        
        if (is_section_line(trimmed_line)) {
            if (current_section) free(current_section);
            current_section = extract_section_name(trimmed_line);
            
            if (current_section) {
                ini_section_t* section = find_or_create_section(ini, current_section);
                if (!section) {
                    free(current_section);
                    free(copy);
                    ini_free(ini);
                    return NULL;
                }
            }
            line = next_line;
            continue;
        }
        
        // 解析键值对
        char* key = NULL;
        char* value = NULL;
        char* section_name = current_section ? current_section : "";
        
        if (parse_line(trimmed_line, &section_name, &key, &value)) {
            ini_section_t* section = find_or_create_section(ini, section_name);
            if (section && key && value) {
                ini_set_string(ini, section_name, key, value);
            }
            free(key);
            free(value);
        }
        
        line = next_line;
    }
    
    if (current_section) free(current_section);
    free(copy);
    return ini;
}

bool ini_save(const ini_file_t* ini, const char* filename) {
    if (!ini || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    // 写入默认节（无名节）的内容
    ini_section_t* default_section = find_section(ini, "");
    if (default_section && default_section->entry_count > 0) {
        for (size_t i = 0; i < default_section->entry_count; i++) {
            fprintf(file, "%s = %s\n", default_section->entries[i].key, default_section->entries[i].value);
        }
        fprintf(file, "\n");
    }
    
    // 写入其他节
    for (size_t i = 0; i < ini->section_count; i++) {
        ini_section_t* section = &ini->sections[i];
        if (!section->name || strcmp(section->name, "") == 0) continue;
        
        fprintf(file, "[%s]\n", section->name);
        for (size_t j = 0; j < section->entry_count; j++) {
            fprintf(file, "%s = %s\n", section->entries[j].key, section->entries[j].value);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    return true;
}

void ini_free(ini_file_t* ini) {
    if (!ini) return;
    
    for (size_t i = 0; i < ini->section_count; i++) {
        ini_section_t* section = &ini->sections[i];
        if (section->name) free(section->name);
        
        for (size_t j = 0; j < section->entry_count; j++) {
            if (section->entries[j].key) free(section->entries[j].key);
            if (section->entries[j].value) free(section->entries[j].value);
        }
        
        if (section->entries) free(section->entries);
    }
    
    if (ini->sections) free(ini->sections);
    free(ini);
}

const char* ini_get_string(const ini_file_t* ini, const char* section, const char* key, const char* default_value) {
    if (!ini || !key) return default_value;
    
    const char* section_name = section ? section : "";
    ini_section_t* sec = find_section(ini, section_name);
    if (!sec) return default_value;
    
    ini_entry_t* entry = find_entry(sec, key);
    if (!entry) return default_value;
    
    return entry->value;
}

int ini_get_int(const ini_file_t* ini, const char* section, const char* key, int default_value) {
    const char* str_value = ini_get_string(ini, section, key, NULL);
    if (!str_value) return default_value;
    
    char* endptr;
    long value = strtol(str_value, &endptr, 10);
    if (endptr == str_value || *endptr != '\0') return default_value;
    
    return (int)value;
}

double ini_get_double(const ini_file_t* ini, const char* section, const char* key, double default_value) {
    const char* str_value = ini_get_string(ini, section, key, NULL);
    if (!str_value) return default_value;
    
    char* endptr;
    double value = strtod(str_value, &endptr);
    if (endptr == str_value || *endptr != '\0') return default_value;
    
    return value;
}

bool ini_get_bool(const ini_file_t* ini, const char* section, const char* key, bool default_value) {
    const char* str_value = ini_get_string(ini, section, key, NULL);
    if (!str_value) return default_value;
    
    // 支持多种布尔值表示
    if (strcmp(str_value, "true") == 0 || strcmp(str_value, "yes") == 0 || 
        strcmp(str_value, "on") == 0 || strcmp(str_value, "1") == 0) {
        return true;
    }
    
    if (strcmp(str_value, "false") == 0 || strcmp(str_value, "no") == 0 || 
        strcmp(str_value, "off") == 0 || strcmp(str_value, "0") == 0) {
        return false;
    }
    
    return default_value;
}

bool ini_set_string(ini_file_t* ini, const char* section, const char* key, const char* value) {
    if (!ini || !key || !value) return false;
    
    const char* section_name = section ? section : "";
    ini_section_t* sec = find_or_create_section(ini, section_name);
    if (!sec) return false;
    
    ini_entry_t* entry = find_entry(sec, key);
    if (entry) {
        // 更新现有值
        free(entry->value);
        entry->value = strdup_safe(value);
        return entry->value != NULL;
    }
    
    // 添加新条目
    if (sec->entry_count >= sec->entry_capacity) {
        size_t new_capacity = sec->entry_capacity * 2;
        ini_entry_t* new_entries = (ini_entry_t*)realloc(sec->entries, sizeof(ini_entry_t) * new_capacity);
        if (!new_entries) return false;
        
        sec->entries = new_entries;
        sec->entry_capacity = new_capacity;
    }
    
    ini_entry_t* new_entry = &sec->entries[sec->entry_count++];
    new_entry->key = strdup_safe(key);
    new_entry->value = strdup_safe(value);
    
    return new_entry->key != NULL && new_entry->value != NULL;
}

bool ini_set_int(ini_file_t* ini, const char* section, const char* key, int value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return ini_set_string(ini, section, key, buffer);
}

bool ini_set_double(ini_file_t* ini, const char* section, const char* key, double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%f", value);
    return ini_set_string(ini, section, key, buffer);
}

bool ini_set_bool(ini_file_t* ini, const char* section, const char* key, bool value) {
    return ini_set_string(ini, section, key, value ? "true" : "false");
}

bool ini_delete_key(ini_file_t* ini, const char* section, const char* key) {
    if (!ini || !key) return false;
    
    const char* section_name = section ? section : "";
    ini_section_t* sec = find_section(ini, section_name);
    if (!sec) return false;
    
    for (size_t i = 0; i < sec->entry_count; i++) {
        if (strcmp(sec->entries[i].key, key) == 0) {
            free(sec->entries[i].key);
            free(sec->entries[i].value);
            
            // 移动后续条目
            for (size_t j = i; j < sec->entry_count - 1; j++) {
                sec->entries[j] = sec->entries[j + 1];
            }
            
            sec->entry_count--;
            return true;
        }
    }
    
    return false;
}

bool ini_delete_section(ini_file_t* ini, const char* section) {
    if (!ini || !section) return false;
    
    for (size_t i = 0; i < ini->section_count; i++) {
        if (strcmp(ini->sections[i].name, section) == 0) {
            // 释放节内存
            free(ini->sections[i].name);
            for (size_t j = 0; j < ini->sections[i].entry_count; j++) {
                free(ini->sections[i].entries[j].key);
                free(ini->sections[i].entries[j].value);
            }
            free(ini->sections[i].entries);
            
            // 移动后续节
            for (size_t j = i; j < ini->section_count - 1; j++) {
                ini->sections[j] = ini->sections[j + 1];
            }
            
            ini->section_count--;
            return true;
        }
    }
    
    return false;
}

bool ini_has_section(const ini_file_t* ini, const char* section) {
    return find_section(ini, section ? section : "") != NULL;
}

bool ini_has_key(const ini_file_t* ini, const char* section, const char* key) {
    if (!ini || !key) return false;
    
    const char* section_name = section ? section : "";
    ini_section_t* sec = find_section(ini, section_name);
    if (!sec) return false;
    
    return find_entry(sec, key) != NULL;
}

size_t ini_get_section_count(const ini_file_t* ini) {
    return ini ? ini->section_count : 0;
}

size_t ini_get_key_count(const ini_file_t* ini, const char* section) {
    if (!ini) return 0;
    
    const char* section_name = section ? section : "";
    ini_section_t* sec = find_section(ini, section_name);
    return sec ? sec->entry_count : 0;
}

size_t ini_get_sections(const ini_file_t* ini, char** sections, size_t max_count) {
    if (!ini || !sections || max_count == 0) return 0;
    
    size_t count = 0;
    for (size_t i = 0; i < ini->section_count && count < max_count; i++) {
        sections[count++] = ini->sections[i].name;
    }
    
    return count;
}

size_t ini_get_keys(const ini_file_t* ini, const char* section, char** keys, size_t max_count) {
    if (!ini || !keys || max_count == 0) return 0;
    
    const char* section_name = section ? section : "";
    ini_section_t* sec = find_section(ini, section_name);
    if (!sec) return 0;
    
    size_t count = 0;
    for (size_t i = 0; i < sec->entry_count && count < max_count; i++) {
        keys[count++] = sec->entries[i].key;
    }
    
    return count;
}

// 内部函数实现

static ini_section_t* find_or_create_section(ini_file_t* ini, const char* section_name) {
    ini_section_t* section = find_section(ini, section_name);
    if (section) return section;
    
    // 创建新节
    if (ini->section_count >= ini->section_capacity) {
        size_t new_capacity = ini->section_capacity * 2;
        ini_section_t* new_sections = (ini_section_t*)realloc(ini->sections, sizeof(ini_section_t) * new_capacity);
        if (!new_sections) return NULL;
        
        ini->sections = new_sections;
        ini->section_capacity = new_capacity;
    }
    
    ini_section_t* new_section = &ini->sections[ini->section_count++];
    new_section->name = strdup_safe(section_name);
    new_section->entries = (ini_entry_t*)malloc(sizeof(ini_entry_t) * INITIAL_ENTRY_CAPACITY);
    new_section->entry_count = 0;
    new_section->entry_capacity = INITIAL_ENTRY_CAPACITY;
    
    if (!new_section->name || !new_section->entries) {
        if (new_section->name) free(new_section->name);
        if (new_section->entries) free(new_section->entries);
        ini->section_count--;
        return NULL;
    }
    
    return new_section;
}

static ini_section_t* find_section(const ini_file_t* ini, const char* section_name) {
    if (!ini || !section_name) return NULL;
    
    for (size_t i = 0; i < ini->section_count; i++) {
        if (strcmp(ini->sections[i].name, section_name) == 0) {
            return (ini_section_t*)&ini->sections[i];
        }
    }
    
    return NULL;
}

static ini_entry_t* find_entry(const ini_section_t* section, const char* key) {
    if (!section || !key) return NULL;
    
    for (size_t i = 0; i < section->entry_count; i++) {
        if (strcmp(section->entries[i].key, key) == 0) {
            return (ini_entry_t*)&section->entries[i];
        }
    }
    
    return NULL;
}

static char* trim_string(char* str) {
    if (!str) return NULL;
    
    // 去除前导空白
    char* start = str;
    while (isspace((unsigned char)*start)) start++;
    
    // 去除尾部空白
    char* end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    *(end + 1) = '\0';
    return start;
}

static bool parse_line(char* line, char** section, char** key, char** value) {
    if (!line || !key || !value) return false;
    
    char* equal_sign = strchr(line, '=');
    if (!equal_sign) return false;
    
    *equal_sign = '\0';
    
    *key = trim_string(line);
    *value = trim_string(equal_sign + 1);
    
    if (!*key || !**key) return false;
    
    *key = strdup_safe(*key);
    *value = strdup_safe(*value);
    
    return *key != NULL && *value != NULL;
}

static bool is_comment_line(const char* line) {
    if (!line) return false;
    return line[0] == ';' || line[0] == '#';
}

static bool is_section_line(const char* line) {
    if (!line) return false;
    return line[0] == '[' && line[strlen(line) - 1] == ']';
}

static char* extract_section_name(char* line) {
    if (!line || strlen(line) < 3) return NULL; // 至少需要 [x]
    
    line[strlen(line) - 1] = '\0'; // 去除尾部']'
    char* name = trim_string(line + 1); // 跳过头部'['
    return strdup_safe(name);
}

static char* strdup_safe(const char* str) {
    if (!str) return NULL;
    char* copy = (char*)malloc(strlen(str) + 1);
    if (copy) strcpy(copy, str);
    return copy;
}