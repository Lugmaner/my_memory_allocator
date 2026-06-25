#include "l_mem_alloc.h"

int main(void){
    void *before = sbrk(0);

    size_t *arr0 = l_malloc(3 * sizeof(size_t));
    void *afterfirst = sbrk(0);
    if(!arr0){ perror("l_malloc"); exit(1); }

    size_t *arr1 = l_malloc(3 * sizeof(size_t));
    void *aftersecond = sbrk(0);
    if(!arr1){ perror("l_malloc"); exit(1); }

    l_free(arr0);
    void *afterFree = sbrk(0);

    size_t *arr2 = l_malloc(3 * sizeof(size_t));
    void *afterthird= sbrk(0);

    l_free(arr2);
    afterFree = sbrk(0);

    size_t *arr3 = l_malloc(5 * sizeof(size_t));
    void *afterfourth = sbrk(0);
    if(!arr3){ perror("l_malloc"); exit(1); }

    printf("before heap alignment and allocation = %p\n", (void*)before);

    printf("after heap alignment and first allocation = %p\n", (void*)afterfirst);
    printf("after second allocation = %p\n", (void*)aftersecond);
    printf("after free = %p\n", (void*)afterFree);
    printf("after third allocation = %p\n", (void*)afterthird);
    printf("after free = %p\n", (void*)afterFree);
    printf("after fourht allocation = %p\n", (void*)afterfourth);
    printf("diff = %td\n", (char*)afterthird - (char*)before);
    exit(0);
}