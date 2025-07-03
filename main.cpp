#include <cstdio>
#include "stage.h"
#include "fund.h"

int main(int, char**) {
    printf("yarr ...\n");
    for (int i = 0; i < num_stages; i++) {
        printf("s[%s]: %g\n", stages[i].name, stages[i].p_exit);
    }

    Fund pb2 { 1.26e8, 1.5e6 };
    Asset orbit {"Orbit",    4.625e7 - 4.6e6};
    pb2.deploy(orbit, YMD(2025, 2, 1), 4.6e6);
    auto& pos = pb2.get_position(orbit);
    printf("ownership: %g\n", pos.ownership());
    printf("company valuation: %g\n", orbit.value());
}