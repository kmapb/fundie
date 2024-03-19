#include <gtest/gtest.h>
#include "../fund.h"

TEST(Fund, Init) {
    Fund f(38e6, 1.5e6, 0.2);
    Asset a {"Apple", 1e9};
    f.deploy(a, 1e6);

    auto& pos = f.get_position(a);
    EXPECT_DOUBLE_EQ(pos.ownership(), 1e-3);
    EXPECT_EQ(pos.cost(), 1e6);
}

