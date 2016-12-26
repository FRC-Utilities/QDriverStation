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
static bstring can_str = NULL;
static bstring cpu_str = NULL;
static bstring ram_str = NULL;
static bstring disk_str = NULL;
static bstring rstatus_str = NULL;
static bstring robot_ip = NULL;
static bstring voltage_str = NULL;
static bstring console_str = NULL;
static bstring stick_check_str = NULL;
static bstring rcode_check_str = NULL;
static bstring robot_check_str = NULL;

/**
 * Changes the \a label to "[*]" if checked is greater than \c 0,
 * otherwise, the function will change the \a label to "[ ]"
 */
static bstring set_checked (bstring label, int checked)
{
    DS_FREESTR (label);
    label = bfromcstr (checked > 0 ? "[*]" : "[ ]");
    return label;
}

/**
 * Return "--.--" if there are no communications with the robot, otherwise,
 * this function shall return the given \a string
 */
static bstring update_label (bstring string)
{
    if (DS_GetRobotCommunications())
        return string;

    return bfromcstr (INVALID);
}

/**
 * Sets the default label texts
 */
static void init_strings()
{
    robot_check_str = set_checked (robot_check_str, 0);
    rcode_check_str = set_checked (rcode_check_str, 0);
    stick_check_str = set_checked (stick_check_str, 0);

    can_str = bfromcstr (INVALID);
    cpu_str = bfromcstr (INVALID);
    ram_str = bfromcstr (INVALID);
    disk_str = bfromcstr (INVALID);
    robot_ip = bfromcstr (INVALID);
    voltage_str = bfromcstr (INVALID);
    rstatus_str = bfromcstr (DS_GetStatusString());
    console_str = bfromcstr ("[INFO] Welcome to the ConsoleDS!");
}

/**
 * De-allocates the memory used by the strings
 */
static void close_strings()
{
    DS_FREESTR (can_str);
    DS_FREESTR (cpu_str);
    DS_FREESTR (ram_str);
    DS_FREESTR (disk_str);
    DS_FREESTR (robot_ip);
    DS_FREESTR (rstatus_str);
    DS_FREESTR (voltage_str);
    DS_FREESTR (console_str);
    DS_FREESTR (stick_check_str);
    DS_FREESTR (rcode_check_str);
    DS_FREESTR (robot_check_str);
}

/**
 * Creates the base windows of the application
 */
static void draw_windows()
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
    mvwaddstr (console_win,  1, 2, bstr2cstr (console_str, 0));
    mvwaddstr (robotip_win,  1, 2, bstr2cstr (robot_ip, 0));
    mvwaddstr (robot_status, 1, 2, bstr2cstr (rstatus_str, 0));

    /* Add voltage elements */
    mvwaddstr (voltage_win,  1,  2, "Voltage:");
    mvwaddstr (voltage_win,  1, 12, bstr2cstr (voltage_str, 0));

    /* Add status panel elements */
    mvwaddstr (status_info, 1, 2, "STATUS:");
    mvwaddstr (status_info, 3, 2, bstr2cstr (robot_check_str, 0));
    mvwaddstr (status_info, 4, 2, bstr2cstr (rcode_check_str, 0));
    mvwaddstr (status_info, 5, 2, bstr2cstr (stick_check_str, 0));
    mvwaddstr (status_info, 3, 6, "Robot Comms");
    mvwaddstr (status_info, 4, 6, "Robot Code");
    mvwaddstr (status_info, 5, 6, "Joysticks");

    /* Add robot status elements */
    mvwaddstr (status_info,  7, 2, "ROBOT STATUS:");
    mvwaddstr (status_info,  9, 2, "CAN:");
    mvwaddstr (status_info, 10, 2, "CPU:");
    mvwaddstr (status_info, 11, 2, "RAM:");
    mvwaddstr (status_info, 12, 2, "Disk:");
    mvwaddstr (status_info,  9, 8, bstr2cstr (can_str, 0));
    mvwaddstr (status_info, 10, 8, bstr2cstr (cpu_str, 0));
    mvwaddstr (status_info, 11, 8, bstr2cstr (ram_str, 0));
    mvwaddstr (status_info, 12, 8, bstr2cstr (disk_str, 0));

    /* Add bottom bar labels */
    mvwaddstr (bottom_window, 1, 2,  "Quit (q)");
    mvwaddstr (bottom_window, 1, 13, "Set enabled (e,d)");
    mvwaddstr (bottom_window, 1, 34, "Set Control Mode (o,a,t)");
    mvwaddstr (bottom_window, 1, 62, "More Options (m)");
}

/**
 * Refreshes the contents of each window
 */
static void refresh_windows()
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
void init_interface()
{
#if defined __WIN32
    system ("CLS");
#else
    system ("clear");
#endif

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
void close_interface()
{
    delwin (window);
    endwin();
    refresh();
    close_strings();

#if defined __WIN32
    system ("CLS");
#else
    system ("clear");
#endif
}

/**
 * Re-draws the user interface
 */
void update_interface()
{
    refresh();
    draw_windows();
    refresh_windows();
}

/**
 * Updates the value displayed in the CAN field
 */
void set_can (const int can)
{
    DS_FREESTR (can_str);
    can_str = update_label (bformat ("%d %%", can));
}

/**
 * Updates the value displayed in the CPU field
 */
void set_cpu (const int cpu)
{
    DS_FREESTR (cpu_str);
    cpu_str = update_label (bformat ("%d %%", cpu));
}

/**
 * Updates the value displayed in the RAM field
 */
void set_ram (const int ram)
{
    DS_FREESTR (ram_str);
    ram_str = update_label (bformat ("%d %%", ram));
}

/**
 * Updates the value displayed in the disk field
 */
void set_disk (const int disk)
{
    DS_FREESTR (disk_str);
    disk_str = update_label (bformat ("%d %%", disk));
}

/**
 * Updates the state of the robot code checkbox
 */
void set_robot_code (const int code)
{
    rcode_check_str = set_checked (rcode_check_str, code);
}

/**
 * Updates the state of the robot communications checkbox
 */
void set_robot_comms (const int comms)
{
    DS_FREESTR (robot_ip);
    robot_ip = bstrcpy (DS_GetAppliedRobotAddress());
    robot_check_str = set_checked (robot_check_str, comms);
}

/**
 * Updates the text of the robot voltage field
 */
void set_voltage (const double voltage)
{
    DS_FREESTR (voltage_str);
    voltage_str = update_label (bformat ("%.2f", voltage));
}

/**
 * Updates the state of the joysticks checkbox
 */
void set_has_joysticks (const int joysticks)
{
    stick_check_str = set_checked (stick_check_str, joysticks);
}

/**
 * Updates the status label to display the current state
 * of the robot and the LibDS
 */
void update_status_label (const bstring string)
{
    DS_FREESTR (rstatus_str);
    rstatus_str = bstrcpy (string);
}
