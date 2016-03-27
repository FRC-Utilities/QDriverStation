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

#include <QLocale>
#include <QTextCodec>
#include <QMessageBox>
#include <QApplication>

//==================================================================================================
// Application includes
//==================================================================================================

#include "Global.h"
#include "Settings.h"
#include "Languages.h"

//==================================================================================================
// Languages::globalVariables
//==================================================================================================

static QTranslator* _translator = Q_NULLPTR;

//==================================================================================================
// Languages::init
//==================================================================================================

void Languages::init() {
    QString locale;
    QTextCodec::setCodecForLocale (QTextCodec::codecForName ("UTF-8"));

    /* Get which translation file and font to open */
    switch (currentLanguage()) {
    case kAuto:
        locale = systemLanguage();
        break;
    case kGerman:
        locale = "de";
        break;
    case kEnglish:
        locale = "en";
        break;
    case kSpanish:
        locale = "es";
        break;
    default:
        locale = "en";
        break;
    }

    /* Load translations */
    translator()->load (":/languages/qds_" + locale);
    DS::log (DS::kInfoLevel, "Locale set to: " + locale);
}

//==================================================================================================
// Languages::appFont
//==================================================================================================

QFont Languages::appFont() {
    QFont font;
    font.setFamily ("QuickSand");
    font.setPixelSize (DPI_SCALE (12));
    return font;
}

//==================================================================================================
// Languages::monoFont
//==================================================================================================

QFont Languages::monoFont() {
    QFont font;
#if defined Q_OS_WIN
    font.setFamily     ("Consolas");
    font.setPixelSize  (DPI_SCALE (12));
#else
    font.setFamily     ("Inconsolata");
    font.setPixelSize  (DPI_SCALE (12));
#endif

    return font;
}

//==================================================================================================
// Languages::systemLanguage
//==================================================================================================

QString Languages::systemLanguage() {
    if (QLocale::system().name().split ("_").count() > 0)
        return QLocale::system().name().split ("_").at (0);

    else
        return "en";
}

//==================================================================================================
// Languages::getTranslator
//==================================================================================================

QTranslator* Languages::translator() {
    if (_translator == Q_NULLPTR) {
        _translator = new QTranslator;
        init();
    }

    return _translator;
}

//==================================================================================================
// Languages::currentLanguage
//==================================================================================================

Languages::LanguageType Languages::currentLanguage() {
    return (LanguageType) Settings::get ("Language", kEnglish).toInt();
}

//==================================================================================================
// Languages::getAvailableLanguages
//==================================================================================================

QStringList Languages::getAvailableLanguages() {
    QStringList list;

    list.append ("Auto");
    list.append ("Deutsch");
    list.append ("English");
    list.append ("Español");

    return list;
}

//==================================================================================================
// Languages::setLanguage
//==================================================================================================

void Languages::setLanguage (LanguageType language) {
    DS::log (DS::kInfoLevel,
             "Setting language to: " + QString::number (language));

    if (language != currentLanguage()) {
        Settings::set ("Language", language);
        QMessageBox::information (Q_NULLPTR,
                                  QObject::tr ("Change language"),
                                  QObject::tr ("You must restart the application "
                                               "in order for the changes to take "
                                               "effect"));
    }
}
