#include <gtest/gtest.h>

#include "../position.h"
#include "../stage.h"

TEST(Stage, evolve) {
    //Name, scale, options, p(die), p(exit)
    Stage stage {"Inception", 2.0, 0.1, 0.0, 0.0 };
    Asset asset { "Dropbox", 5e6 };
    stage.traverse_stage(asset);
    EXPECT_EQ(asset.value(), 12e6);
}