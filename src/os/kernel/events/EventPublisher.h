#ifndef __EventPublisher_include__
#define __EventPublisher_include__

#include <lib/util/BlockingQueue.h>
#include "kernel/threads/Thread.h"
#include "kernel/events/Event.h"
#include "kernel/events/Receiver.h"
#include "kernel/Spinlock.h"

/**
 * @author Filip Krakowski
 */
class EventPublisher : public Thread {

public:

    explicit EventPublisher(Receiver &receiver);

    EventPublisher(const EventPublisher &other) = delete;

    EventPublisher &operator=(const EventPublisher &other) = delete;

    ~EventPublisher();

    void run() override;

    void add(const Event &event);

    void stop();

private:

    Util::BlockingQueue<const Event*> eventQueue;

    Receiver &receiver;

    Spinlock lock;

    bool isRunning = true;

    void notify();
};


#endif
