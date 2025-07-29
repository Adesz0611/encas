#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;
#define ENCAS_API

#define ENCAS_MALLOC(size) malloc(size)
#define ENCAS_REALLOC(ptr, size) realloc(ptr, size)
#define ENCAS_FREE(ptr) free(ptr)

#define LOAD_FACTOR 0.75f


typedef struct Encas_FaceKey {
    u32 v[3];
} Encas_FaceKey;

typedef struct Encas_FaceKeyMap {
    Encas_FaceKey *keys;
    u8 *values;
    u32 cap;
    u32 len;
} Encas_FaceKeyMap;

ENCAS_API bool Encas_EqualFaceKey(const Encas_FaceKey *a, const Encas_FaceKey *b);
ENCAS_API void Encas_CreateFaceKeyMap(Encas_FaceKeyMap *map, u32 cap);
ENCAS_API void Encas_DeleteFaceKeyMap(Encas_FaceKeyMap *map);
ENCAS_API void Encas_RehashFaceKeyMap(Encas_FaceKeyMap *map, u32 new_cap);
ENCAS_API void Encas_SetFaceKeyMap(Encas_FaceKeyMap *map, Encas_FaceKey key, u8 value);
ENCAS_API bool Encas_GetFaceKeyMap(Encas_FaceKeyMap *map, Encas_FaceKey key, u8 *out_value);
ENCAS_API void Encas_RehashFaceKeyMap(Encas_FaceKeyMap *map, u32 new_cap);

// FNV-1a 64-bit hash
static inline u64 hash_facekey(const Encas_FaceKey *key) {
    const u8 *data = (const u8 *)key;
    u64 hash = 14695981039346656037ULL;
    for (u32 i = 0; i < sizeof(Encas_FaceKey); ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

ENCAS_API bool Encas_EqualFaceKey(const Encas_FaceKey *a, const Encas_FaceKey *b) {
    return memcmp(a, b, sizeof(Encas_FaceKey)) == 0;
}

ENCAS_API void Encas_CreateFaceKeyMap(Encas_FaceKeyMap *map, u32 cap) {
    map->cap = cap;
    map->len = 0;
    map->keys = ENCAS_MALLOC(map->cap * sizeof(Encas_FaceKey));
    memset(map->keys, 0, map->cap * sizeof(Encas_FaceKey));
    map->values = ENCAS_MALLOC(map->cap * sizeof(u8));
    memset(map->values, 0, map->cap * sizeof(u8));
}

ENCAS_API void Encas_DeleteFaceKeyMap(Encas_FaceKeyMap *map) {
    ENCAS_FREE(map->keys);
    ENCAS_FREE(map->values);
    map->keys = NULL;
    map->values = NULL;
    map->cap = 0;
    map->len = 0;
}

ENCAS_API void Encas_SetFaceKeyMap(Encas_FaceKeyMap *map, Encas_FaceKey key, u8 value) {
    if ((float)map->len / map->cap >= LOAD_FACTOR) {
        Encas_RehashFaceKeyMap(map, map->cap * 2);
    }

    u64 hash = hash_facekey(&key);
    u32 index = hash % map->cap;

    while (map->values[index] != 0) {
        if (Encas_EqualFaceKey(&map->keys[index], &key)) {
            map->values[index] = value;
            return;
        }
        index = (index + 1) % map->cap;
    }

    map->keys[index] = key;
    map->values[index] = value;
    map->len++;
}

ENCAS_API bool Encas_GetFaceKeyMap(Encas_FaceKeyMap *map, Encas_FaceKey key, u8 *out_value) {
    uint64_t hash = hash_facekey(&key);
    uint32_t index = hash % map->cap;

    while (map->values[index] != 0) {
        if (Encas_EqualFaceKey(&map->keys[index], &key)) {
            *out_value = map->values[index];
            return true;
        }
        index = (index + 1) % map->cap;
    }
    return false;
}

ENCAS_API void Encas_RehashFaceKeyMap(Encas_FaceKeyMap *map, u32 new_cap) {
    Encas_FaceKey *old_keys = map->keys;
    u8 *old_values = map->values;
    u32 old_cap = map->cap;

    map->keys = ENCAS_MALLOC(new_cap * sizeof(Encas_FaceKey));
    memset(map->keys, 0, new_cap * sizeof(Encas_FaceKey));
    map->values = ENCAS_MALLOC(new_cap * sizeof(u8));
    memset(map->values, 0, new_cap * sizeof(u8));
    map->cap = new_cap;
    map->len = 0;

    for (u32 i = 0; i < old_cap; ++i) {
        if (old_values[i] != 0) {
            Encas_SetFaceKeyMap(map, old_keys[i], old_values[i]);
        }
    }

    ENCAS_FREE(old_keys);
    ENCAS_FREE(old_values);
}
