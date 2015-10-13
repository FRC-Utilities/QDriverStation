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
#ifndef _QDRIVER_STATION_SETTINGS_H
#define _QDRIVER_STATION_SETTINGS_H

#include <QVariant>

/**
 * @class Settings
 * @brief Provides a single instance of QSettings to use around the application
 *
 * The \c Settings class was implemented to provide a simple way of reading
 * and writting application settings. Another reason for its implementation
 * was to avoid the initialization of multiple QSettings objects, when in
 * fact we only need one.
 */
class Settings {
  public:
    /**
     * Resets the user settings of the application
     */
    static void clear();

    /**
     * Saves the specified \a value to the specified \a key
     */
    static void set (QString key, const QVariant& value);

    /**
     * Reads the stored value of the \a key.
     * If the key is not found, returns the \a defaultValue
     */
    static QVariant get (QString key, const QVariant& defaultValue);
};

#endif
