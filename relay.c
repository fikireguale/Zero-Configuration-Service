//ip A: 224.1.10.x
//ip B: 224.1.20.x
#include "relay.h"

#define RPORT 5000
#define SPORT 6000
#define IPA "224.1.1.1"
#define IPB "224.1.2.1"

pthread_t appAServiceBThread;
pthread_t appBServiceAThread;
pthread_t serviceBAppAThread;
pthread_t serviceAAppBThread;
mcast_t* appAmcast; 
mcast_t* appBmcast;
mcast_t* serviceBmcast;
mcast_t* serviceAmcast;

void* listener(void* arg) {
    mcast_pair* args = (mcast_pair *) arg;
    char message[1000];
    while (true) {
        int num_bytes_received = multicast_receive(&args->receiver, message, sizeof(message));
        multicast_send(&args->sender, message, strlen(message));
        sleep(100);
    }
}

int main(int argc, char* argv[]) {
    //create threads

    appAmcast = multicast_init(IPA, SPORT, RPORT);
    appBmcast = multicast_init(IPB, SPORT, RPORT);
    serviceBmcast = multicast_init(IPB, RPORT, SPORT);
    serviceAmcast = multicast_init(IPA, RPORT, SPORT);

    mcast_pair* args = malloc(sizeof(mcast_pair));
    args->receiver = *serviceBmcast;
    args->sender = *appAmcast;

    if (pthread_create(&appAServiceBThread, NULL, listener, args) != 0 && pthread_detach(appAServiceBThread) != 0) {
        perror("Relay failed to create a listening thread");
        return -1;
    }

    args->receiver = *serviceAmcast;
    args->sender = *appBmcast;

    if (pthread_create(&appBServiceAThread, NULL, listener, args) != 0 && pthread_detach(appBServiceAThread) != 0) {
        perror("Relay failed to create a listening thread");
        return -1;
    }

    args->sender = *serviceBmcast;
    args->receiver = *appAmcast;

    if (pthread_create(&serviceBAppAThread, NULL, listener, args) != 0 && pthread_detach(serviceBAppAThread) != 0) {
        perror("Relay failed to create a listening thread");
        return -1;
    }

    args->sender = *serviceAmcast;
    args->receiver = *appBmcast;

    if (pthread_create(&serviceAAppBThread, NULL, listener, args) != 0 && pthread_detach(serviceAAppBThread) != 0) {
        perror("Relay failed to create a listening thread");
        return -1;
    }
    
    while (true) { sleep(100); }
}