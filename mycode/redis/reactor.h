#ifndef _QIAO_REACTOR_H
#define _QIAO_REACTOR_H

#include <functional>

struct reactor_t;
using event_callback_fn = std::function<void(int, int, void*)>;
using error_callback_fn = std::function<void(int, char*)>;

struct event_t {

    int fd;
    reactor_t *r;
    buffer_t *in;
    buffer_t *out;

};



#endif //_QIAO_REACTOR_H