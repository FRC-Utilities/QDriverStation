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

//=============================================================================
// System includes
//=============================================================================

#include <QFont>
#include <QLocale>
#include <QTextCodec>
#include <QMessageBox>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "Global.h"
#include "Settings.h"
#include "Languages.h"

//=============================================================================
// Languages::globalVariables
//=============================================================================

static QTranslator* _translator = Q_NULLPTR;

//=============================================================================
// Languages::init
//=============================================================================

void Languages::init()
{
    QString locale;
    QString fontName;
    QTextCodec::setCodecForLocale (QTextCodec::codecForName ("UTF-8"));

    /* Get which translation file and font to open */
    switch (currentLanguage())
        {
        case kAuto:
            fontName = "Quicksand";
            locale   = systemLanguage();
            break;
        case kGerman:
            locale   = "de";
            fontName = "Quicksand";
            break;
        case kEnglish:
            locale   = "en";
            fontName = "Quicksand";
            break;
        case kSpanish:
            locale   = "es";
            fontName = "Quicksand";
            break;
        case kAurebesh:
            locale   = "en";
            fontName = "Aurebesh";
            break;
        default:
            locale   = "en";
            fontName = "Quicksand";
            break;
        }

    /* Change application font */
    QFont font;
    font.setFamily (fontName);
    font.setPixelSize (DPI_SCALE (12));
    QApplication::setFont (font);

    /* Load translations */
    translator()->load (":/languages/qds_" + locale);
}

//=============================================================================
// Languages::getTranslator
//=============================================================================

QTranslator* Languages::translator()
{
    if (_translator == Q_NULLPTR)
        {
            _translator = new QTranslator;
            init();
        }

    return _translator;
}

//=============================================================================
// Languages::currentLanguage
//=============================================================================

Languages::LanguageType Languages::currentLanguage()
{
    return (LanguageType) Settings::get ("Language", kAuto).toInt();
}

//=============================================================================
// Languages::getAvailableLanguages
//=============================================================================

QStringList Languages::getAvailableLanguages()
{
    QStringList list;

    list.append ("Auto");
    list.append ("Deutsch");
    list.append ("English");
    list.append ("Español");
    list.append ("Aurebesh (Star Wars)");

    return list;
}

//=============================================================================
// Languages::setLanguage
//=============================================================================

void Languages::setLanguage (LanguageType language)
{
    if (language != currentLanguage())
        {
            Settings::set ("Language", language);
            QMessageBox::information (Q_NULLPTR,
                                      QObject::tr ("Change language"),
                                      QObject::tr ("You must restart the application "
                                                   "in order for the changes to take "
                                                   "effect"));
        }
}

//=============================================================================
// Languages::systemLanguage
//=============================================================================

QString Languages::systemLanguage()
{
    return QLocale::system().name().split ("_").at (0);
}
