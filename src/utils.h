#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

//#define LS_DEBUG
#ifdef LS_DEBUG
// Here we use stderr because stdout maybe used as default saver.
#define LSLOG(fmt, args...) \
    fprintf(stderr, "%s %d: " fmt "\n", __FUNCTION__, __LINE__, ##args);
#else
#define LSLOG(fmt, args...)
#endif

#include <pthread.h>
namespace logsaver {
// Thread specific data control
static pthread_once_t s_once = PTHREAD_ONCE_INIT;
static pthread_key_t s_ptr_key;
static inline void init_once(void) {
    pthread_key_create(&s_ptr_key, NULL);
}
static inline void set_specific(void *p) {
    pthread_once(&s_once, init_once);
    pthread_setspecific(s_ptr_key, p);
}
static inline void *get_specific() {
    pthread_once(&s_once, init_once);
    return pthread_getspecific(s_ptr_key);
}
} //namespace logsaver

#include <sys/time.h>
namespace logsaver {
// Timer functions for timeout
static inline void set_oneshot_timer(int timeout_ms) {
    struct itimerval new_value, old_value;
    new_value.it_value.tv_sec = timeout_ms/1000;
    new_value.it_value.tv_usec = (timeout_ms%1000)*1000;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &new_value, &old_value);
}
static inline void unset_oneshot_timer() {
    set_oneshot_timer(0);
}
} //namespace logsaver

#endif //UTILS_H
