#include "registry.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

float serviceTO;
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

void setServiceTO(float time) {
	serviceTO = time;
}

void checkHeartbeat(registryEntry* entry, time_t currentTime) {
	if ((difftime(time(NULL), entry->timeOfLastHeartbeat) > serviceTO) && entry->up) {
		entry->up = false;
		serviceEvent* nextEvent = entry->startEvent;
		serviceEvent* newEvent = malloc(sizeof(serviceEvent));
		newEvent->status = false;
		newEvent->next = NULL;
		newEvent->timestamp = entry->timeOfLastHeartbeat + (int)serviceTO;
		
		for (int i = 0; i < entry->totalEvents - 1; i++) 
			nextEvent = nextEvent->next;

		nextEvent->next = newEvent;
		entry->totalEvents++;
	}
}

void insertEntry(node* entry) {
	registryEntry* newEntry = malloc(sizeof(registryEntry));
	newEntry->startEvent = malloc(sizeof(serviceEvent));
	newEntry->totalEvents = 1;
	newEntry->startEvent->status = true;
	newEntry->startEvent->next = NULL;
	newEntry->next = NULL;
	newEntry->node = entry;
	newEntry->up = true;
	newEntry->timeOfLastHeartbeat = time(NULL);
	newEntry->startEvent->timestamp = newEntry->timeOfLastHeartbeat;

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
	for (int i = 0; i < index; i++) {
		entry = entry->next;
	}

	checkHeartbeat(entry, time(NULL));
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
		if (strcmp(name, entry->node->name) == 0) {
			checkHeartbeat(entry, time(NULL));
			return entry;
		}
		entry = entry->next;
	}

	return NULL;
}

adEntry* getAdFromService(char* serviceName) {
	if (subscriptionLength == 0)
		return NULL;

	adEntry* entry = headAd;
	for (int i = 0; i < subscriptionLength && entry != NULL; i++) {
		if (strcmp(serviceName, entry->serviceName) == 0)
			return entry;
		entry = entry->next;
	}

	return NULL;
}

int setStatusFromName(char* name, bool status) {
	if (registryLength == 0)
		return -1;

	registryEntry* entry = head;
	for (int i = 0; i < registryLength; i++) {
		if (strcmp(name, entry->node->name) == 0) {
			entry->timeOfLastHeartbeat = time(NULL);

			if (entry->up != status) {
				entry->up = status;
				serviceEvent* nextEvent = entry->startEvent;
				serviceEvent* newEvent = malloc(sizeof(serviceEvent));
				newEvent->status = status;
				newEvent->next = NULL;
				newEvent->timestamp = entry->timeOfLastHeartbeat;

				for (int i = 0; i < entry->totalEvents - 1; i++)
					nextEvent = nextEvent->next;

				nextEvent->next = newEvent;
				entry->totalEvents++;
			}

			return 0;
		}
		entry = entry->next;
	}

	return -1;
}

// i am pretty sure i am not freeing all the memory correctly

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