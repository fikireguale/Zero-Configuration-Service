#include "zcs.h"

int zcs_init(int type) {
    return 0;
}

int zcs_start(char *name, zcs_attribute_t attr[], int num) {
    return 0;
}

int zcs_post_ad(char *ad_name, char *ad_value) {
    return 0;
}

int zcs_query(char *attr_name, char *attr_value, char *node_names[], int namelen) {
    return 0;
}

int zcs_get_attribs(char *name, zcs_attribute_t attr[], int *num) {
    return 0;
}

int zcs_listen_ad(char *name, zcs_cb_f cback) {
    return 0;
}

int zcs_shutdown() {
    return 0;
}

void zcs_log() {

}