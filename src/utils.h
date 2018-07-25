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

#endif //UTILS_H
