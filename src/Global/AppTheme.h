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

#ifndef _QDS_APP_THEME_H
#define _QDS_APP_THEME_H

#include <QString>

///
/// This class manages the appearance and colors used by
/// the QDriverStation, nothing more, nothing less.
///
class AppTheme {
  public:
    ///
    /// Allows us to create a "theme" by defining the different
    /// color types and values used by the application UI.
    ///
    typedef struct {
        QString base;
        QString highlight;
        QString background;
        QString foreground;
    } Theme;

    ///
    /// Configures the widget style and loads the saved theme.
    /// This function should be called only once during
    /// the application startup.
    ///
    static void init();

    ///
    /// Loads and applies the saved theme from the settings.
    ///
    static void loadTheme();

    ///
    /// Loads and applies the default (grey with green) theme.
    ///
    static void resetTheme();

    ///
    /// Loads, applies and saves the given \a theme.
    ///
    static void apply (const Theme& theme);

    ///
    /// Returns the current theme and its different colors.
    ///
    static Theme current();
};

#endif
