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

#include "interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include <DS_Utils.h>
#include <DS_String.h>
#include <DS_Client.h>

/*
 * Define basic label states
 */
#define INVALID  "--.--"
#define ENABLED  "Enabled"
#define DISABLED "Disabled"

/*
 * Define windows
 */
static WINDOW* window = NULL;
static WINDOW* robotip_win = NULL;
static WINDOW* console_win = NULL;
static WINDOW* status_info = NULL;
static WINDOW* voltage_win = NULL;
static WINDOW* robot_status = NULL;
static WINDOW* bottom_window = NULL;

/*
 * Define window elements
 */
static DS_String can_str;
static DS_String cpu_str;
static DS_String ram_str;
static DS_String disk_str;
static DS_String robot_ip;
static DS_String voltage_str;
static DS_String console_str;
static DS_String rstatus_str;
static DS_String stick_check_str;
static DS_String rcode_check_str;
static DS_String robot_check_str;

/**
 * Changes the \a label to "[*]" if checked is greater than \c 0,
 * otherwise, the function will change the \a label to "[ ]"
 */
static void set_checked (DS_String* label, int checked)
{
    if (label) {
        DS_StrRmBuf (label);
        *label = DS_StrNew (checked ? "[*]" : "[ ]");
    }
}

/**
 * Return "--.--" if there are no communications with the robot, otherwise,
 * this function shall return the given \a string
 */
static void update_label (DS_String* string)
{
    if (!DS_GetRobotCommunications() && string) {
        DS_StrRmBuf (string);
        *string = DS_StrNew (INVALID);
    }
}

/**
 * Sets the default label texts
 */
static void init_strings (void)
{
    set_checked (&robot_check_str, 0);
    set_checked (&rcode_check_str, 0);
    set_checked (&stick_check_str, 0);

    can_str = DS_StrNew (INVALID);
    cpu_str = DS_StrNew (INVALID);
    ram_str = DS_StrNew (INVALID);
    disk_str = DS_StrNew (INVALID);
    robot_ip = DS_StrNew (INVALID);
    voltage_str = DS_StrNew (INVALID);
    rstatus_str = DS_StrNew (INVALID);
    console_str = DS_StrNew ("[INFO] Welcome to the ConsoleDS!");
}

/**
 * Creates the base windows of the application
 */
static void draw_windows (void)
{
    /* Delete the windows */
    delwin (robotip_win);
    delwin (console_win);
    delwin (status_info);
    delwin (voltage_win);
    delwin (robot_status);
    delwin (bottom_window);

    /* Set window sizing */
    int top_height = 3;
    int bottom_height = 3;
    int side_width = DS_Min (COLS / 4, 40);
    int central_height = LINES - (top_height + bottom_height);

    /* Create top windows */
    voltage_win  = newwin (top_height, side_width, 0, 0);
    robotip_win  = newwin (top_height, side_width, 0, COLS - side_width);
    robot_status = newwin (top_height, COLS - 2 * (side_width), 0, side_width);

    /* Create central windows */
    console_win = newwin (central_height, COLS - side_width, top_height, 0);
    status_info = newwin (central_height, side_width, top_height, COLS - side_width);

    /* Create botttom window */
    bottom_window = newwin (bottom_height, COLS, LINES - bottom_height, 0);

    /* Draw borders */
    wborder (voltage_win,   0, 0, 0, 0, 0, 0, 0, 0);
    wborder (robot_status,  0, 0, 0, 0, 0, 0, 0, 0);
    wborder (robotip_win,   0, 0, 0, 0, 0, 0, 0, 0);
    wborder (console_win,   0, 0, 0, 0, 0, 0, 0, 0);
    wborder (status_info,   0, 0, 0, 0, 0, 0, 0, 0);
    wborder (bottom_window, 0, 0, 0, 0, 0, 0, 0, 0);

    /* Add top window elements */
    mvwaddstr (console_win,  1, 2, DS_StrToChar (&console_str));
    mvwaddstr (robotip_win,  1, 2, DS_StrToChar (&robot_ip));
    mvwaddstr (robot_status, 1, 2, DS_StrToChar (&rstatus_str));

    /* Add voltage elements */
    mvwaddstr (voltage_win,  1,  2, "Voltage:");
    mvwaddstr (voltage_win,  1, 11, DS_StrToChar (&voltage_str));

    /* Add status panel elements */
    mvwaddstr (status_info, 1, 2, "STATUS:");
    mvwaddstr (status_info, 3, 2, DS_StrToChar (&robot_check_str));
    mvwaddstr (status_info, 4, 2, DS_StrToChar (&rcode_check_str));
    mvwaddstr (status_info, 5, 2, DS_StrToChar (&stick_check_str));
    mvwaddstr (status_info, 3, 6, "Robot Comms");
    mvwaddstr (status_info, 4, 6, "Robot Code");
    mvwaddstr (status_info, 5, 6, "Joysticks");

    /* Add robot status elements */
    mvwaddstr (status_info,  7, 2, "ROBOT STATUS:");
    mvwaddstr (status_info,  9, 2, "CAN:");
    mvwaddstr (status_info, 10, 2, "CPU:");
    mvwaddstr (status_info, 11, 2, "RAM:");
    mvwaddstr (status_info, 12, 2, "Disk:");
    mvwaddstr (status_info,  9, 8, DS_StrToChar (&can_str));
    mvwaddstr (status_info, 10, 8, DS_StrToChar (&cpu_str));
    mvwaddstr (status_info, 11, 8, DS_StrToChar (&ram_str));
    mvwaddstr (status_info, 12, 8, DS_StrToChar (&disk_str));

    /* Add bottom bar labels */
    mvwaddstr (bottom_window, 1, 2,  "Quit (q)");
    mvwaddstr (bottom_window, 1, 13, "Set enabled (e,d)");
    mvwaddstr (bottom_window, 1, 34, "Set Control Mode (o,a,t)");
    mvwaddstr (bottom_window, 1, 62, "More Options (m)");
}

/**
 * Refreshes the contents of each window
 */
static void refresh_windows (void)
{
    wnoutrefresh (window);
    wnoutrefresh (robotip_win);
    wnoutrefresh (console_win);
    wnoutrefresh (status_info);
    wnoutrefresh (voltage_win);
    wnoutrefresh (robot_status);
    wnoutrefresh (bottom_window);
}

/**
 * Creates the main window and sets the default label values
 */
void init_interface (void)
{
    int exit_code = 0;

#if defined __WIN32
    exit_code = system ("CLS");
#else
    exit_code = system ("clear");
#endif

    if (exit_code != 0)
        clear();

    init_strings();
    window = initscr();

    if (!window) {
        printf ("Cannot initialize window!\n");
        exit (EXIT_FAILURE);
    }

    noecho();
    curs_set (0);
    keypad (window, 1);

    clear();
}

/**
 * Removes the main window and deletes the allocated label memory
 */
void close_interface (void)
{
    delwin (window);
    endwin();
    refresh();

#if defined __WIN32
    system ("CLS");
#else
    system ("clear");
#endif
}

/**
 * Re-draws the user interface
 */
void update_interface (void)
{
    refresh();
    draw_windows();
    refresh_windows();
}

/**
 * Updates the status label to display the current state
 * of the robot and the LibDS
 */
void update_status_label (void)
{
    DS_StrRmBuf (&rstatus_str);
    rstatus_str = DS_StrNew (DS_GetStatusString());
}

/**
 * Updates the value displayed in the CAN field
 */
void set_can (const int can)
{
    DS_StrRmBuf (&can_str);
    can_str = DS_StrFormat ("%d %%", can);
    update_label (&can_str);
}

/**
 * Updates the value displayed in the CPU field
 */
void set_cpu (const int cpu)
{
    DS_StrRmBuf (&cpu_str);
    cpu_str = DS_StrFormat ("%d %%", cpu);
    update_label (&cpu_str);
}

/**
 * Updates the value displayed in the RAM field
 */
void set_ram (const int ram)
{
    DS_StrRmBuf (&ram_str);
    ram_str = DS_StrFormat ("%d %%", ram);
    update_label (&ram_str);
}

/**
 * Updates the value displayed in the disk field
 */
void set_disk (const int disk)
{
    DS_StrRmBuf (&disk_str);
    disk_str = DS_StrFormat ("%d %%", disk);
    update_label (&disk_str);
}

/**
 * Updates the state of the robot code checkbox
 */
void set_robot_code (const int code)
{
    set_checked (&rcode_check_str, code);
}

/**
 * Updates the state of the robot communications checkbox
 */
void set_robot_comms (const int comms)
{
    DS_StrRmBuf (&robot_ip);
    robot_ip = DS_StrNew (DS_GetAppliedRobotAddress());
    set_checked (&robot_check_str, comms);
}

/**
 * Updates the text of the robot voltage field
 */
void set_voltage (const double voltage)
{
    DS_StrRmBuf (&voltage_str);
    voltage_str = DS_StrFormat ("%f V", voltage);
    update_label (&voltage_str);
}

/**
 * Updates the state of the joysticks checkbox
 */
void set_has_joysticks (const int joysticks)
{
    set_checked (&stick_check_str, joysticks);
}
