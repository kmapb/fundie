#include "stage.h"
#include "round.h"

const Stage stages[] = {
    //Name          Growth  options dilution p(die) p(exit)
    { "Seed",       2.5,    0.18,   0.22,    0.50,  0.03 },
    { "Series A",   2.5,    0.12,   0.22,    0.35,  0.07 },
    { "Series B",   2.0,    0.08,   0.18,    0.25,  0.10 },
    { "Series C",   1.75,   0.06,   0.15,    0.20,  0.15 },
    { "Series D",   1.5,    0.04,   0.12,    0.15,  0.20 },
    { "Series E",   1.3,    0.02,   0.10,    0.10,  0.30 },
    { "Series F",   1.25,   0.00,   0.10,    0.05,  0.50 },
    { "IPO",        1.00,   0.00,   0.00,    0.00,  1.00 },
};

const int num_stages = sizeof(stages) / sizeof(Stage);


const StageMap& get_stage_map() {
    static StageMap stageMap;
    if (stageMap.size() == num_stages) return stageMap;
    assert(stageMap.size() == 0);

    for (const auto& stage: stages) {
        std::string sn { stage.name };
        stageMap.emplace(sn, stage);
    }
    assert(stageMap.size() == num_stages);
    return stageMap;
}

Stage::Result
Stage::traverse_stage(Asset& asset) const {
    auto result = Result::CONTINUE;
    // Normalize into [0, 1) so p=1.0 remains deterministic.
    auto dice = double(random()) / (double(RAND_MAX) + 1.0);
    if (dice < p_write_off) {
        asset.set_value(0.0);
        return Result::DIE;
    }
    dice -= p_write_off;
    if (dice < p_exit) {
        result = Result::EXIT;
    }

    asset.set_value(asset.get_value() * next_stage_factor);
    asset.accept_new_money(0.2 * asset.get_value()); // XXX: hardcoded 20% dilution
    asset.reserve_option_pool(option_pool);
    return result;
}
