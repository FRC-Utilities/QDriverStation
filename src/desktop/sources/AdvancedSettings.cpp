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

#include <QMessageBox>
#include <QColorDialog>
#include <QApplication>

#include "Settings.h"
#include "AdvancedSettings.h"

#include <DriverStation.h>

const QString BASE
("#2a2a2a");
const QString HIGHLIGHT  ("#2edc00");
const QString BACKGROUND ("#313131");
const QString FOREGROUND ("#ffffff");

AdvancedSettings::AdvancedSettings()
{
    ui.setupUi (this);
    resize (0, 0);

    /* Main dialog buttons */
    connect (ui.ApplyButton, SIGNAL (clicked()), this, SLOT (onApplyClicked()));
    connect (ui.ResetButton, SIGNAL (clicked()), this, SLOT (onResetClicked()));
    connect (ui.CancelButton, SIGNAL (clicked()), this, SLOT (onCancelClicked()));

    /* Appearance tab */
    connect (ui.BaseButton, SIGNAL (clicked()), this, SLOT (onSelectorClicked()));
    connect (ui.HighlightButton, SIGNAL (clicked()), this,
             SLOT (onSelectorClicked()));
    connect (ui.BackgroundButton, SIGNAL (clicked()), this,
             SLOT (onSelectorClicked()));
    connect (ui.ForegroundButton, SIGNAL (clicked()), this,
             SLOT (onSelectorClicked()));
    connect (ui.BaseEdit, SIGNAL (textChanged (QString)), this,
             SLOT (onColorChanged (QString)));
    connect (ui.HighlightEdit, SIGNAL (textChanged (QString)), this,
             SLOT (onColorChanged (QString)));
    connect (ui.BackgroundEdit, SIGNAL (textChanged (QString)), this,
             SLOT (onColorChanged (QString)));
    connect (ui.ForegroundEdit, SIGNAL (textChanged (QString)), this,
             SLOT (onColorChanged (QString)));

    readSettings();
}

void AdvancedSettings::setTeamNumber (int team)
{
    ui.CustomAddressEdit->setPlaceholderText (
        QString ("roboRIO-%1.local").arg (team));
}

//------------------------------------------------------------------------------
// QSettings-related stuff
//------------------------------------------------------------------------------

void AdvancedSettings::readSettings()
{
    loadApplicationColors();

    QString base = Settings::get ("Base", BASE).toString();
    QString highlight = Settings::get ("Highlight", HIGHLIGHT).toString();
    QString background = Settings::get ("Background", BACKGROUND).toString();
    QString foreground = Settings::get ("Foreground", FOREGROUND).toString();

    ui.BaseEdit->setText (base);
    ui.HighlightEdit->setText (highlight);
    ui.BackgroundEdit->setText (background);
    ui.ForegroundEdit->setText (foreground);

    QString customAddress = Settings::get ("Custom Address", "").toString();
    ui.CustomAddressEdit->setText (customAddress);
    ui.CustomAddressCheck->setChecked (!customAddress.isEmpty());
    DriverStation::getInstance()->setCustomAddress (customAddress);

    emit updateColors();
}

void AdvancedSettings::applySettings()
{
    Settings::set ("Base", ui.BaseEdit->text());
    Settings::set ("Highlight", ui.HighlightEdit->text());
    Settings::set ("Background", ui.BackgroundEdit->text());
    Settings::set ("Foreground", ui.ForegroundEdit->text());

    QString address = ui.CustomAddressCheck->isChecked() ?
                      ui.CustomAddressEdit->text() : "";

    Settings::set ("Custom Address", address);
    DriverStation::getInstance()->setCustomAddress (address);

    loadApplicationColors();

    emit updateColors();
    emit settingsChanged();
}

void AdvancedSettings::resetSettings()
{
    Settings::set ("Base", BASE);
    Settings::set ("Highlight", HIGHLIGHT);
    Settings::set ("Background", BACKGROUND);
    Settings::set ("Foreground", FOREGROUND);

    readSettings();
}

//------------------------------------------------------------------------------
// Global qApp-related stuff
//------------------------------------------------------------------------------

void AdvancedSettings::loadApplicationColors()
{
    QColor base = QColor (Settings::get ("Base", BASE).toString());
    QColor highlight = QColor (Settings::get ("Highlight",
                               HIGHLIGHT).toString());
    QColor background = QColor (Settings::get ("Background",
                                BACKGROUND).toString());
    QColor foreground = QColor (Settings::get ("Foreground",
                                FOREGROUND).toString());

    QPalette palette = qApp->palette();
    palette.setColor (QPalette::Base, base);
    palette.setColor (QPalette::Window, base);
    palette.setColor (QPalette::Link, highlight);
    palette.setColor (QPalette::Text, foreground);
    palette.setColor (QPalette::Button, background);
    palette.setColor (QPalette::Highlight, highlight);
    palette.setColor (QPalette::HighlightedText, base);
    palette.setColor (QPalette::BrightText, highlight);
    palette.setColor (QPalette::WindowText, foreground);
    palette.setColor (QPalette::ButtonText, foreground);
    palette.setColor (QPalette::ToolTipBase, "#222222");
    palette.setColor (QPalette::ToolTipText, "#cccccc");
    palette.setColor (QPalette::AlternateBase, background);

    qApp->setPalette (palette);
}

//------------------------------------------------------------------------------
// Functions that react to UI events
//------------------------------------------------------------------------------

void AdvancedSettings::onResetClicked()
{
    int ret = QMessageBox::question (this, tr ("Clear Settings"),
                                     tr ("Are you sure you want to clear the "
                                         "application settings?"));

    if (ret == QMessageBox::Yes)
        resetSettings();
}

void AdvancedSettings::onApplyClicked()
{
    hide();
    applySettings();
}

void AdvancedSettings::onCancelClicked()
{
    hide();
    readSettings();
}

//------------------------------------------------------------------------------
// Functions that determine which preview boxes and line-edits to update
//------------------------------------------------------------------------------

void AdvancedSettings::onSelectorClicked()
{
    int emitter = getEmitter (QObject::sender());

    /* Configure the color dialog */
    QString color;
    QColorDialog dialog;
    dialog.setCurrentColor (getColor (emitter));
    dialog.setOption (QColorDialog::DontUseNativeDialog);

    /* Get new color */
    if (dialog.exec() == QColorDialog::Accepted)
        color = QVariant (dialog.currentColor()).toString();

    /* User clicked the 'Cancel' button in the color dialog */
    else
        return;

    /* Update the line edit that matches the button that called this function */
    switch (emitter) {
    case Base:
        ui.BaseEdit->setText (color);
        break;
    case Highlight:
        ui.HighlightEdit->setText (color);
        break;
    case Background:
        ui.BackgroundEdit->setText (color);
        break;
    case Foreground:
        ui.ForegroundEdit->setText (color);
        break;
    }
}

void AdvancedSettings::onColorChanged (QString color)
{
    int emitter = getEmitter (QObject::sender());

    /* The color is empty, use the previous value */
    if (color.isEmpty())
        color = QVariant (getColor (emitter)).toString();

    /* Make sure that the color is formatted as a HEX color */
    if (!color.contains ("#"))
        color = "#" + color;

    /* We use CSS to change the color of the preview box */
    QString style = QString ("background-color: %1;").arg (color);

    /* Update the preview box that matches the line edit that was changed */
    switch (emitter) {
    case Base:
        ui.BaseEdit->setText (color);
        ui.BaseBox->setStyleSheet (style);
        break;
    case Highlight:
        ui.HighlightEdit->setText (color);
        ui.HighlightBox->setStyleSheet (style);
        break;
    case Background:
        ui.BackgroundEdit->setText (color);
        ui.BackgroundBox->setStyleSheet (style);
        break;
    case Foreground:
        ui.ForegroundEdit->setText (color);
        ui.ForegroundBox->setStyleSheet (style);
        break;
    }
}

QColor AdvancedSettings::getColor (int type)
{
    QColor color;
    switch (type) {
    case Base:
        color = (QColor) ui.BaseEdit->text();
        break;
    case Highlight:
        color = (QColor) ui.HighlightEdit->text();
        break;
    case Background:
        color = (QColor) ui.BackgroundEdit->text();
        break;
    case Foreground:
        color = (QColor) ui.ForegroundEdit->text();
        break;
    }

    return color;
}

int AdvancedSettings::getEmitter (const QObject* object)
{
    int emitter = Base;
    QString name = object->objectName();

    if (name.contains ("Base"))
        emitter = Base;

    else if (name.contains ("Highlight"))
        emitter = Highlight;

    else if (name.contains ("Background"))
        emitter = Background;

    else if (name.contains ("Foreground"))
        emitter = Foreground;

    return emitter;
}
