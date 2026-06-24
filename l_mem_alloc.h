#ifndef L_MEM_ALLOC
#define L_MEM_ALLOC

#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdalign.h>
#include <stddef.h>

typedef struct{
    size_t size_of_type;
    size_t num_of_elements;
} alloc_data_t;


void *l_malloc(size_t n, size_t size);
void *l_calloc(size_t n, size_t size);
void l_free(void *to_free);
void *l_realloc(void *p, size_t n, size_t size);

#endif