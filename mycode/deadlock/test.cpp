#define _GUN_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <iostream>

#define MAX 100

enum Type {PROCESS, RESOURCE};

struct source_type {

    uint64_t id;
    enum Type type;

    uint64_t lock_id;
    int degress;
};

struct vertex {

    struct source_type s;
    struct vertex *next;
};

struct task_graph {

    struct vertex list[MAX];
    int num;

    struct source_type locklist[MAX];
    int lockidx;

    pthread_mutex_t mutex;
};

struct task_graph *tg = NULL;
int path[MAX + 1];
int visited[MAX];
int k = 0;
int deadlock = 0;

struct vertex *create_vertex(struct source_type& type) {

    struct vertex* tex = (struct vertex*)malloc(sizeof(struct vertex));

    tex->s = type;
    tex->next = NULL;

    return tex;
}

int search_vertex(struct source_type& type) {

    int i = 0;
    for (i = 0; i < tg->num; ++i) {
        if (tg->list[i].s.type == type.type && tg->list[i].s.id == type.id) {
            return i;
        }
    }

    return -1;
}

void add_vertex(struct source_type& type) {

    if (search_vertex(type) == -1) {
        tg->list[tg->num].s = type;
        tg->list[tg->num].next = NULL;
        tg->num++;
    }
}

void add_edge(struct source_type& from, struct source_type& to) {

    add_vertex(from);
    add_vertex(to);

    struct vertex* v = &(tg->list[search_vertex(from)]);

    while (v->next != NULL) {
        v = v->next;
    }

    v->next = create_vertex(to);
}

int verify_edge(struct source_type& i, struct source_type& j) {

    if (tg->num == 0) return 0;

    int idx = search_vertex(i);
    if (idx == -1) return 0;

    struct vertex *v = &(tg->list[idx]);
    while (v != NULL) {
        if (v->s.id == j.id) return 1;
        v = v->next;
    }

    return 0;
}

void remove_edge(struct source_type& from, struct source_type& to) {

    int idxi = search_vertex(from);
    int idxj = search_vertex(to);

    if (idxi != -1 && idxj != -1) {
        struct vertex *v = &tg->list[idxi];
        struct vertex *remove;

        while (v->next != NULL) {
            if (v->next->s.id == to.id) {
                remove = v->next;
                v->next = v->next->next;

                free(remove);
                break;
            }

            v = v->next;
        }
    }
}

void print_deadlock() {
    int i = 0;

    printf("cycle : ");
    for (i = 0; i < k - 1; ++i) {
        printf("%ld --> ", tg->list[path[i]].s.id);
    }

    printf("%ld\n", tg->list[path[i]].s.id);
}