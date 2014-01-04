#include <Arduino.h>

Thread::Thread() {
    thread = 0;
}

void Thread::execute(void (*func)()) {
    function = func;

    if (thread != 0) {
        return;
    }

    doit = true;

    pthread_create(&thread, NULL, &Thread::run, this);
    pthread_detach(thread);
}

void Thread::signal(uint32_t sig) {
    if (thread == 0) {
        return;
    }
    pthread_kill(thread, sig);
}

boolean Thread::running() {
    if (thread == 0) {
        return false;
    }
    if (pthread_kill(thread, 0) != 0) {
        thread = 0;
        return false;
    }
    return true;
}

void Thread::kill() {
    doit = false;
}

void *Thread::run(void *context) {
    Thread *t = (Thread *)context;
    while (t->doit) {
        t->function();
    }
}
