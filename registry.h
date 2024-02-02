#include "zcs.h"
typedef struct registryEntry registryEntry;

struct registryEntry {
    node* node;
    registryEntry* next;
};

int getRegistryLength();

void insertEntry(node *entry);

registryEntry* getEntryFromIndex(int index);

registryEntry* getEntryFromName(char *name);

registryEntry* removeEntryFromIndex(int index);

registryEntry* removeEntryFromName(char* name);