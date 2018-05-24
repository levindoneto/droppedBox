#include "../headers/parUtils.hpp"
#include "../headers/dropboxUtils.h"

Thread::~Thread() {};

void Thread::start() {
    pthread_create(&id, INIT, &Thread::static_run, this);
}

void* Thread::static_run(void *void_this) {
    Thread* thread_this = static_cast<Thread*>(void_this);
    return thread_this->run();
}
