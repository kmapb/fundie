#include "fund.h"

Position &Fund::get_position(Asset &a);
{
    auto p = positions_.find(a.name());
    if (p != positions_.end()) {
        return p->second;
    }
    positions_.emplace(a.name(), Position(a));
    auto ret = positions_.find(a.name());
    assert(ret != positions_.end());
    return ret->second;
}

void Fund::deploy(Asset &a, CalTime ymd, const double value) {
    assert(value <= dry_powder());
    auto &p = get_position(a);
    auto shares = a.accept_new_money(value);
    p.buy(ymd, value, shares);
    deployed_ += value;

    assert(get_position(a).ownership() <= 1.0);
    assert(get_position(a).cost() >= value);
}
