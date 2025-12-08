//#define _GNU_SOURCE
#include <dlfcn.h>
#include <link.h>
#include <stdlib.h>
//#include <iostream>
#include <stdio.h>
#include <unistd.h>



#if 0
void *nmalloc(size_t size, const char* filename, int line) {


    void *p = malloc(size);

    char buff[128] = {0};
    snprintf(buff, 128, "./mem/%p.mem", p);

    FILE * fp = fopen(buff, "w");
    if (!fp) {
        free(p);
        return NULL;
    }

    fprintf(fp, "[+]%s:%d, addr: %p, size: %ld\n", filename, line, p, size);
    fclose(fp);
    //printf("nmalloc: %p, size: %ld\n", p, size);
    return p;
}

void nfree(void *ptr) {

    char buff[128] = {0};

    snprintf(buff, 128, "./mem/%p.mem", ptr);
    if (unlink(buff) < 0) {
        printf("double free: %p", ptr);
        return;
    }
    //printf("nfree: %p\n", ptr);

    free(ptr);
}

#define malloc(size) nmalloc(size, __FILE__, __LINE__)
#define free(ptr) nfree(ptr)

#else

#ifdef __cplusplus
extern "C" {
#endif
//hook
typedef void *(*malloc_t)(size_t size);
malloc_t malloc_f = NULL;

typedef void(*free_t)(void *ptr);
free_t free_f = NULL;

int enable_malloc = 1;
int enable_free = 1;

void *convertToELF(void *addr) {
    Dl_info info;
    struct link_map *link;
    dladdr1(addr, &info, (void**)&link, RTLD_DL_LINKMAP);

    return (void*)((size_t)addr - (size_t)(link->l_addr));
}

void *malloc(size_t size) {

    void *p = NULL;
    if (enable_malloc) {
        enable_malloc = 0;
        p = malloc_f(size);

        void *caller = __builtin_return_address(0); //0返回上一级, 1返回上一级的上一级，依次...
        char buff[128] = {0};
        snprintf(buff, 128, "./mem/%p.mem", p);

        FILE *fp = fopen(buff, "w");
        if (!fp)
        {
            free(p);
            return NULL;
        }
//addr2line -f -e ../memleak -a 0x168e
        fprintf(fp, "[+]%p, addr: %p, size: %ld\n", convertToELF(caller), p, size);
        fclose(fp);

        enable_malloc = 1;
    } else {
        p = malloc_f(size);
    }

    return p;
}

void free(void *ptr) {

    if (enable_free) {
        enable_free = 0;
        char buff[128] = {0};

        snprintf(buff, 128, "./mem/%p.mem", ptr);
        if (unlink(buff) < 0)
        {
            printf("double free: %p\n", ptr);
            enable_free = 1;
            return;
        }
        free_f(ptr);

        enable_free = 1;
    } else {
        free_f(ptr);
    }

    return;
}

void init_hook(void) {

    if (!malloc_f) {
        //malloc_f = reinterpret_cast<malloc_t>(dlsym(RTLD_NEXT, "malloc"));
        malloc_f = (malloc_t)dlsym(RTLD_NEXT, "malloc");
    }

    if (!free_f) {
        //free_f = reinterpret_cast<free_t>(dlsym(RTLD_NEXT, "free"));
        free_f = (free_t)dlsym(RTLD_NEXT, "free");
    }
}

#ifdef __cplusplus
}
#endif

#endif



// __FILE__, __LINE__, __func__

int main() {
    init_hook();
#if 1
    void *p1 = malloc(5);
    void *p2 = malloc(10);
    void *p3 = malloc(15);
    void *p4 = malloc(10);

    free(p1);
    free(p3);
    free(p4);
#else
    void *p1 = nmalloc(5);
    void *p2 = nmalloc(10);
    void *p3 = nmalloc(15);

    nfree(p1);
    nfree(p3);
#endif

    //getchar();
}
