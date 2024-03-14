#include <pthread.h>
#include "multicast.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    mcast_t receiver;
    mcast_t sender;
} mcast_pair;