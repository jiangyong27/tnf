#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "so_loader.h"
#include "net/net_conf.h"

const char *version = "1.0.0";
const char *get_version()
{
    return version;
}

int tnf_main(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s conf.ini\n", argv[0]);
        return -1;
    }

    std::string conf_file = argv[1];
    std::string so_file;

    CNetConf net_conf;
    if (!net_conf.OpenIniFile(conf_file)) {
        printf("open conf[%s] failed!\n", conf_file.c_str());
        return -2;
    }
    net_conf.GetFiledValue("main", "app_so", "", &so_file);

    CSoLoader so_loader;
    if (!so_loader.Open(so_file)) {
        printf("open so_file[%s] failed!\n", so_file.c_str());
        return -3;
    }

    typedef int (*app_run_func)(int argc, char **argv);
    app_run_func app_run = (app_run_func) so_loader.GetFunc("app_run");
    if (app_run == NULL) {
        printf("get func app_run failed!\n");
        return -2;
    }

    return app_run(argc, argv);
}
