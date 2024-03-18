#include "service.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Incorrect arguments. Correct usage: service <LAN IP ADDR> \n");
        return 1;
    }

    int rv = zcs_init(ZCS_SERVICE_TYPE, argv[1]);

    zcs_attribute_t attribs[] = {
        {.attr_name = "type", .value = "light"},
        {.attr_name = "location", .value = "bedroom"},
        {.attr_name = "make", .value = "ikea"} };

    rv = zcs_start("zcsLight", attribs, sizeof(attribs) / sizeof(zcs_attribute_t));

    for (int i = 0; i < 1000; i++) {
        rv = zcs_post_ad("power", "on");
        sleep(10);
        rv = zcs_post_ad("power", "off");
        sleep(10);
    }

    rv = zcs_shutdown();
}
