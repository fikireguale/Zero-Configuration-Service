#include "zcs.h"
#include <stdbool.h>
#include <time.h>
typedef struct registryEntry registryEntry;
typedef struct adEntry adEntry;

struct registryEntry {
    node* node;
    bool up;
    time_t timeOfLastHeartbeat;
    registryEntry* next;
};

struct adEntry {
    char* serviceName;
    zcs_cb_f cback;
    adEntry* next;
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

int setStatusFromIndex(int index, bool status);