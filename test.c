#include "l_mem_alloc.h"

int main(void){
    void *before = sbrk(0);
    size_t *arr = l_malloc(3, sizeof(size_t));
    void *after = sbrk(0);
    if(!arr){ perror("l_malloc"); exit(1); }

    printf("diff = %td\n", (char*)after - (char*)before);
    exit(0);
}