#include "buffer.h"
#include <cstdlib>
#include <cstring>

struct buf_chain_t {

    buf_chain_t* next;
    uint32_t buffer_len;
    uint32_t misalign;
    uint32_t off;
    uint8_t* buffer;
};

struct buffer_t {

    buf_chain_t* first;
    buf_chain_t* last;
    buf_chain_t** last_with_datap;
    uint32_t total_len;
    uint32_t last_read_pos;
};

#define CHAIN_SPACE_LEN(ch) ((ch)->buffer_len - ((ch)->misalign + (ch)->off))
#define MIN_BUFFER_SIZE 1024
#define MAX_TO_COPY_IN_EXPEND 4096
#define BUFFER_CHAIN_MAX_AUTO_SIZE 4096
#define MAX_TO_REALIGN_IN_EXPAND 2048
#define BUFFER_CHAIN_MAX 16 * 1024 * 1024
#define BUFFER_CHAIN_EXTRA(t, c) (t *)((buf_chain_t *)(c) + 1)
#define BUFFER_CHAIN_SIZE sizeof(buf_chain_t)

uint32_t buffer_len(buffer_t* buf) {

    return buf->total_len;
}

buffer_t* buffer_new(uint32_t sz) {

    (void)sz;
    buffer_t *buf = (buffer_t *)malloc(sizeof(buffer_t));
    if (!buf) {
        return nullptr;
    }

    memset(buf, 0, sizeof(buffer_t));
    buf->last_with_datap = &buf->first;
    return buf;
}

static buf_chain_t* buf_chain_new(uint32_t size) {

    buf_chain_t *chain;
    uint32_t to_alloc;
    if (size > BUFFER_CHAIN_MAX - BUFFER_CHAIN_SIZE) return nullptr;

    size += BUFFER_CHAIN_SIZE;

    if (size < BUFFER_CHAIN_MAX / 2) {
        to_alloc = MIN_BUFFER_SIZE;
        while (to_alloc < size) {
            to_alloc <<= 1;
        }
    } else {
        to_alloc = size;
    }
    if ((chain = (buf_chain_t*)(to_alloc)) == nullptr) {
        return nullptr;
    }
    memset(chain, 0, BUFFER_CHAIN_SIZE);
    chain->buffer_len = to_alloc - BUFFER_CHAIN_SIZE;
    chain->buffer = BUFFER_CHAIN_EXTRA(uint8_t, chain);

    return chain;
}

static void buf_chain_free_all(buf_chain_t* chain) {
    buf_chain_t* next;
    for (; chain; chain = next) {
        next = chain->next;
        free(chain);
    }
}

void buffer_free(buffer_t* buf) {
    if (buf) {
        buf_chain_free_all(buf->first);
    }
}

static buf_chain_t** free_empty_chains(buffer_t* buf) {

    buf_chain_t** ch = buf->last_with_datap;
    while ((*ch) && (*ch)->off != 0) {
        ch = &(*ch)->next;
    }
    if (*ch) {
        buf_chain_free_all(*ch);
        *ch = nullptr;
    }
    return ch;
}

static void buf_chain_insert(buffer_t* buf, buf_chain_t* chain) {

    if (*buf->last_with_datap == nullptr) {
        buf->first = buf->last = chain;
    } else {
        buf_chain_t** chp;
        chp = free_empty_chains(buf);
        *chp = chain;
        if (chain->off)
            buf->last_with_datap = chp;
        buf->last = chain;
    }
}