#include <cstdio>
#include "stage.h"
#include "fund.h"

int main(int, char**) {
    printf("yarr ...\n");
    for (int i = 0; i < num_stages; i++) {
        printf("s[%s]: %g\n", stages[i].name, stages[i].p_exit);
    }

    Fund pb1 { 38.5e6, 1.5e6 };
}
