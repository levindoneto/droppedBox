#ifndef PARUTILS_H
#define PARUTILS_H

#include "dropboxUtils.h"

class Thread {
private:
    pthread_t id;
public:
    virtual ~Thread() = INIT;
    virtual void* run() = INIT;
    void start();
    static void* static_run(void *void_this);
};

#endif
