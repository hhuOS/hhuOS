/**
 * PIC - programmable interrupt controller. In this hardware device the different
 * interrupts can be activated or masked out. Using the PIC one can controll,
 * which hardware interrupts shall be passed to the CPU.
 *
 * @author  original by Olaf Spinczyk, TU Dortmund
 * 			modified by Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */

#ifndef __PIC_include__
#define __PIC_include__

#define PIC_EOI 0x20

class Pic {

private:
    Pic(const Pic &copy);
    Pic() = default;

    // Singleton instance
    static Pic *pic;

public:

    /**
     * Get or create the singleton instance
     */
    static Pic *getInstance() {
        if(pic == nullptr) {
            pic = new Pic();
        }
        return pic;
    }

    // numbers of important interrupt ports
    enum {
       timer    = 0,   	// Programmable Interrupt Timer (PIT)
       keyboard = 1,    // keyboard
        mouse = 12		// mouse
    };

    /**
     * Demasks an interrupt number in the corresponding PIC. If this is done,
     * all interrupts with this number will be passed to the CPU.
     *
     * @param interrupt_device The number of the interrupt that should be activated.
     */
    void allow (int interrupt_device);

    /**
     * Forbids an interrupt. If this is done, the interrupt is masked out
     * and every interrupts with this number that is thrown will be
     * surpressed and not arrive the CPU.
     *
     * @param interrupt_device The number of the interrupt that should be deactivated/masked out.
     */
    void forbid (int interrupt_device);

    /**
     * Gets the state of this interrupt - whether it is masked out or not.
     *
     * @param interrupt_device The number of the interrupt we want to get the state.
     * @return Status of the bit for the interrupt device
     */
    bool status (int interrupt_device);

    /**
     * Send an end of interrupt signal to the corresponding PIC.
     *
     * @param interrupt_device The number of the interrupt for which we want to send an EOI.
     */
    void sendEOI (unsigned int interrupt_device);
 };

#endif
