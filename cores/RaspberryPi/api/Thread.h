#ifndef _THREAD_H
#define _THREAD_H

#include <Arduino.h>
#include <pthread.h>
#include <signal.h>

class Thread {
    private:
        pthread_t thread;
        volatile boolean doit;
        void (*function)();

    public:
        Thread();
        void execute(void (*func)());
        void signal(uint32_t sig);
        boolean running();
        void kill();
        static void *run(void *context);
};

#endif
