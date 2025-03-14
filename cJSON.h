#ifndef CJSON_H
#define CJSON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Define minimal types for cJSON */
typedef struct cJSON {
    char *string;             /* Key string for objects, NULL otherwise */
    char *valuestring;        /* For string items */
    int type;                 /* Type of item: see below */
    struct cJSON *child;      /* For objects/arrays: points to first child */
    struct cJSON *next;       /* Next sibling */
} cJSON;

/* Type definitions (only those needed for this project) */
#define cJSON_NULL   0
#define cJSON_String 1
#define cJSON_Array  2
#define cJSON_Object 3

/* Function prototypes */
cJSON *cJSON_Parse(const char *value);
cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string);
int cJSON_IsArray(const cJSON * const item);
int cJSON_GetArraySize(const cJSON *array);
cJSON *cJSON_GetArrayItem(const cJSON *array, int index);
int cJSON_IsString(const cJSON * const item);
void cJSON_Delete(cJSON *item);

#ifdef __cplusplus
}
#endif

#endif

