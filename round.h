#pragma once

#include <cassert>

struct Round {
    double preMV;
    double postMV;
    double newMoney() const {
        assert(postMV > preMV);
        return postMV - preMV;
    }

    void setNewMoney(double newMoney) {
        assert(newMoney >= 0.0);
        preMV = postMV - newMoney;
    }
};

double computeProRata(Round &round,
                      double prevOwnership,
                      double targetOwnership=-1.0)
{
    if (targetOwnership < 0.0) {
        targetOwnership = prevOwnership;
    }
    const double moneyInSoFar = round.newMoney();

    double dilution = round.newMoney() / round.postMV;
    assert(dilution > 0.0 && dilution < 1.0);

    double dilutedO = prevOwnership * (1.0 - dilution);
    assert(dilutedO > 0.0 && dilutedO < 1.0);

    double approxProRata = (targetOwnership - dilutedO) * round.postMV;
    round.setNewMoney(moneyInSoFar + approxProRata);
    return approxProRata;
}

