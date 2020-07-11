//
// Created by Pioooooo on 2020/7/8.
//

#ifndef RISC_V_DEBUG_H
#define RISC_V_DEBUG_H

#include <iostream>

#define __DEBUG 0

#define DEBUG(fmt, ...) \
        do { if (__DEBUG) fprintf(stderr, fmt "\n", __VA_ARGS__); } while (0)

#endif //RISC_V_DEBUG_H
