/*
 * The Driver Station Library (LibDS)
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "DS_Utils.h"
#include "DS_Config.h"
#include "DS_Protocol.h"
#include "DS_DefaultProtocols.h"

#include <stdio.h>
#include <string.h>

/**
 * The 2016 FRC control system is very similar to the FRC 2015 control system,
 * the only (DS/Comms) difference is that the robot address is found at
 * roboRIO-TEAM-FRC.local (instead of roboRIO-TEAM.local).
 */
static DS_String robot_address (void)
{
    return DS_StrFormat ("roboRIO-%d-FRC.local", CFG_GetTeamNumber());
}

/**
 * Initializes and configures the FRC 2016 Communication Protocol
 */
DS_Protocol DS_GetProtocolFRC_2016 (void)
{
    DS_Protocol protocol = DS_GetProtocolFRC_2015();

    /* Set robot address function */
    protocol.robot_address = &robot_address;

    /* Set protocol name */
    DS_StrRmBuf (&protocol.name);
    protocol.name = DS_StrNew ("FRC 2016");

    return protocol;
}
