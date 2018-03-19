#ifndef __WorkerThread_include__
#define __WorkerThread_include__

#include "Thread.h"

/**
 * @author Filip Krakowski
 */
template <typename T>
class WorkerThread : public Thread {

public:

    WorkerThread(void (*workFunction)(T *), T *data);

    void run() override;

private:

    void (*workFunction)(T *);
    void *data;

};

template <class T>
WorkerThread<T>::WorkerThread(void (*workFunction)(T *), T *data) {
    this->workFunction = workFunction;
    this->data = data;
}

template <class T>
void WorkerThread<T>::run() {
    workFunction(data);
}

#endif
