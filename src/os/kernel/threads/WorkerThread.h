#ifndef __WorkerThread_include__
#define __WorkerThread_include__

#include "Thread.h"

/**
 * @author Filip Krakowski
 */
template <typename T, typename R>
class WorkerThread : public Thread {

public:

    WorkerThread(R (*work)(const T&), const T &data, void (*callback)(const Thread&, const R&));

    void run() override;

private:

    R (*workFunction)(const T&);

    void (*onFinished)(const Thread&, const R&);

    T data;
};

template <class T, class R>
WorkerThread<T, R>::WorkerThread(R (*work)(const T&), const T &data, void (*callback)(const Thread&, const R&)) {

    this->workFunction = work;

    this->onFinished = callback;

    this->data = data;
}

template <class T, class R>
void WorkerThread<T, R>::run() {

    R result = workFunction(data);

    onFinished(*this, result);
}

#endif
