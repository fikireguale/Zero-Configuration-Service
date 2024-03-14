//ip A: 224.1.1.1
//ip B: 224.1.2.1
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
    mcast_pair* args = (mcast_pair*)arg;
    char message[1000];
    while (true) {
        int num_bytes_received = multicast_receive(&args->receiver, message, sizeof(message));
        if (message[0] == '%') {
            memset(message, '\0', strlen(message));
            continue;
        } else
            message[0] = '%';
        printf("%s\n", message);
        multicast_send(&args->sender, message, strlen(message));
        memset(message, '\0', strlen(message));
        //sleep(10);
    }
}

int main() {
    //create threads and casts

    serviceAmcast = multicast_init(IPA, SPORT, RPORT);
    serviceBmcast = multicast_init(IPB, SPORT, RPORT);
    appBmcast = multicast_init(IPB, RPORT, SPORT);
    appAmcast = multicast_init(IPA, RPORT, SPORT);

    multicast_setup_recv(serviceAmcast);
    multicast_setup_recv(serviceBmcast);
    multicast_setup_recv(appAmcast);
    multicast_setup_recv(appBmcast);

    mcast_pair* args1 = malloc(sizeof(mcast_pair));
    args1->receiver = *serviceAmcast;
    args1->sender = *appBmcast;

    if (pthread_create(&appAServiceBThread, NULL, listener, args1) != 0) {
        perror("Relay failed to create a listening thread");
        return -1;
    }

    mcast_pair* args2 = malloc(sizeof(mcast_pair));
    args2->receiver = *serviceBmcast;
    args2->sender = *appAmcast;

    if (pthread_create(&appBServiceAThread, NULL, listener, args2) != 0) {
        perror("Relay failed to create a listening thread");
        return -1;
    }

    mcast_pair* args3 = malloc(sizeof(mcast_pair));
    args3->sender = *serviceBmcast;
    args3->receiver = *appAmcast;

    if (pthread_create(&serviceAAppBThread, NULL, listener, args3) != 0) {
        perror("Relay failed to create a listening thread");
        return -1;
    }

    mcast_pair* args4 = malloc(sizeof(mcast_pair));
    args4->sender = *serviceAmcast;
    args4->receiver = *appBmcast;

    if (pthread_create(&serviceBAppAThread, NULL, listener, args4) != 0) {
        perror("Relay failed to create a listening thread");
        return -1;
    }

    //send discovery to all services on both ips
    multicast_send(appAmcast, "%01#", 4);
    multicast_send(appBmcast, "%01#", 4);

    while (true) { sleep(100); }
}