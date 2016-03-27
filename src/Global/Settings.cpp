/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

#include "Settings.h"

#include <QSettings>
#include <QApplication>

//==================================================================================================
// SETTINGS
//==================================================================================================

QSettings* SETTINGS() {
    static QSettings settings (QApplication::organizationName(),
                               QApplication::applicationName());
    return &settings;
}

//==================================================================================================
// Settings::clear
//==================================================================================================

void Settings::clear() {
    SETTINGS()->clear();
}

//==================================================================================================
// Settings::set
//==================================================================================================

void Settings::set (QString key, const QVariant& value) {
    SETTINGS()->setValue (key, value);
}

//==================================================================================================
// Settings::get
//==================================================================================================

QVariant Settings::get (QString key, const QVariant& default_value) {
    return SETTINGS()->value (key, default_value);
}
