#include <stdio.h>
#include "global.h"


extern void go();

int main(int argc, char** argv)
{
    initialize_pl();
    go(argv[1]);
    return 0;
}
