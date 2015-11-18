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
#ifndef _QDRIVER_STATION_SMART_WINDOW_H
#define _QDRIVER_STATION_SMART_WINDOW_H

#include <QMoveEvent>
#include <QCloseEvent>
#include <QMainWindow>

/**
 * @class SmartWindow
 * @brief Implements automatic docking, positioning and auto-resize of a window
 *
 * Implements functions and hacks to allow a window to save its position, dock
 * at the bottom of the screen (as the FRC DriverStation does) and resizing
 * itself to fit the contents of the widgets that contains
 */
class SmartWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Initializes the window and moves it to its previous position
     */
    explicit SmartWindow();

    /**
     * Represents the available window display modes
     */
    enum WindowMode { kNormal, kDocked };

    /**
     * Returns \c true if the window mode is 'Docked'
     */
    bool isDocked();

    /**
     * Returns \c true if the window is set to have the smallest size possible
     */
    bool isFixedSize();

    /**
     * Returns \c true if the window is set to ask the user to confirm close
     */
    bool isPromptOnQuit();

protected:
    /**
     * @internal
     * Saves the window location when its moved
     */
    void moveEvent (QMoveEvent* e);

    /**
     * @internal
     * Depending on window configuration, the function may ask the user to
     * confirm the closing down of the application
     */
    void closeEvent (QCloseEvent* e);

public slots:
    /**
     * If \a fixed is set to \c true, the window will resize itself to the
     * smallest size possible and will inhibit the system from resizing it
     */
    void setUseFixedSize (bool fixed);

    /**
     * If \a prompt is set to \c true, the window will ask the user for
     * confirmation when a close event is detected
     */
    void setPromptOnQuit (bool prompt);

    /**
     * Changes the display mode of the window.
     * The available options are:
     *     - \c Normal, the window will use the system's window controls
     *     - \c Docked, the window will be frameless and will be docked to
     *          to the bottom of the screen to emulate the FRC DriverStation
     */
    void setWindowMode (const WindowMode& mode);

private slots:
    /**
     * If the window is not docked and the window is configured to use a fixed
     * size, the function will resize the window to the smallest size possible
     * and will prevent the system from resizing it.
     *
     * This function is called periodically by the window
     */
    void resizeToFit();

private:
    /**
     * Used to avoid showing a "Do you want to quit?" message twice when
     * closing the applicaiton or closing the window.
     */
    bool m_closingDown;

    /**
     * Used to determine whenever the user can resize the window or not.
     * This variable can be changed using the \c setUseFixedSize() function
     */
    bool m_useFixedSize;

    /**
     * Used to determine whenever we should ask for the user to confirm
     * quiting the application or closing the current window.
     *
     * This variable can be changed using the \c setPromptOnQuit() function.
     */
    bool m_promptOnQuit;

    /**
     * Represents the current window mode (normal or docked)
     */
    WindowMode m_windowMode;
};

#endif
