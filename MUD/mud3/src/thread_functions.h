#ifndef THREAD_FUNCTIONS_H
#define THREAD_FUNCTIONS_H

#include <pthread.h>
#include <semaphore.h>

#include "mudconfig.h"
#include "player.h"

// main program threads
void thread_driver_func();
void *thread_process_func(void *arg);
void *thread_saveRooms_func(void *arg);

// this is for determining how long to sleep
unsigned long napTime(struct timeval *current, struct timeval *last);

// this is for checking on heartbeat functions
bool heartbeatCheck(struct timeval *current, struct timeval *lastHeartbeat);
void heartbeat();

void handleLogin(Player::PlayerPointer player, const std::string &command);

#endif // THREAD_FUNCTIONS_H
