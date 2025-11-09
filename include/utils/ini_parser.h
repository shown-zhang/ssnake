#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <stdbool.h>
#include <stddef.h>

// INI配置项结构
typedef struct {
    char* key;
    char* value;
} ini_entry_t;

// INI节结构
typedef struct {
    char* name;
    ini_entry_t* entries;
    size_t entry_count;
    size_t entry_capacity;
} ini_section_t;

// INI文件结构
typedef struct {
    ini_section_t* sections;
    size_t section_count;
    size_t section_capacity;
} ini_file_t;

// 函数声明

/**
 * @brief 创建空的INI文件结构
 * @return 新创建的INI文件结构指针，失败返回NULL
 */
ini_file_t* ini_create(void);

/**
 * @brief 从文件加载INI配置
 * @param filename 文件名
 * @return 加载的INI文件结构指针，失败返回NULL
 */
ini_file_t* ini_load(const char* filename);

/**
 * @brief 从字符串加载INI配置
 * @param data INI格式的字符串
 * @return 加载的INI文件结构指针，失败返回NULL
 */
ini_file_t* ini_load_from_string(const char* data);

/**
 * @brief 保存INI配置到文件
 * @param ini INI文件结构
 * @param filename 文件名
 * @return 成功返回true，失败返回false
 */
bool ini_save(const ini_file_t* ini, const char* filename);

/**
 * @brief 释放INI文件结构
 * @param ini INI文件结构指针
 */
void ini_free(ini_file_t* ini);

/**
 * @brief 获取配置值
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @param default_value 默认值（当找不到时返回）
 * @return 配置值字符串，找不到返回默认值
 */
const char* ini_get_string(const ini_file_t* ini, const char* section, const char* key, const char* default_value);

/**
 * @brief 获取整型配置值
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @param default_value 默认值
 * @return 整型配置值
 */
int ini_get_int(const ini_file_t* ini, const char* section, const char* key, int default_value);

/**
 * @brief 获取浮点型配置值
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @param default_value 默认值
 * @return 浮点型配置值
 */
double ini_get_double(const ini_file_t* ini, const char* section, const char* key, double default_value);

/**
 * @brief 获取布尔型配置值
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @param default_value 默认值
 * @return 布尔型配置值
 */
bool ini_get_bool(const ini_file_t* ini, const char* section, const char* key, bool default_value);

/**
 * @brief 设置配置值
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @param value 值
 * @return 成功返回true，失败返回false
 */
bool ini_set_string(ini_file_t* ini, const char* section, const char* key, const char* value);

/**
 * @brief 设置整型配置值
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @param value 值
 * @return 成功返回true，失败返回false
 */
bool ini_set_int(ini_file_t* ini, const char* section, const char* key, int value);

/**
 * @brief 设置浮点型配置值
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @param value 值
 * @return 成功返回true，失败返回false
 */
bool ini_set_double(ini_file_t* ini, const char* section, const char* key, double value);

/**
 * @brief 设置布尔型配置值
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @param value 值
 * @return 成功返回true，失败返回false
 */
bool ini_set_bool(ini_file_t* ini, const char* section, const char* key, bool value);

/**
 * @brief 删除配置项
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @return 成功返回true，失败返回false
 */
bool ini_delete_key(ini_file_t* ini, const char* section, const char* key);

/**
 * @brief 删除整个节
 * @param ini INI文件结构
 * @param section 节名
 * @return 成功返回true，失败返回false
 */
bool ini_delete_section(ini_file_t* ini, const char* section);

/**
 * @brief 检查节是否存在
 * @param ini INI文件结构
 * @param section 节名
 * @return 存在返回true，不存在返回false
 */
bool ini_has_section(const ini_file_t* ini, const char* section);

/**
 * @brief 检查配置项是否存在
 * @param ini INI文件结构
 * @param section 节名
 * @param key 键名
 * @return 存在返回true，不存在返回false
 */
bool ini_has_key(const ini_file_t* ini, const char* section, const char* key);

/**
 * @brief 获取节数量
 * @param ini INI文件结构
 * @return 节数量
 */
size_t ini_get_section_count(const ini_file_t* ini);

/**
 * @brief 获取指定节的键数量
 * @param ini INI文件结构
 * @param section 节名
 * @return 键数量
 */
size_t ini_get_key_count(const ini_file_t* ini, const char* section);

/**
 * @brief 获取节名列表
 * @param ini INI文件结构
 * @param sections 存储节名的数组（需要预先分配足够空间）
 * @param max_count 最大数量
 * @return 实际获取的节数量
 */
size_t ini_get_sections(const ini_file_t* ini, char** sections, size_t max_count);

/**
 * @brief 获取指定节的键名列表
 * @param ini INI文件结构
 * @param section 节名
 * @param keys 存储键名的数组（需要预先分配足够空间）
 * @param max_count 最大数量
 * @return 实际获取的键数量
 */
size_t ini_get_keys(const ini_file_t* ini, const char* section, char** keys, size_t max_count);

#endif // INI_PARSER_H