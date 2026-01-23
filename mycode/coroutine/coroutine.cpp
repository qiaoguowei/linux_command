#include "coroutine.h"
#include <assert.h>
#include <cstdlib>
#include <unistd.h>

pthread_key_t global_sched_key;
static pthread_once_t sched_key_once = PTHREAD_ONCE_INIT;

static void coroutine_sched_key_destructor(void *data) {
    free(data);
}

static void __attribute__((constructor(1000))) coroutine_sched_key_creator() {

    assert(pthread_key_create(&global_sched_key, coroutine_sched_key_destructor) == 0);
    assert(pthread_setspecific(global_sched_key, NULL) == 0);

    return;
}

int coroutine_create(Coroutine **new_co, proc_coroutine func, void *arg) {

    assert(pthread_once(&sched_key_once, coroutine_sched_key_creator) == 0);
    Schedule *sched = coroutine_get_sched();

    if (sched == nullptr) {
        schedule_create(0);

        sched = coroutine_get_sched();
        if (sched == nullptr) {
            Output::print("Failed to create scheduler");
            return -1;
        }
    }

    Coroutine *co = static_cast<Coroutine*>(calloc(1, sizeof(Coroutine)));
    if (co == nullptr) {
        Output::print("Failed to allocate memory for new coroutine");
        return -2;
    }

#ifdef _USE_UCONTEXT
    co->stack = nullptr;
    co->stack_size = 0;
#else
    int ret = posix_memalign(&co->stack, getpagesize(), sched->stack_size);
    if (ret) {
        Output::print("Failed to allocate stack for new coroutine");
        free(co);
        return -3;
    }
#endif
    co->sched = sched;
    co->status = 
}