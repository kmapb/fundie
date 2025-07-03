#include "stage.h"
#include "round.h"

constexpr double M = 1e6;
constexpr double B = 1e9;
const Stage stages[] = {
    //Name          Growth    options p(die)  p(exit)
    { "Seed",       2.25,     0.2,    0.35,   0.1 },
    { "Series A",   2.75,     0.15,   0.30,   0.15 },
    { "Series B",   2.5,      0.1,    0.23,   0.175 },
    { "Series C",   2.75,     0.25,   0.23,   0.2},
    { "Series D",   2.75,     0.04,   0.20,   0.3 },
    { "Series E",   2.25,     0,      0.15,   0.35 },
    { "Series F",   3.00,     0,      0.1,    0.5},
    { "IPO",        3.00,     0,      0.00,   1.0 },
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
    auto dice = double(random()) / RAND_MAX;
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
