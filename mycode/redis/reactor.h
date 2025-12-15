#ifndef _QIAO_REACTOR_H
#define _QIAO_REACTOR_H

#include <functional>
#include <sys/epoll.h>

#define MAX_EVENT_NUM 512
#define MAX_CONN ((1<<16)-1) //65535

struct reactor_t;
using event_callback_fn = std::function<void(int, int, void*)>;
using error_callback_fn = std::function<void(int, char*)>;

struct event_t {

    int fd;
    reactor_t *r;
    buffer_t *in;
    buffer_t *out;

    event_callback_fn read_fn;
    event_callback_fn write_fn;
    error_callback_fn error_fn;
};

struct reactor_t {

    int epfd;
    int listenfd;
    int stop;
    event_t *events;
    int iter;
    struct epoll_event fire[MAX_EVENT_NUM];
};

int event_buffer_read(event_t *e);
int event_buffer_write(event_t *e, void *buf, int sz);

reactor_t *create_reactor();

void release_reactor(reactor_t *r);

event_t *new_event(reactor_t *r, int fd, \
    event_callback_fn rd, event_callback_fn wr, error_callback_fn err);

void free_event(event_t *e);

buffer_t *evbuf_in(event_t *e);
buffer_t *evbuf_out(event_t *e);

reactor_t *event_base(event_t *e);

int set_nonblock(int fd);

int add_event(reactor_t *r, int events, event_t *e);

int del_event(reactor_t *r, event_t *e);

int enable_event(reactor_t *r, event_t *e, int readable, int writeable);

void eventloop_once(reactor_t *r, int timeout);

void stop_eventloop(reactor_t *r);

void eventloop(reactor_t *r);

int create_server(reactor_t *r, short port, event_callback_fn func);

int event_buffer_read(event_t *e);

int event_buffer_write(event_t *e, void *buf, int sz);



#endif //_QIAO_REACTOR_H


