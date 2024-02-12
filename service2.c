#include <unistd.h>
#include "zcs.h"

int main() {
    int rv;
    rv = zcs_init(ZCS_SERVICE_TYPE);
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
    sleep(200);
    rv = zcs_shutdown();
}

main();
