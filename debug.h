#pragma once

#ifndef NDEBUG
#define dbg(module, X) do { \
    if (DBG.module) { \
        printf X ; \
        fflush(stdout); \
    } \
} while(0)
#else
#define dbg(module, ...) do {} while(0)
#endif

#define DBG_MODULES \
   X(fund) \
   X(position) \
   X(stage) \
   X(harness)

extern struct DBGModule {
    #define X(module) bool module;
    DBG_MODULES
    #undef X
    DBGModule();
} DBG;
