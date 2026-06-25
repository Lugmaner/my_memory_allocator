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

static void split_block(void *found_mem, size_t aligned_expected_size){
    alloc_data_t *old_header = (alloc_data_t *)found_mem;
    size_t old_cap = old_header->capacity;

    const size_t MIN_PAYLOAD = alignof(max_align_t);

    if(aligned_expected_size > SIZE_MAX - ALIGNED_HEADER_SIZE){return;}
    size_t need = aligned_expected_size + ALIGNED_HEADER_SIZE;
    if(need > SIZE_MAX - MIN_PAYLOAD){return;}
    need += MIN_PAYLOAD;

    if(old_cap < need){return;}

    old_header->capacity = aligned_expected_size;
    
    alloc_data_t * new_header = (alloc_data_t *)((char *)old_header + ALIGNED_HEADER_SIZE + aligned_expected_size);
    new_header->capacity = old_cap - aligned_expected_size - ALIGNED_HEADER_SIZE;
    new_header->expected_size = 0;
    new_header->is_free = true;
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

static alloc_data_t *heap_walk(size_t aligned_expected_size){
    char *mem_iterator = HEAP_START;
    char *brk = (char *)sbrk(0);
    alloc_data_t *casted_iterator = (alloc_data_t*)mem_iterator;

    while (mem_iterator < brk && (casted_iterator->is_free != true || casted_iterator->capacity < aligned_expected_size))
    {
        mem_iterator += casted_iterator->capacity + ALIGNED_HEADER_SIZE;
        casted_iterator = (alloc_data_t*)mem_iterator;
    }
    
    if(mem_iterator < brk){
        return casted_iterator;
    }
    return NULL;
}

void *l_malloc(size_t expected_size){

    if(expected_size == 0){
        return NULL;
    }

    if(!HEAP_START && (heap_init() == -1)){
        return NULL;
    }

    size_t aligned_expected_size;
    if(align_up(expected_size,&aligned_expected_size) == -1){
        errno = EOVERFLOW;
        return NULL;
    }

    //search for free allocated mem
    alloc_data_t *found_mem = NULL;
    if((found_mem = heap_walk(aligned_expected_size)) != NULL){

        split_block(found_mem, aligned_expected_size);

        found_mem->is_free = false;
        found_mem->expected_size = expected_size;

        return user_ptr_from_header(found_mem);
    }

    //no mem found ==> new allocation

    if(aligned_expected_size > SIZE_MAX - ALIGNED_HEADER_SIZE){
        errno = EOVERFLOW;
        return NULL;
    }

    size_t total_size = aligned_expected_size + ALIGNED_HEADER_SIZE;

    alloc_data_t *allocated = sbrk(total_size);
    if(allocated == (void *)-1){
        errno = ENOMEM;
        return NULL;
    }

    allocated->capacity = aligned_expected_size;
    allocated->expected_size = expected_size;
    allocated->is_free = false;

    return user_ptr_from_header(allocated);
}

void l_free(void *ptr){
    if(ptr == NULL) return;
    alloc_data_t *h = (alloc_data_t *)((char *)ptr - ALIGNED_HEADER_SIZE);
    h->is_free = true;
}