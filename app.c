# include "app.h"

//ideally we would have a single app.c, and could load the data through a file. for now, multiple app.c will work for demo purposes

void hello(char* s, char* r) {
    printf("Ad received: %s, with value: %s\n", s, r);
    zcs_log();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Incorrect arguments. Correct usage: app <LAN IP ADDR> \n");
        return 1;
    }

    int rv = zcs_init(ZCS_APP_TYPE, argv[1]);

    char* names[10];
    while (true) { //loop until relevant service comes online the first time, that way we can listen to the add
        rv = zcs_query("type", "toaster", names, 10);
        if (rv > 0) { 
            zcs_attribute_t attrs[5];
            int anum = 5;
            rv = zcs_get_attribs(names[0], attrs, &anum);
            if ((strcmp(attrs[1].attr_name, "location") == 0) &&
                (strcmp(attrs[1].value, "kitchen") == 0)) {
                rv = zcs_listen_ad(names[0], hello);
                break;
            }
        }
        sleep(10);
    }
    sleep(20);
    zcs_shutdown();
}