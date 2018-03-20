#ifndef __InterruptHandler_include__
#define __InterruptHandler_include__

/**
 * Interface for an interrupt handler.
 * Every interrupt hanlder should dervive from this interface.
 * The trigger-method is called if an interrupt occured.
 *
 * @author Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */
class InterruptHandler {
    
public:

    InterruptHandler() = default;

    InterruptHandler(const InterruptHandler &copy) = delete;

    InterruptHandler &operator=(const InterruptHandler &copy) = delete;
       
    virtual ~InterruptHandler() = default;

    /**
     * Routine to handle an interrupt. Needs to be implemented in deriving class.
     */
    virtual void trigger() = 0;
};

#endif
