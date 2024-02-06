#include "zcs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "registry.h"
#include "multicast.h"

int userType = 0;
bool up = false;
node *thisService;
const int BUFFERSIZE = 1000;
char buffer[BUFFERSIZE];
int readPt = 0;
int writePt = 0;
pthread_mutex_t buffer_mutex;
// linked list for specific nodes for zcs_listen_ad 
int MAX_RETRIES = 3;
int zcs_init_is_done = 0;
int zcs_shutdown_ongoing = 0;

void* write_buffer(void* arg, mcast_t *mcastNodeToApp) {
    // Continously listening and writing messages to the buffer (except when read_buffer thread holds lock..)
    int num_bytes_received = 0;
    char temp_buffer[100];

    while (1) {
        if (zcs_shutdown_ongoing == 1) {
            // shutdown procedure
            break;
        }
        if (multicast_check_receive(mcastNodeToApp)) {
            num_bytes_received = multicast_receive(mcastNodeToApp, temp_buffer, sizeof(temp_buffer));
            if (num_bytes_received > 0) {
                pthread_mutex_lock(&buffer_mutex);
                // Add logic for: while buffer is full wait
                if (writePt + num_bytes_received < sizeof(buffer)) {
                    // Write to buffer
                    memcpy(buffer + writePt, temp_buffer, num_bytes_received);
                    // Clear buffer
                    memset(buffer, 0, sizeof(buffer));
                    writePt = 0;
                    //writePt += num_bytes_received % BUFFERSIZE; 
                }
                pthread_mutex_unlock(&buffer_mutex);
            }
        }
    }
    // Add logic for: pthread_exit
    return NULL;
}

void* read_buffer(void* arg, mcast_t *mcastAppToNode, mcast_t *mcastNodeToApp) {
    // when reading the buffer, it's possible to misss a message..

    while (1) {
        if (zcs_shutdown_ongoing == 1) {
            // shutdown procedure
            break;
        }

        pthread_mutex_lock(&buffer_mutex);
        // Add logic for: while buffer is empty wait
        //   wait until something to read
        // Add conditional statement to check buffer index
        // Read/Pull data out of buffer
        pthread_mutex_unlock(&buffer_mutex);
        
    }

    return NULL;
}


int zcs_init(int type) {
    userType = type;

    // Start network
    int rport = atoi(argv[1]); // 5000
    int sport = rport + rport + random() % rport; // 6000
    // Channel used to send instructions to nodes via sport
    mcast_t *mcastAppToNode = multicast_init("224.1.1.1", sport, 0);
    // Channel used to receive responses from nodes via rport
    mcast_t *mcastNodeToApp = multicast_init("224.1.1.1", 0, rport);

    if (mcastAppToNode == NULL || mcastNodeToApp == NULL) {
        return -1;
    }

    pthread_t listen_thread;
    pthread_mutex_init(&buffer_mutex, NULL);

    // Start listening thread where buffer gets written
    if (pthread_create(&listen_thread, NULL, write_buffer, NULL) != 0) {
        perror("Failed to create listening thread");
        return -1;
    }
    // App broadcasts DISCOVERY and listens for NOTIFICATIONs to update its local_registry
    if (userType == ZCS_APP_TYPE) {
        // Send 1 DISCOVERY msg
        char msg[] = "zcs|01#";
        multicast_send(mcastAppToNode, msg, strlen(msg));
        // wait 1 sec
    }
    // Read messages using read_buffer() above
        // + handle all msg types
                // for app:
                // printf("Buffer contents: %s\n", buffer);
                // update local registry
                // node *node_list = decode_buffer(buffer);
                // for node in node_list
                //  insertEntry(node* entry);
    zcs_init_is_done = 1;
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