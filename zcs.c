#include "zcs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "registry.h"

int userType = 0;
bool up = false;
node *thisService;

int zcs_init(int type) {
    //send DISCOVERY message (makes other nodes send a NOTIFICATION message, i.e. share their existence)
    userType = type;

    return 0;
}

int zcs_start(char *name, zcs_attribute_t attr[], int num) {
    //check for proper initialization of library
    if (userType == 0)
        return -1;
    
    thisService = malloc(sizeof(*thisService) + sizeof(zcs_attribute_t) * num);

    thisService->name = name;
    thisService->numOfAttr = num;

    for (int i = 0; i < num; i++) {
        thisService->attr[i] = attr[i];
    }

    up = true;
    //test statements to check that node is properly created
    //printf("service started\n");
    //printf("%s\n", thisService->attr[num-1].attr_name);

    //put node into local library log, then send NOTIFICATION message to tell other nodes about existence/being UP
    insertEntry(thisService);
    //start HEARTBEAT

    zcs_log();

    return 0;
}

int zcs_post_ad(char *ad_name, char *ad_value) {
    return 0;
}

int zcs_query(char *attr_name, char *attr_value, char *node_names[], int namelen) {
    // iterate over local registry
    // for each node, check to see if it has an attribute with a specific value (until registry is exhausted or name array full, i.e. namelen entries)
    // if yes, add to name array
    // return number of valid nodes found

    //send discovery? perhaps send discovery, then listen for a set period of time for notifications (e.g. wait 5s after last notification) to ensure all the responses (e.g. wait for a listening thread to join)

    int count = 0;

    for (int i = 0; i < getRegistryLength() && count < namelen; i++) {
        registryEntry* entry = getEntryFromIndex(i);
        if (!entry->up) continue; //if node is down, doesnt really make sense to consider it
        for (int j = 0; j < entry->node->numOfAttr && count < namelen; j++) {
            if (strcmp(attr_name, entry->node->attr[j].attr_name) == 0 && strcmp(attr_value, entry->node->attr[j].value) == 0) {
                node_names[count] = entry->node->name;
                count++;
            }
        }
    }

    return count;
}

int zcs_get_attribs(char *name, zcs_attribute_t attr[], int *num) {
    // for the provided service name, set the (preallocated) array to the number of attributes
    // num originally size of the array. set num to attributes read. so you will always read <= num attributes
    // if num < node->numOfAttr, return num attributes. otherwise return node->numOfAttr attributes

    //get node from registry
    registryEntry* entry = getEntryFromName(name);

    if (entry == NULL)
        return -1; //error if no service with given name

    int i = 0;
    for (; i < entry->node->numOfAttr && i < *num; i++) {
        attr[i] = entry->node->attr[i];
    }

    *num = i;
    return 0;
}

int zcs_listen_ad(char *name, zcs_cb_f cback) {
    return 0;
}

int zcs_shutdown() {
    //check that service was already registered and that it is currently UP. if not fail
    if (userType == 0) 
        return -1;

    // if local registry is a linked list, can just remove the entry locally. other nodes will mark as down due to lack of heartbeat
    // if service comes back, add to end of local registry and start heartbeating again. other nodes will see the service name in their registry and just mark it as up
    removeEntryFromName(thisService->name);
    up = false;
    //stop HEARTBEAT
    return 0;
}

void zcs_log() {
    printf("-------- LOG START --------\n");

    for (int i = 0; i < getRegistryLength(); i++) {
        registryEntry* entry = getEntryFromIndex(i);
        printf("Node name: %s\nSatus: %s\nLast Status Change: %f seconds ago\nAttributes:\n", entry->node->name, entry->up ? "UP" : "DOWN", difftime(entry->timeOfServiceChange, time(NULL)));
        for (int j = 0; j < entry->node->numOfAttr; j++) {
            printf("\t%s: %s\n", entry->node->attr[j].attr_name, entry->node->attr[j].value);
        }
    }

    printf("--------- LOG END ---------\n");
}