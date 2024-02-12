#include "zcs.h"
#include <stdbool.h>
#include <time.h>
typedef struct registryEntry registryEntry;
typedef struct adEntry adEntry;
typedef struct serviceEvent serviceEvent;

struct registryEntry {
    node* node;
    bool up;
    int totalEvents;
    time_t timeOfLastHeartbeat;
    registryEntry* next;
    serviceEvent* startEvent;
};

struct adEntry {
    char* serviceName;
    zcs_cb_f cback;
    adEntry* next;
};

struct serviceEvent {
    time_t timestamp;
    bool status;
    serviceEvent* next;
};

void setServiceTO(float time);

int getRegistryLength();

int getAdSubscriptionLength();

void insertEntry(node *entry);

void insertAd(char* serviceName, zcs_cb_f cback);

registryEntry* getEntryFromIndex(int index);

adEntry* getAdFromIndex(int index);

registryEntry* getEntryFromName(char *name);

adEntry* getAdFromService(char* serviceName);

registryEntry* removeEntryFromIndex(int index);

registryEntry* removeEntryFromName(char* name);

int setStatusFromName(char* name, bool status);