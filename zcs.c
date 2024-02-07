#include "zcs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "registry.h"
#include "multicast.h"
#include <unistd.h>

#define BUFFERSIZE 1000
#define RPORT 5000
#define SPORT 6000
#define IP "224.1.1.1"
#define HEARTBEATTO 5.0
#define HEARTBEATPAUSE 2

int userType = 0;
bool up = false;

node* thisService;

char buffer[BUFFERSIZE];
// threads
int readPt = 0;
int writePt = 0;
const int DATA_SIZE = 100;
pthread_cond_t full;
pthread_cond_t empty;
pthread_mutex_t buffer_mutex;
pthread_t listen_thread, respond_thread, heartbeat_thread;
// linked list for specific nodes for zcs_listen_ad 
int MAX_RETRIES = 3;
int zcs_init_is_done = 0;
int zcs_shutdown_ongoing = 0;

mcast_t* mcast;

void* heartbeat(void* arg) {
    char message[1000];
    strcat(message, "$10|");
    strcat(message, thisService->name);
    strcat(message, "#");
    while (up) {
        multicast_send(mcast, message, strlen(message));
        sleep(HEARTBEATPAUSE);
    }
}

void generateNotification() {
    char message[1000];
    strcat(message, "$00|");
    strcat(message, thisService->name);
    strcat(message, "|");
    char snum[5];
    sprintf(snum, "%d", thisService->numOfAttr);
    strcat(message, snum);
    strcat(message, "|");

    for (int i = 0; i < thisService->numOfAttr; i++) {
        strcat(message, thisService->attr[i].attr_name);
        strcat(message, ";");
        strcat(message, thisService->attr[i].value);
        strcat(message, ";");
    }

    strcat(message, "#");

    multicast_send(mcast, message, strlen(message));
}

void processData(char* read_data) {
    printf("processing data... %s\n", read_data);
    char* start = read_data;
    char* end;
    // strchr(): returns a pointer to '$' if it is found in start ie read_data
    while ((start = strchr(start, '$')) != NULL) {
        end = strchr(start, '#');
        if (end == NULL) {
            break; // No end marker found, stop processing
        }

        *end = '\0'; // Replace '#' to isolate the msg
        char* msg = start + 1; // Start of actual msg (skip '$')

        // Parse msg
        if (strncmp(msg, "00", 2) == 0) {
            msg += 3;
            // NOTIFICATION
            // msg = "$msgType|nodeName|numOfAttr|status|attrName1=value1;attrName2=value2;...#"
            // --> output a list of node structs
            char* nodeName = strtok(msg, "|");
            char* numOfAttrStr = strtok(NULL, "|");
            int numOfAttr = atoi(numOfAttrStr);

            zcs_attribute_t attr[numOfAttr];

            node* n = malloc(sizeof(node) + sizeof(zcs_attribute_t) * numOfAttr);;
            n->name = strdup(nodeName);
            n->numOfAttr = numOfAttr;

            for (int i = 0; i < numOfAttr; i++) {
                n->attr[i].attr_name = strdup(strtok(NULL, ";"));
                n->attr[i].value = strdup(strtok(NULL, ";"));
            }

            if (getEntryFromName(n->name) != NULL) //if service wasnt previously registered, register it
                insertEntry(n);
            else //if it was, change its status to being UP
                setStatusFromName(n->name, true);

            printf("NOTIFICATION from %s, Attributes: %d\n", nodeName, numOfAttr);
            // parse attributes

        }
        else if (strncmp(msg, "01", 2) == 0) {
            // DISCOVERY
            // msg = "$01#"
            // --> as a node, send a notif
            printf("DISCOVERY message received\n");
            generateNotification();
        }
        else if (strncmp(msg, "10", 2) == 0) {
            // HEARTBEAT
            // msg = "$10|nodeName#"
            char* nodeName = strtok(msg + 3, "|"); // +3 to skip "10|"
            printf("HEARTBEAT from %s\n", nodeName);
            setStatusFromName(nodeName, true);
            // --> heartbeat logic

        }
        else if (strncmp(msg, "11", 2) == 0) {
            // ADVERTISEMENT
            // msg = "$msgType|nodeName|ad_name;ad_value#"
            char* nodeName = strtok(msg + 3, "|");
            char* ad_content = strtok(NULL, "|");
            printf("ADVERTISEMENT from %s, Content: %s\n", nodeName, ad_content);
            // -> execute zcs_listen_ad if headAd pointer isn't null

        }
        else {
            // Process other message types
            printf("UNKNOWN message received\n");
        }
        start = end + 1; // Go to next msg
    }
}

void* write_buffer(void* arg) {
    // Continously listening and writing messages to the buffer (except when read_buffer thread holds lock..)
    int num_bytes_received = 0;
    char temp_buffer[100];

    while (1) {
        if (zcs_shutdown_ongoing == 1) {
            // shutdown procedure
            break;
        }
        // While buffer is full, wait
        //  since next position to write to is the current position to read from
        while (((writePt + 1) % BUFFERSIZE) == readPt) {
            pthread_cond_wait(&empty, &buffer_mutex);
        }

        if (multicast_check_receive(mcast)) {
            num_bytes_received = multicast_receive(mcast, temp_buffer, sizeof(temp_buffer));
            if (num_bytes_received > 0) {
                pthread_mutex_lock(&buffer_mutex);
                if (writePt + num_bytes_received < sizeof(buffer)) {
                    // Write to buffer
                    // void *memcpy(void *dest, const void *src, size_t n);
                    // 'num_bytes_received + 1' to include the null terminator?
                    memcpy(buffer + writePt, temp_buffer, num_bytes_received);
                    writePt += num_bytes_received % BUFFERSIZE;
                }
                pthread_mutex_unlock(&buffer_mutex);
            }
        }
        pthread_cond_signal(&full);
    }
    // pthread_exit(NULL);
    return NULL;
}

void* read_buffer(void* arg) {
    // when reading the buffer, it's possible to miss a message if buffer is held here..
    // process 

    while (1) {
        if (zcs_shutdown_ongoing == 1) {
            // shutdown procedure
            break;
        }

        pthread_mutex_lock(&buffer_mutex);
        // While buffer is empty wait
        while (writePt == readPt) {
            pthread_cond_wait(&full, &buffer_mutex);
        }

        int num_bytes_to_read = writePt;
        if (num_bytes_to_read > 0) {
            char read_data[num_bytes_to_read];
            // void *memcpy(void *dest, const void *src, size_t n);
            memcpy(read_data, buffer, num_bytes_to_read);

            // Process the data in read_data as needed (hopefully doesn't take too long..)
                // Extract msg string from buffer
                // Based on msg type redirect to different methods to handle
            processData(read_data);

            // Clear the buffer and reset writePt
            memset(buffer, 0, BUFFERSIZE);
            memset(read_data, 0, num_bytes_to_read);
            writePt = 0;
        }

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&buffer_mutex);
    }
    // pthread_exit(NULL);
    return NULL;
}

int zcs_init(int type) {
    userType = type;
    setServiceTO(HEARTBEATTO);

    // Start network
    // Channel used to send instructions to nodes via sport
    if (userType == ZCS_SERVICE_TYPE) {
        mcast = multicast_init(IP, SPORT, RPORT);
        multicast_setup_recv(mcast);
    } else {
        mcast = multicast_init(IP, RPORT, SPORT);
        multicast_setup_recv(mcast);
    }

    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_mutex_init(&buffer_mutex, NULL);

    // Start listening thread where buffer gets written
    if (pthread_create(&listen_thread, NULL, write_buffer, NULL) != 0) {
        perror("Failed to create listening thread");
        return -1;
    }

    // Start response thread where buffer gets read
    if (pthread_create(&respond_thread, NULL, read_buffer, NULL) != 0) {
        perror("Failed to create response thread");
        return -1;
    }

    // App broadcasts DISCOVERY (and listens for NOTIFICATIONs to update its local_registry)
    if (userType == ZCS_APP_TYPE) {
        // Send 1 DISCOVERY msg
        char* msg = "$01#";
        multicast_send(mcast, msg, strlen(msg));
        sleep(1);
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

int zcs_start(char* name, zcs_attribute_t attr[], int num) {
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

    //send NOTIFICATION message to tell about about existence/status of this service
    generateNotification();

    //start HEARTBEAT
    if (pthread_create(&heartbeat_thread, NULL, heartbeat, NULL) != 0) {
        perror("Failed to create heartbeat thread");
        return -1;
    }

    return 0;
}

int zcs_post_ad(char* ad_name, char* ad_value) {
    return 0;
}

int zcs_query(char* attr_name, char* attr_value, char* node_names[], int namelen) {
    // iterate over local registry
    // for each node, check to see if it has an attribute with a specific value (until registry is exhausted or name array full, i.e. namelen entries)
    // if yes, add to name array
    // return number of valid nodes found

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

int zcs_get_attribs(char* name, zcs_attribute_t attr[], int* num) {
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

int zcs_listen_ad(char* name, zcs_cb_f cback) {
    return 0;
}

int zcs_shutdown() {
    //check that service was already registered and that it is currently UP. if not fail
    if (userType == 0)
        return -1;

    // apps will mark a service as down due to lack of heartbeat
    // if service comes back, will start heartbeating again. apps will see the service name in their registry and just mark it as up
    if (userType == ZCS_SERVICE_TYPE)
        free(thisService);
    up = false; //stops HEARTBEAT, if this is a service

    return 0;
}

void zcs_log() {
    printf("-------- LOG START --------\n");

    for (int i = 0; i < getRegistryLength(); i++) {
        registryEntry* entry = getEntryFromIndex(i);
        printf("Node name: %s\nSatus: %s\nLast Heartbeat: %f seconds ago\nAttributes:\n", entry->node->name, entry->up ? "UP" : "DOWN", difftime(time(NULL), entry->timeOfLastHeartbeat));
        for (int j = 0; j < entry->node->numOfAttr; j++) {
            printf("\t%s: %s\n", entry->node->attr[j].attr_name, entry->node->attr[j].value);
        }
        printf("\n");
    }

    printf("--------- LOG END ---------\n\n");
}