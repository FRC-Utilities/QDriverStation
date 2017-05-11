/*
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
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

#include <LibDS.h>
#include <curses.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "joystick.h"
#include "interface.h"

static int running = 1;
static void process_events();
static void* get_user_input();

/**
 * Main entry point of the application
 */
int main()
{
    /* Initialize the DS (and its event loop) */
    DS_Init();

    /* Connect to the FRC simulator (or OpenRIO Sim) */
    DS_SetCustomRobotAddress ("127.0.0.1");

    /* Initialize the application modules */
    init_joysticks();
    init_interface();

    /* Get user input from a different thread */
    pthread_t user_input_thread;
    pthread_create (&user_input_thread, NULL, &get_user_input, NULL);

    /* Load the FRC 2016 communication protocol */
    DS_Protocol frc2016 = DS_GetProtocolFRC_2016();
    DS_ConfigureProtocol (&frc2016);

    /* Run the application's event loop (unrelated to DS) */
    while (running) {
        process_events();
        update_interface();
        update_joysticks();
        DS_Sleep (20);
    }

    /* Close the DS and the application modules */
    DS_Close();
    close_interface();
    close_joysticks();

    /* Exit the application */
    return EXIT_SUCCESS;
}

/**
 * Checks if the LibDS has any new events and displays them
 * on the console screen.
 */
static void process_events()
{
    DS_Event event;
    while (DS_PollEvent (&event)) {
        switch (event.type) {
        case DS_JOYSTICK_COUNT_CHANGED:
            set_has_joysticks (DS_GetJoystickCount());
            break;
        case DS_NETCONSOLE_NEW_MESSAGE:
            break;
        case DS_ROBOT_VOLTAGE_CHANGED:
            set_voltage (event.robot.voltage);
            break;
        case DS_ROBOT_CAN_UTIL_CHANGED:
            set_can (event.robot.can_util);
            break;
        case DS_ROBOT_CPU_INFO_CHANGED:
            set_cpu (event.robot.cpu_usage);
            break;
        case DS_ROBOT_RAM_INFO_CHANGED:
            set_ram (event.robot.ram_usage);
            break;
        case DS_ROBOT_DISK_INFO_CHANGED:
            set_disk (event.robot.disk_usage);
            break;
        case DS_STATUS_STRING_CHANGED:
            update_status_label();
            break;
        case DS_ROBOT_COMMS_CHANGED:
            set_robot_comms (event.robot.connected);
            break;
        case DS_ROBOT_CODE_CHANGED:
            set_robot_code (event.robot.code);
            break;
        default:
            break;
        }
    }
}

/**
 * Checks if the user has pressed any key on the keyboard and
 * reacts to the given user input
 */
static void* get_user_input()
{
    while (running) {
        switch (tolower (getch())) {
        case 'q':
            running = 0;
            break;
        case 'e':
            DS_SetRobotEnabled (1);
            break;
        case 'd':
            DS_SetRobotEnabled (0);
            break;
        case 'o':
            DS_SetRobotEnabled (0);
            DS_SetControlMode (DS_CONTROL_TELEOPERATED);
            break;
        case 'a':
            DS_SetRobotEnabled (0);
            DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
            break;
        case 't':
            DS_SetRobotEnabled (0);
            DS_SetControlMode (DS_CONTROL_TEST);
            break;
        case ' ':
            DS_SetEmergencyStopped (1);
            break;
        }

        DS_Sleep (20);
    }

    pthread_exit (0);
    return NULL;
}

