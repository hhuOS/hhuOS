/**
 * ISR - Interface for an interrupt service routine.
 * Every interrupt hanlder should dervive from this interface.
 * The trigger-method is called if an interrupt occured.
 *
 * @author Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */
#ifndef __ISR_include__
#define __ISR_include__

class ISR {
    
private:
    ISR (const ISR &copy);

public:
    ISR () {}
       
    virtual ~ISR () {}

    // Routine to handle an interrupt - needs to be implemented in deriving class
    virtual void trigger () = 0;
};

#endif
