/*
 * Copyright (C) 2020 Namit Shah, Martand Javia & Harvish Jariwala
 * Ahmedabad University
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

#ifndef __Mathexpr_include__
#define ___Mathexpr_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"
/**
 * Implementation of Command.
 * Calculates the mathematical expression 
 *
 * -h, --help: Show help message
 *
 * @author Namit Shah, Martand Javia, Harvish Jariwala
 * @date 2020
 */
class Mathexpr : public Command {

public:
    /**
     * Default-constructor.
     */
    Mathexpr() = delete;
    /**
     * Copy-constructor.
     */
    Mathexpr(const Mathexpr &copy) = delete;
    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Mathexpr(Shell &shell);
    /**
     * Destructor.
     */
    ~Mathexpr() override = default;
    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    int calculateExpression(String expr);
    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};

#endif