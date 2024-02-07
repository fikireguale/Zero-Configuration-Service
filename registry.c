#include "registry.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int registryLength = 0;
int subscriptionLength = 0;
registryEntry* head = NULL;
adEntry* headAd = NULL;

int getRegistryLength() {
	return registryLength;
}

int getAdSubscriptionLength() {
	return subscriptionLength;
}

void insertEntry(node* entry) {
	registryEntry* newEntry = malloc(sizeof(registryEntry));
	newEntry->next = NULL;
	newEntry->node = entry;
	newEntry->up = true;
	newEntry->timeOfServiceChange = time(NULL);

	if (registryLength == 0) {
		head = newEntry;
	} else {
		registryEntry* next = head;

		while (next->next != NULL) {
			next = next->next;
		}

		next->next = newEntry;
	}
	
	registryLength++;
}

void insertAd(char* serviceName, zcs_cb_f cback) {
	adEntry* newEntry = malloc(sizeof(adEntry));
	newEntry->next = NULL;
	newEntry->serviceName = serviceName;
	newEntry->cback = cback;

	if (subscriptionLength == 0) {
		headAd = newEntry;
	}
	else {
		adEntry* next = headAd;

		while (next->next != NULL) {
			next = next->next;
		}

		next->next = newEntry;
	}

	subscriptionLength++;
}

registryEntry* getEntryFromIndex(int index) {
	if (registryLength == 0 || registryLength <= index || index < 0)
		return NULL;
	
	registryEntry* entry = head;
	for (int i = 0; i < index; i++)
		entry = entry->next;

	return entry;
}

adEntry* getAdFromIndex(int index) {
	if (subscriptionLength == 0 || subscriptionLength <= index || index < 0)
		return NULL;

	adEntry* entry = headAd;
	for (int i = 0; i < index; i++)
		entry = entry->next;

	return entry;
}

registryEntry* getEntryFromName(char* name) {
	if (registryLength == 0)
		return NULL;

	registryEntry* entry = head;
	for (int i = 0; i < registryLength; i++) {
		if (strcmp(name, entry->node->name) == 0)
			return entry;
		entry = entry->next;
	}

	return NULL;
}

adEntry* getAdFromService(char* serviceName) {
	if (subscriptionLength == 0)
		return NULL;

	adEntry* entry = headAd;
	for (int i = 0; i < registryLength; i++) {
		if (strcmp(serviceName, entry->serviceName) == 0)
			return entry;
		entry = entry->next;
	}

	return NULL;
}

int setStatusFromIndex(int index, bool status) {
	if (registryLength == 0 || registryLength <= index || index < 0)
		return -1;

	registryEntry* entry = head;
	for (int i = 0; i < index; i++)
		entry = entry->next;

	entry->up = status;
	entry->timeOfServiceChange = time(NULL);
	return 0;
}

int setStatusFromName(char* name, bool status) {
	if (registryLength == 0)
		return -1;

	registryEntry* entry = head;
	for (int i = 0; i < registryLength; i++) {
		if (strcmp(name, entry->node->name) == 0) {
			entry->up = status;
			entry->timeOfServiceChange = time(NULL);
			return 0;
		}
		entry = entry->next;
	}

	return -1;
}

// i am pretty sure i am not freeing all the memory

registryEntry* removeEntryFromIndex(int index) {
	if (registryLength == 0 || registryLength <= index || index < 0)
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
	registryLength--;
	return entry;
}

registryEntry* removeEntryFromName(char* name) {
	if (registryLength == 0)
		return NULL;

	registryEntry* entry = head;
	registryEntry* prevEntry = NULL;
	for (int i = 0; i < registryLength; i++) {
		if (strcmp(name, entry->node->name) == 0) {
			if (prevEntry == NULL)
				head = entry->next;
			 else 
				prevEntry->next = entry->next;
			
			free(entry);
			registryLength--;
			return entry;
		}

		prevEntry = entry;
		entry = entry->next;
	}

	return NULL;
}