#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "zcs.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: service4 <LAN IP ADDR> \n");
        exit(0);
    }
    int rv;
    rv = zcs_init(ZCS_SERVICE_TYPE, argv[1]);
    zcs_attribute_t attribs[] = {
        {.attr_name = "type", .value = "toaster"},
        {.attr_name = "location", .value = "kitchen"},
        {.attr_name = "make", .value = "hamilton"} };
    rv = zcs_start("zcsToaster", attribs, sizeof(attribs) / sizeof(zcs_attribute_t));
    for (int i = 0; i < 1000; i++) {
        rv = zcs_post_ad("toast", "on");
        sleep(10);
        rv = zcs_post_ad("toast", "off");
        sleep(10);
    }
    rv = zcs_shutdown();
}
