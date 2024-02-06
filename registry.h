#include "zcs.h"
#include <stdbool.h>
typedef struct registryEntry registryEntry;
typedef struct adEntry adEntry;

struct registryEntry {
    node* node;
    bool up;
    registryEntry* next;
};

struct adEntry {
    char* serviceName;
    zcs_cb_f cback;
    adEntry* next;
};

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