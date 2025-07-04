// Copyright, 2024, Pebblebed Management LLC. All rights reserved, pal.
#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

#include "types.h"
#include "position.h"

struct Stage {
    const char* name;
    const pct_t next_stage_factor;
    const pct_t option_pool;
    const pct_t dilution;
    // State transition probabiblities
    const prob_t p_write_off;
    const prob_t p_exit;
    typedef enum {
        CONTINUE,
        DIE,
        EXIT,
    } Result;

    Result traverse_stage(Asset& asset) const;
};

extern const int num_stages;
extern const Stage stages[];
typedef std::unordered_map<std::string, const Stage&> StageMap;
const StageMap& get_stage_map();
