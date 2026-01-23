#include "coroutine.h"
#include <sys/epoll.h>


int epoller_create() {
    return epoll_create(1);
}