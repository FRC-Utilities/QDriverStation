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

//==================================================================================================
// System includes
//==================================================================================================

#include <QFont>
#include <QIcon>
#include <QPalette>
#include <QApplication>
#include <QFontDatabase>
#include <QStyleFactory>

//==================================================================================================
// Application includes
//==================================================================================================

#include "AppTheme.h"
#include "Global/Settings.h"

//==================================================================================================
// AppTheme::Init
//==================================================================================================

void AppTheme::init() {
    /* Load the application fonts */
    QFontDatabase::addApplicationFont (":/fonts/Inconsolata.otf");
    QFontDatabase::addApplicationFont (":/fonts/Quicksand-Bold.ttf");
    QFontDatabase::addApplicationFont (":/fonts/Quicksand-Regular.ttf");

    /* Configure appearance options */
    QApplication::setStyle (QStyleFactory::create ("Fusion"));
#if defined Q_OS_MAC
    QApplication::setWindowIcon (QIcon (""));
#else
    QApplication::setWindowIcon (QIcon (":/icon.ico"));
#endif
}

//==================================================================================================
// AppTheme::LoadTheme
//==================================================================================================

void AppTheme::loadTheme() {
    /* Get the saved colors and modify the application palette */
    Theme theme = current();
    QPalette palette = QApplication::palette();
    palette.setColor (QPalette::Base,            theme.base);
    palette.setColor (QPalette::Window,          theme.base);
    palette.setColor (QPalette::HighlightedText, theme.base);
    palette.setColor (QPalette::Link,            theme.highlight);
    palette.setColor (QPalette::Highlight,       theme.highlight);
    palette.setColor (QPalette::BrightText,      theme.highlight);
    palette.setColor (QPalette::Button,          theme.background);
    palette.setColor (QPalette::AlternateBase,   theme.background);
    palette.setColor (QPalette::Text,            theme.foreground);
    palette.setColor (QPalette::WindowText,      theme.foreground);
    palette.setColor (QPalette::ButtonText,      theme.foreground);
    palette.setColor (QPalette::ToolTipBase,     "#222222");
    palette.setColor (QPalette::ToolTipText,     "#cccccc");

    /* Apply the modified palette to the application */
    QApplication::setPalette (palette);
}

//==================================================================================================
// AppTheme::resetTheme
//==================================================================================================

void AppTheme::resetTheme() {
    Theme theme;

    theme.base       = "#2a2a2a";
    theme.highlight  = "#2edc00";
    theme.background = "#313131";
    theme.foreground = "#dedede";

    apply (theme);
}

//==================================================================================================
// AppTheme::applyTheme
//==================================================================================================

void AppTheme::apply (const Theme& theme) {
    Settings::set ("Base",       theme.base);
    Settings::set ("Highlight",  theme.highlight);
    Settings::set ("Background", theme.background);
    Settings::set ("Foreground", theme.foreground);

    loadTheme();
}


//==================================================================================================
// AppTheme::current
//==================================================================================================

AppTheme::Theme AppTheme::current() {
    Theme theme;

    theme.base       = Settings::get ("Base",       "#2a2a2a").toString();
    theme.highlight  = Settings::get ("Highlight",  "#2edc00").toString();
    theme.background = Settings::get ("Background", "#313131").toString();
    theme.foreground = Settings::get ("Foreground", "#dedede").toString();

    return theme;
}
