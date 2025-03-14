#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Helper function to skip whitespace */
static void skip_whitespace(const char **p) {
    while (**p && isspace(**p)) {
        (*p)++;
    }
}

/* Parse a JSON string (only supports simple strings without escapes) */
static char *parse_string(const char **p) {
    skip_whitespace(p);
    if (**p != '\"') return NULL;
    (*p)++;  /* Skip the opening quote */
    const char *start = *p;
    while (**p && **p != '\"') {
        (*p)++;
    }
    if (**p != '\"') return NULL;
    size_t len = *p - start;
    char *str = (char *)malloc(len + 1);
    if (!str) return NULL;
    strncpy(str, start, len);
    str[len] = '\0';
    (*p)++;  /* Skip the closing quote */
    return str;
}

/* Forward declaration for recursive parsing */
static cJSON *parse_value(const char **p);

/* Parse a JSON array of strings */
static cJSON *parse_array(const char **p) {
    skip_whitespace(p);
    if (**p != '[') return NULL;
    (*p)++;  /* Skip '[' */
    cJSON *array = (cJSON *)malloc(sizeof(cJSON));
    if (!array) return NULL;
    memset(array, 0, sizeof(cJSON));
    array->type = cJSON_Array;
    cJSON *prev = NULL;

    skip_whitespace(p);
    while (**p && **p != ']') {
        cJSON *item = (cJSON *)malloc(sizeof(cJSON));
        if (!item) break;
        memset(item, 0, sizeof(cJSON));
        item->type = cJSON_String;
        item->valuestring = parse_string(p);
        if (!item->valuestring) {
            free(item);
            break;
        }
        if (prev == NULL) {
            array->child = item;
        } else {
            prev->next = item;
        }
        prev = item;
        skip_whitespace(p);
        if (**p == ',') {
            (*p)++; /* Skip comma */
        }
        skip_whitespace(p);
    }
    if (**p == ']') {
        (*p)++;  /* Skip ']' */
    }
    return array;
}

/* Parse a JSON object with only one key-value pair (for our dummy case) */
static cJSON *parse_object(const char **p) {
    skip_whitespace(p);
    if (**p != '{') return NULL;
    (*p)++; /* Skip '{' */
    cJSON *object = (cJSON *)malloc(sizeof(cJSON));
    if (!object) return NULL;
    memset(object, 0, sizeof(cJSON));
    object->type = cJSON_Object;
    cJSON *prev = NULL;

    skip_whitespace(p);
    while (**p && **p != '}') {
        char *key = parse_string(p);
        if (!key) {
            cJSON_Delete(object);
            return NULL;
        }
        skip_whitespace(p);
        if (**p != ':') {
            free(key);
            cJSON_Delete(object);
            return NULL;
        }
        (*p)++; /* Skip ':' */
        skip_whitespace(p);
        cJSON *item = parse_value(p);
        if (!item) {
            free(key);
            cJSON_Delete(object);
            return NULL;
        }
        item->string = key; /* Set key for this item */
        if (prev == NULL) {
            object->child = item;
        } else {
            prev->next = item;
        }
        prev = item;
        skip_whitespace(p);
        if (**p == ',') {
            (*p)++; /* Skip comma */
        }
        skip_whitespace(p);
    }
    if (**p == '}') {
        (*p)++; /* Skip '}' */
    }
    return object;
}

/* Parse a JSON value: either an object, array, or string for our purposes */
static cJSON *parse_value(const char **p) {
    skip_whitespace(p);
    if (**p == '{') {
        return parse_object(p);
    } else if (**p == '[') {
        return parse_array(p);
    } else if (**p == '\"') {
        cJSON *item = (cJSON *)malloc(sizeof(cJSON));
        if (!item) return NULL;
        memset(item, 0, sizeof(cJSON));
        item->type = cJSON_String;
        item->valuestring = parse_string(p);
        return item;
    }
    return NULL;
}

/* Parse the JSON text into a cJSON structure */
cJSON *cJSON_Parse(const char *value) {
    if (!value) return NULL;
    const char *p = value;
    skip_whitespace(&p);
    cJSON *root = parse_value(&p);
    return root;
}

/* Retrieve an object item by key (case-sensitive) */
cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string) {
    if (!object || object->type != cJSON_Object) return NULL;
    cJSON *child = object->child;
    while (child) {
        if (child->string && strcmp(child->string, string) == 0) {
            return child;
        }
        child = child->next;
    }
    return NULL;
}

/* Check if an item is an array */
int cJSON_IsArray(const cJSON * const item) {
    return (item && item->type == cJSON_Array);
}

/* Get the size (number of elements) in an array */
int cJSON_GetArraySize(const cJSON *array) {
    if (!array || array->type != cJSON_Array) return 0;
    int count = 0;
    cJSON *child = array->child;
    while (child) {
        count++;
        child = child->next;
    }
    return count;
}

/* Get an array item by index */
cJSON *cJSON_GetArrayItem(const cJSON *array, int index) {
    if (!array || array->type != cJSON_Array) return NULL;
    cJSON *child = array->child;
    while (child && index > 0) {
        child = child->next;
        index--;
    }
    return child;
}

/* Check if an item is a string */
int cJSON_IsString(const cJSON * const item) {
    return (item && item->type == cJSON_String && item->valuestring != NULL);
}

/* Recursively free the cJSON structure */
void cJSON_Delete(cJSON *item) {
    if (!item) return;
    if (item->type == cJSON_Object || item->type == cJSON_Array) {
        cJSON *child = item->child;
        while (child) {
            cJSON *next = child->next;
            cJSON_Delete(child);
            child = next;
        }
    }
    if (item->string) free(item->string);
    if (item->valuestring) free(item->valuestring);
    free(item);
}

