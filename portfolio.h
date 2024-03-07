#pragma once
#include <vector>
#include "position.h"

struct Portfolio {
    std::vector<Position> positions;

    double average_ownership() const {
        auto total = std::accumulate(positions.begin(), positions.end(), 0.0,
            [](double left, const Position& right) {
                return left + right.ownership();
            });
        return total / positions.size();
    }

    double value() const {
        return std::accumulate(positions.begin(), positions.end(), 0.0,
            [](double left, const Position& right) {
                return left + right.value();
            });

    }

    double cost() const {
        return std::accumulate(positions.begin(), positions.end(), 0.0,
            [](double left, const Position& right) {
                return left + right.cost();
            });
    }

    void add_position(const Position& pos) {
        positions.push_back(pos);
    }
};