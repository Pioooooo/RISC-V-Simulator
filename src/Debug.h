//
// Created by Pioooooo on 2020/7/8.
//

#ifndef RISC_V_DEBUG_H
#define RISC_V_DEBUG_H

#include <iostream>

#define __DEBUG 1

#define DEBUG(fmt, ...) \
        do { if (__DEBUG) fprintf(stderr, "%s:%d\t" fmt "\n", __FILE__, __LINE__, __VA_ARGS__); } while (0)

#endif //RISC_V_DEBUG_H
