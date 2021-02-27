/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_TERMINALOUTPUTSTREAM_H
#define HHUOS_TERMINALOUTPUTSTREAM_H

#include <util/graphic/Terminal.h>
#include "OutputStream.h"

namespace Util::Stream  {

class TerminalOutputStream : public OutputStream {

public:

    explicit TerminalOutputStream(Graphic::Terminal &terminal);

    TerminalOutputStream(const TerminalOutputStream &copy) = delete;

    TerminalOutputStream &operator=(const TerminalOutputStream &copy) = delete;

    ~TerminalOutputStream() override = default;

    void write(uint8_t c) override;

private:

    Graphic::Terminal &terminal;
};

}

#endif
