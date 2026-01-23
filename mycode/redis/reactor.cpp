#include "reactor.h"
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

reactor_t *create_reactor() {

    reactor_t *r = (reactor_t*)malloc(sizeof(reactor_t));
    r->epfd = epoll_create(1);
    r->listenfd = 0;
    r->stop = 0;
    r->iter = 0;
    r->events = (event_t*)malloc(sizeof(event_t) * MAX_CONN);
    memset(r->events, 0, sizeof(event_t) * MAX_CONN);
    memset(r->fire, 0, sizeof(struct epoll_event) * MAX_EVENT_NUM);
    return r;
}

void release_reactor(reactor_t *r) {

    free(r->events);
    close(r->epfd);
    free(r);
}

static event_t *_get_event_t(reactor_t *r) {
    
    r->iter++;
    while (r->events[r->iter & MAX_CONN].fd > 0) {
        r->iter++;
    }
    return &r->events[r->iter];
}

event_t *new_event(reactor_t *r, int fd, \
    event_callback_fn rd, event_callback_fn wt, error_callback_fn err) {

    event_t *e = _get_event_t(r);
    e->r = r;
    e->fd = fd;
    e->in = static_cast<uint8_t*>(malloc(1024));
    memset(e->in, 0, 1024);
    e->out = static_cast<uint8_t*>(malloc(1024));
    memset(e->out, 0, 1024);
    e->read_fn = std::move(rd);
    e->write_fn = std::move(wt);
    e->error_fn = std::move(err);
    return e;
}

uint8_t *evbuf_in(event_t* e) {
    return e->in;
}

uint8_t *evbuf_out(event_t* e) {
    return e->out;
}

reactor_t *event_base(event_t *e) {
    return e->r;
}

void free_event(event_t *e) {
    free(e->in);
    free(e->out);
}

int set_nonblock(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

int add_event(reactor_t *r, int events, event_t *e) {

    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = e;
    if (epoll_ctl(r->epfd, EPOLL_CTL_ADD, e->fd, &ev) == -1) {
        std::cout << "add event err fd = " << e->fd << std::endl;
        return 1;
    }
    return 0;
}

int del_event(reactor_t *r, event_t *e) {

    epoll_ctl(r->epfd, EPOLL_CTL_DEL, e->fd, nullptr);
    free_event(e);
    return 0;
}

int enable_event(reactor_t *r, event_t *e, int readable, int writeable) {
    struct epoll_event ev;
    ev.events = (readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0);
    ev.data.ptr = e;
    if (epoll_ctl(r->epfd, EPOLL_CTL_MOD, e->fd, &ev) == -1) {
        return 1;
    }
    return 0;
}

void eventloop_once(reactor_t *r, int timeout) {

    int n = epoll_wait(r->epfd, r->fire, MAX_EVENT_NUM, timeout);
    for (int i = 0; i < n; ++i) {
        struct epoll_event *e = &r->fire[i];
        int mask = e->events;
        if (e->events & EPOLLERR)
            mask |= EPOLLIN | EPOLLOUT;
        if (e->events & EPOLLHUP)
            mask |= EPOLLIN | EPOLLOUT;
        event_t *et = (event_t *)e->data.ptr;
        if (mask & EPOLLIN) {
            if (et->read_fn)
                et->read_fn(et->fd, EPOLLIN, et);
        }
        if (mask & EPOLLOUT) {
            if (et->write_fn)
                et->write_fn(et->fd, EPOLLOUT, et);
            else {
                uint8_t *buf = (evbuf_out(et));
                event_buffer_write(et, buf, strlen(reinterpret_cast<char*>(evbuf_out(et))));
            }
        }
    }
}

void stop_eventloop(reactor_t *r) {
    r->stop = 1;
}

void eventloop(reactor_t *r) {
    while (!r->stop) {
        eventloop_once(r, -1);
    }
}

int create_server(reactor_t *r, short port, event_callback_fn func) {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        std::cout << "create listenfd error!" << std::endl;
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(int)) == -1) {
        std::cout << "reuse address error: " << strerror(errno) << std::endl;
        close(listenfd);
        return -1;
    }

    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
        std::cout << "bind error " << strerror(errno) << std::endl;
        close(listenfd);
        return -1;
    }

    if (listen(listenfd, 1024) < 0) {
        std::cout << "listen error " << strerror(errno) << std::endl;
        close(listenfd);
        return -1;
    }

    if (set_nonblock(listenfd) < 0) {
        std::cout << "set_nonblock error " << strerror(errno) << std::endl;
        close(listenfd);
        return -1;
    }

    r->listenfd = listenfd;
    event_t *e = new_event(r, listenfd, func, nullptr, nullptr);
    add_event(r, EPOLLIN, e);

    std::cout << "listen port : " << port << std::endl;
    return 0;
}

int event_buffer_read(event_t *e) {

    int fd = e->fd;
    int num = 0;
    while (true) {
        int n = read(fd, evbuf_in(e) + num, 1023 - num);
        if (n == 0) {
            std::cout << "close connection fd = " << fd << std::endl;
            if (e->error_fn)
                e->error_fn(fd, "close socket");
            del_event(e->r, e);
            close(fd);
            return 0;
        } else if (n < 0) {
            if (errno == EINTR)
                continue;
            if (errno == EWOULDBLOCK)
                break;
            std::cout << "read error fd = " << fd << " err = " << strerror(errno) << std::endl;
            if (e->error_fn)
                e->error_fn(fd, strerror(errno));
            del_event(e->r, e);
            close(fd);
            return -1;
        } else {
            std::cout << "recv data from client:" << evbuf_in(e) << std::endl;
        }
        num += n;
    }
    return num;
}

static int _write_socket(event_t *e, void *buf, int sz) {
    int fd = e->fd;
    while (true) {
        int n = write(fd, buf, sz);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            if (errno == EWOULDBLOCK)
                break;
            if (e->error_fn)
                e->error_fn(fd, strerror(errno));
            del_event(e->r, e);
            close(e->fd);
        }
        return n;
    }
    return 0;
}

int event_buffer_write(event_t *e, void *buf, int sz) {
    uint8_t *out  = evbuf_out(e);
    if (strlen(reinterpret_cast<char*>(out)) == 0) {
        int n = _write_socket(e, buf, sz);
        if (n == 0 || n < sz) {
            // 发送失败，除了将没有发送出去的数据写入缓冲区，还要注册写事件
            //buffer_add(out, (char*)buf + n, sz - n);
            buf = (char*)buf + n;
            sz -= n;
            enable_event(e->r, e, 1, 1);
            return 0;
        } else if (n < 0) {
            return 0;
        }
        return 1;
    }
    return 1;
}