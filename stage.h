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
    typedef enum {
        CONTINUE,
        DIE,
        EXIT,
    } Result;

    Result traverse_stage(Asset& asset);
    Result pro_rata(Asset& asset, Position& position, currency_t& out_currency, double pro_rata_rate = 1.0);
};

extern const int num_stages;
extern const Stage stages[];

