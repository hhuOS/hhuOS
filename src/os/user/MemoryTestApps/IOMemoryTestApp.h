/** A simple test app for the IO memory manager 
 * 
 * Burak Akguel, Christian Gesse, HHU 2017 **/

#ifndef __IOMemTest_include__
#define __IOMemTest_include__


#include "kernel/threads/Thread.h"


class IOMemoryTestApp : public Thread {
    
private:
    IOMemoryTestApp (const IOMemoryTestApp &copy); // Verhindere Kopieren
    
   
public:
    // Gib dem Anwendungsthread einen Stack.
    IOMemoryTestApp (unsigned int* stack) : Thread ("IOMemoryTestApp") { }

    // Thread-Startmethode
    void run ();

 };

#endif
