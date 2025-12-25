#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

//LD_PRELOAD=./syscall.so /etc/init.d/mysql start

#if DEBUG_ENABLE
#define dblog(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define dblog(fmt, ...)
#endif

typedef int (*open_t)(const char *pathname, int flags);
open_t open_f = NULL;

typedef int (*creat_t)(const char *pathname, mode_t mode);
creat_t creat_f = NULL;

typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
read_t read_f = NULL;

typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
write_t write_f = NULL;

typedef int (*close_t)(int fd);
close_t close_f = NULL;

typedef off_t (*lseek_t)(int fd, off_t offset, int whence);
lseek_t lseek_f = NULL;

int open(const char *pathname, int flags) {
    if (!open_f) {
        open_f = dlsym(RTLD_NEXT, "open");
    }
    dblog("open ...%s\n", pathname);

    return open_f(pathname, flags);
}

int creat(const char *pathname, mode_t mode) {
    if (!creat_f) {
        creat_f = dlsym(RTLD_NEXT, "creat");
    }
    dblog("creat ...\n");

    return creat_f(pathname, mode);
}

ssize_t read(int fd, void *buf, size_t count) {
    if (!read_f) {
        read_f = dlsym(RTLD_NEXT, "read");
    }
    dblog("read ...\n");

    return read_f(fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count) {
    if (!write_f) {
        write_f = dlsym(RTLD_NEXT, "write");
    }
    dblog("write ...\n");

    return write_f(fd, buf, count);
}

int close(int fd) {
    if (!close_f) {
        close_f = dlsym(RTLD_NEXT, "close");
    }
    dblog("close ...\n");

    return close_f(fd);
}

off_t lseek(int fd, off_t offset, int whence) {
    if (!lseek_f) {
        lseek_f = dlsym(RTLD_NEXT, "lseek");
    }
    dblog("lseek ...\n");

    return lseek_f(fd, offset, whence);
}