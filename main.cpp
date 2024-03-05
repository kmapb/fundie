#include <cstdio>
#include "stage.h"
#include "position.h"

int main(int argc, char** argv) {
    printf("yarr ...\n");
    for (int i = 0; i < num_stages; i++) {
        printf("s[%s]: %g\n", stages[i].name, stages[i].p_exit);
    }
}
