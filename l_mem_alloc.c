#include "l_mem_alloc.h"

static int align_up(size_t x, size_t *out){
    size_t align = alignof(max_align_t);
    if(x > SIZE_MAX - (align - 1)) return -1;
    *out = (x + align - 1) & ~(align - 1);
    return 0;
}

static void *user_ptr_from_header(void *header, size_t header_size){
    return (char *)header + header_size;
}

void *l_malloc(size_t n, size_t size){

    if(n != 0 && size > SIZE_MAX / n){
        errno = EOVERFLOW;
        return NULL;
    }

    size_t bytes_requested;
    if(align_up(n * size,&bytes_requested) == -1){
        errno = EOVERFLOW;
        return NULL;
    }

    size_t aligned_struct_size;
    if(align_up(sizeof(alloc_data_t),&aligned_struct_size) == -1){
        errno = EOVERFLOW;
        return NULL;
    }

    if(bytes_requested > SIZE_MAX - aligned_struct_size){
        errno = EOVERFLOW;
        return NULL;
    }

    size_t total_size = bytes_requested + aligned_struct_size;

    uintptr_t cur = (uintptr_t)sbrk(0);
    size_t pad;
    if(align_up(cur,&pad) == -1){
        errno = EOVERFLOW;
        return NULL;
    }
    pad -= cur;
    if(pad != 0 && sbrk(pad) == (void*)-1){
        return NULL;
    }

    alloc_data_t *allocated = sbrk(total_size);
    if(allocated == (void *)-1){
        return NULL;
    }

    allocated->num_of_elements = n;
    allocated->size_of_type = size;

    return user_ptr_from_header(allocated, aligned_struct_size);
}
