#include <gtest/gtest.h>

#include "../position.h"
#include "../stage.h"

TEST(Stage, evolve) {
    //Name, Round Size, post, options, p(die), p(exit)
    Stage stage {"Gonzito", 1e6, 10e6, 0.1, 0.0, 0.0 };
    Asset asset { "Dropbox", 5e5 };
    stage.traverse_stage(asset);
    EXPECT_EQ(asset.value(), 10e6);
}