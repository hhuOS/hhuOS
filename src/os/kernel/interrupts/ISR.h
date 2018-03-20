/*
* Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
* Heinrich-Heine University
*
* This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
* later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef __ISR_include__
#define __ISR_include__

/**
 * ISR - Interface for an interrupt service routine.
 * Every interrupt hanlder should dervive from this interface.
 * The trigger-method is called if an interrupt occured.
 *
 * @author Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */
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
