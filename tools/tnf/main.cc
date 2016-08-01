#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "tnf_main.h"

int main(int argc, char **argv)
{
    printf("version:%s\n", get_version());
    return tnf_main(argc, argv);
}
