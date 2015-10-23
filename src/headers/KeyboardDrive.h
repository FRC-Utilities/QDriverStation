/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once
#ifndef _QDRIVER_STATION_KEYBOARD_DRIVE_H
#define _QDRIVER_STATION_KEYBOARD_DRIVE_H

#include <QKeyEvent>
#include <QCloseEvent>
#include <ui_KeyboardDrive.h>

/**
 * \class KeyboardDrive
 * \brief Moves the robot with the keyboard
 *
 * The \c KeyboardDrive class allows the user to operate the robot using input
 * from the computer's keyboard
 */
class KeyboardDrive : public QDialog
{
    Q_OBJECT

public:
    KeyboardDrive();

protected:
    void keyPressEvent (QKeyEvent* event);
    void keyReleaseEvent (QKeyEvent* event);

private:
    bool m_driveEnabled;
    Ui::KeyboardDrive ui;

private slots:
    void resizeToFit();
    void setDriveEnabled (bool enabled);
};

#endif
