#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "so_loader.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s so_file\n", argv[0]);
        return -1;
    }

    std::string so_file = argv[1];
    CSoLoader so_loader;
    if (!so_loader.Open(so_file)) {
        printf("open so_file[%s] failed!\n", so_file.c_str());
        return -2;
    }
    return 0;
}
