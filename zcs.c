#include "zcs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int type = 0;
node *thisService;

int zcs_init(int type) {
    //send DISCOVERY message (makes other nodes send a NOTIFICATION message, i.e. share their existence)

    return 0;
}

int zcs_start(char *name, zcs_attribute_t attr[], int num) {
    //check for proper initialization of library
    if (type == 0)
        return -1;
    
    thisService = malloc(sizeof(*thisService) + sizeof(zcs_attribute_t) * num);

    thisService->name = name;
    thisService->numOfAttr = num;

    for (int i = 0; i < num; i++) {
        thisService->attr[i] = attr[i];
    }

    //put node into local library log if not already there, then send NOTIFICATION message to tell other nodes about existence/being UP
    //start HEARTBEAT

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
    //check that service was already registered and that it is currently UP. if not fail

    //mark service as DOWN in local registry

    //stop HEARTBEAT
    return 0;
}

void zcs_log() {

}