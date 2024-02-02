#include "registry.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int length = 0;
registryEntry* head = NULL;

int getRegistryLength() {
	return length;
}

void insertEntry(node* entry) {
	registryEntry* newEntry = malloc(sizeof(registryEntry));
	newEntry->next = NULL;
	newEntry->node = entry;
	newEntry->up = true;

	if (length == 0) {
		head = newEntry;
	} else {
		registryEntry* next = head;

		while (next->next != NULL) {
			next = next->next;
		}

		next->next = newEntry;
	}
	
	length++;
}

registryEntry* getEntryFromIndex(int index) {
	if (length == 0 || length <= index || index < 0)
		return NULL;
	
	registryEntry* entry = head;
	for (int i = 0; i < index; i++)
		entry = entry->next;

	return entry;
}

registryEntry* getEntryFromName(char* name) {
	if (length == 0)
		return NULL;

	registryEntry* entry = head;
	for (int i = 0; i < length; i++) {
		if (strcmp(name, entry->node->name) == 0)
			return entry;
		entry = entry->next;
	}

	return NULL;
}

int setStatusFromIndex(int index, bool status) {
	if (length == 0 || length <= index || index < 0)
		return -1;

	registryEntry* entry = head;
	for (int i = 0; i < index; i++)
		entry = entry->next;

	entry->up = status;
	return 0;
}

int setStatusFromName(char* name, bool status) {
	if (length == 0)
		return -1;

	registryEntry* entry = head;
	for (int i = 0; i < length; i++) {
		if (strcmp(name, entry->node->name) == 0) {
			entry->up = status;
			return 0;
		}
		entry = entry->next;
	}

	return -1;
}

registryEntry* removeEntryFromIndex(int index) {
	if (length == 0 || length <= index || index < 0)
		return NULL;
	
	registryEntry* entry = head;
	registryEntry* prevEntry = NULL;
	for (int i = 0; i < index; i++) {
		prevEntry = entry;
		entry = entry->next;
	}

	if (prevEntry == NULL)
		head = entry->next;
	else 
		prevEntry->next = entry->next;
	
	free(entry);
	length--;
	return entry;
}

registryEntry* removeEntryFromName(char* name) {
	if (length == 0)
		return NULL;

	registryEntry* entry = head;
	registryEntry* prevEntry = NULL;
	for (int i = 0; i < length; i++) {
		if (strcmp(name, entry->node->name) == 0) {
			if (prevEntry == NULL)
				head = entry->next;
			 else 
				prevEntry->next = entry->next;
			
			free(entry);
			length--;
			return entry;
		}

		prevEntry = entry;
		entry = entry->next;
	}

	return NULL;
}