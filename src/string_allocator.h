// api
#ifndef SKS_STRING_ALLOCATOR_H
#define SKS_STRING_ALLOCATOR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./messages.h"
#include "../external/mystrings.h"

#define SKS_STRING_NAME_LEN 128 

typedef struct {
  size_t id;
  size_t len;
} sks_string;

typedef struct {
  size_t bsp;                   // bytes sp
  size_t bytes_capacity;

  ascii* bytes;                 // for the bytes of the actual strings 
} sks_string_allocator;

int sks_s_allocator_init(int init_cap, sks_string_allocator* out); 

int sks_s_allocator_add(sks_string_allocator* allocator, 
                        ascii* name, 
                        sks_string *string_out);

void sks_s_allocator_destroy(sks_string_allocator allocator);

#define SKS_S_ALLOCATOR_GET(allocator, string_id, out)                         \
do {                                                                           \
  if (string_id >= (allocator).bsp) {                                          \
    SKS_MSG_ERR("[ERROR] string ID out of bounds for string allocator.\n");    \
    out = NULL;                                                                \
  } else {                                                                     \
    out = &(allocator).bytes[string_id];                                       \
  }                                                                            \
} while (0)                                                                    \


#endif

// implementation

// #define SKS_STRING_ALLOCATOR_IMPL
#if (defined(SKS_STRING_ALLOCATOR_IMPL))

int sks_s_allocator_init(int init_cap, sks_string_allocator* out) {
  sks_string_allocator s_alloc;

  s_alloc.bsp = 0;
  s_alloc.bytes_capacity = (init_cap ? init_cap : 32) * SKS_STRING_NAME_LEN;
  s_alloc.bytes = (ascii *)malloc(sizeof(ascii) * s_alloc.bytes_capacity);

  if (!s_alloc.bytes) {
    SKS_MSG_ERR("[ERROR] Could not allocate for string_allocator bytes.\n");
    return -1;
  }

  *out = s_alloc;
  return 0;
}

int sks_s_allocator_add(sks_string_allocator* allocator, ascii* name, sks_string *string_out) {
  if (!allocator) {
    SKS_MSG_ERR("[ERROR] Invalid allocator when trying to add a string to "
                    "string allocator.\n");
    return -1;
  }
  if (!name) {
    SKS_MSG_ERR("[ERROR] Invalid name given when trying to add string to "
                    "string allocator.\n");
    return -1;
  }

  size_t i = strlen((char *)name);
  i++;      // null terminator

  if (i + allocator->bsp >= allocator->bytes_capacity) {
      while (i + allocator->bsp >= allocator->bytes_capacity)
        allocator->bytes_capacity *= 2;

      void *ptr = realloc(allocator->bytes, sizeof(ascii) * allocator->bytes_capacity);
      if (!ptr) {
        SKS_MSG_ERR("[ERROR] Failed to realloc memory to string allocator bytes.\n");
        return -1;
      }
      allocator->bytes = (ascii *)ptr;
  }

  sks_string string;

  string.len = i;
  strncpy((char *)(allocator->bytes + allocator->bsp), (char *)name, string.len);
  string.id = allocator->bsp;
  allocator->bsp += string.len;

  *string_out = string;

  return 0;
}

void sks_s_allocator_destroy(sks_string_allocator allocator) {
    allocator.bsp = 0;
    allocator.bytes_capacity = 32;
    free(allocator.bytes);
}

#endif
