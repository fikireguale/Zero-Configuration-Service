#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "zcs.h"

void hello(char* s, char* r) {
    printf("Ad received: %s, with value: %s\n", s, r);
    zcs_log();
}

int main() {
    bool listening = false;
    int rv;
    rv = zcs_init(ZCS_APP_TYPE);
    char* names[10];
    while (true) {
        rv = zcs_query("type", "speaker", names, 10);
        if (rv > 0) {
            zcs_attribute_t attrs[5];
            int anum = 5;
            rv = zcs_get_attribs(names[0], attrs, &anum);
            if (!listening && (strcmp(attrs[1].attr_name, "location") == 0) &&
                (strcmp(attrs[1].value, "kitchen") == 0)) {
                listening = true;
                rv = zcs_listen_ad(names[0], hello);
            }
        }
    }
}


