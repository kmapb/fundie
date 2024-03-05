// Copyright, 2024, Pebblebed Management LLC. All rights reserved, pal.
#pragma once

#include <cstdint>
#include "position.h"

typedef double currency_t;
typedef double pct_t;
typedef double prob_t;

struct Stage {
    const char* name;
    const currency_t round_size;
    const currency_t post_money_valuation;
    const pct_t option_pool;

    // State transition probabiblities
    const prob_t p_write_off;
    const prob_t p_exit;
};

extern const int num_stages;
extern const Stage stages[];

typedef enum {
    CONTINUE,
    DIE,
    EXIT,
} StageResult;

extern Position
traverse_stage(const Stage& before, const Stage& after, const Position& in);
