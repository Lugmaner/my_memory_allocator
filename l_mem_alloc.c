#include "l_mem_alloc.h"

static char *HEAP_START = NULL;
static size_t ALIGNED_HEADER_SIZE = 0;

static int align_up(size_t x, size_t *out){
    size_t align = alignof(max_align_t);
    if(x > SIZE_MAX - (align - 1)) return -1;
    *out = (x + align - 1) & ~(align - 1);
    return 0;
}

static void *user_ptr_from_header(void *header){
    return (char *)header + ALIGNED_HEADER_SIZE;
}

static int heap_init(void){
    char* cur_heap_start = (char*)sbrk(0);
    size_t aligned;
    if(align_up((uintptr_t)cur_heap_start,&aligned) == -1){
        errno = ENOMEM;
        return -1;
    }
    size_t pad = aligned - (size_t)cur_heap_start;
    if(pad != 0 && sbrk(pad) == (void*)-1) {
        errno = ENOMEM;
        return -1;
    }

    //init ALIGNED_HEADER_SIZE
    if(align_up(sizeof(alloc_data_t),&ALIGNED_HEADER_SIZE) == -1){
        errno = EOVERFLOW;
        return -1;
    }

    HEAP_START = (char*)sbrk(0);

    return 0;
}

static alloc_data_t *heap_walk(size_t bytes_requested){
    char *mem_iterator = HEAP_START;
    char *brk = (char *)sbrk(0);
    alloc_data_t *casted_iterator = (alloc_data_t*)mem_iterator;

    while (mem_iterator < brk && (casted_iterator->is_free != true || casted_iterator->capacity < bytes_requested))
    {
        mem_iterator += casted_iterator->capacity + ALIGNED_HEADER_SIZE;
        casted_iterator = (alloc_data_t*)mem_iterator;
    }
    
    if(mem_iterator < brk){
        return casted_iterator;
    }
    return NULL;
}

void *l_malloc(size_t n, size_t size){

    if(!HEAP_START && (heap_init() == -1)){
        return NULL;
    }

    if(n != 0 && size > SIZE_MAX / n){
        errno = EOVERFLOW;
        return NULL;
    }

    size_t bytes_requested;
    if(align_up(n * size,&bytes_requested) == -1){
        errno = EOVERFLOW;
        return NULL;
    }

    //search for free allocated mem
    alloc_data_t *found_mem = NULL;
    if((found_mem = heap_walk(bytes_requested)) != NULL){

        found_mem->is_free = false;
        found_mem->num_of_elements = n;
        found_mem->size_of_type = size;

        return user_ptr_from_header(found_mem);
    }

    //no mem found ==> new allocation

    if(bytes_requested > SIZE_MAX - ALIGNED_HEADER_SIZE){
        errno = EOVERFLOW;
        return NULL;
    }

    size_t total_size = bytes_requested + ALIGNED_HEADER_SIZE;

    alloc_data_t *allocated = sbrk(total_size);
    if(allocated == (void *)-1){
        errno = ENOMEM;
        return NULL;
    }

    allocated->capacity = bytes_requested;
    allocated->num_of_elements = n;
    allocated->size_of_type = size;
    allocated->is_free = false;

    return user_ptr_from_header(allocated);
}
