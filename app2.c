#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zcs.h"

void hello(char* s, char* r) {
    printf("Ad received: %s, with value: %s\n", s, r);
    zcs_log();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: app2 <LAN IP ADDR> \n");
        exit(0);
    }
    int rv;
    rv = zcs_init(ZCS_APP_TYPE, argv[1]);
    char* names[10];
    rv = zcs_query("type", "light", names, 10);
    if (rv > 0) {
        zcs_attribute_t attrs[5];
        int anum = 5;
        rv = zcs_get_attribs(names[0], attrs, &anum);
        if ((strcmp(attrs[1].attr_name, "location") == 0) &&
            (strcmp(attrs[1].value, "bedroom") == 0)) {
            rv = zcs_listen_ad(names[0], hello);
        }
    }
    sleep(200);
    zcs_shutdown();
<<<<<<< HEAD
=======

    return 0;
>>>>>>> 0c224f3c8f95fa8ada68e676d5deb12b7d93f8bd
}