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
#include <LibDS/DriverStation.h>

#include "SettingsDialog.h"
#include "Global/Settings.h"

#define d_ColorBase       "#2a2a2a"
#define d_ColorHighlight  "#2edc00"
#define d_ColorBackground "#313131"
#define d_ColorForeground "#dedede"

SettingsDialog::SettingsDialog() {
    ui.setupUi (this);
    resize (0, 0);

    /* Main dialog buttons */
    connect (ui.ApplyButton,  SIGNAL (clicked()), this, SLOT (onApplyClicked()));
    connect (ui.ResetButton,  SIGNAL (clicked()), this, SLOT (onResetClicked()));
    connect (ui.CancelButton, SIGNAL (clicked()), this, SLOT (onCancelClicked()));

    /* Appearance tab */
    connect (ui.BaseButton,       SIGNAL (clicked()),
             this,                SLOT   (onSelectorClicked()));
    connect (ui.HighlightButton,  SIGNAL (clicked()),
             this,                SLOT   (onSelectorClicked()));
    connect (ui.BackgroundButton, SIGNAL (clicked()),
             this,                SLOT   (onSelectorClicked()));
    connect (ui.ForegroundButton, SIGNAL (clicked()),
             this,                SLOT   (onSelectorClicked()));
    connect (ui.BaseEdit,         SIGNAL (textChanged (QString)),
             this,                SLOT   (onColorChanged (QString)));
    connect (ui.HighlightEdit,    SIGNAL (textChanged (QString)),
             this,                SLOT   (onColorChanged (QString)));
    connect (ui.BackgroundEdit,   SIGNAL (textChanged (QString)),
             this,                SLOT   (onColorChanged (QString)));
    connect (ui.ForegroundEdit,   SIGNAL (textChanged (QString)),
             this,                SLOT   (onColorChanged (QString)));
}

void SettingsDialog::updatePlaceholder() {
    ui.CustomAddressEdit->setPlaceholderText (
        DriverStation::getInstance()->defaultRobotAddress());
}

//------------------------------------------------------------------------------
// READ, APPLY AND RESET SAVED SETTINGS
//------------------------------------------------------------------------------

void SettingsDialog::readSettings() {
    loadApplicationColors();

    /* Get saved colors */
    QString base       = Settings::get ("Base", d_ColorBase).toString();
    QString highlight  = Settings::get ("Highlight",
                                        d_ColorHighlight).toString();
    QString background = Settings::get ("Background",
                                        d_ColorBackground).toString();
    QString foreground = Settings::get ("Foreground",
                                        d_ColorForeground).toString();

    /* Publish the saved colors in the UI */
    ui.BaseEdit->setText (base);
    ui.HighlightEdit->setText (highlight);
    ui.BackgroundEdit->setText (background);
    ui.ForegroundEdit->setText (foreground);

    /* Get the custom robot address */
    QString customAddress = Settings::get ("Custom Address", "").toString();
    ui.CustomAddressEdit->setText (customAddress);
    ui.CustomAddressCheck->setChecked (!customAddress.isEmpty());
    DriverStation::getInstance()->setCustomAddress (customAddress);

    /* Now, redraw the UI */
    emit updateColors();
}

void SettingsDialog::applySettings() {
    /* Save colors */
    Settings::set ("Base", ui.BaseEdit->text());
    Settings::set ("Highlight", ui.HighlightEdit->text());
    Settings::set ("Background", ui.BackgroundEdit->text());
    Settings::set ("Foreground", ui.ForegroundEdit->text());

    /* Save custom address */
    QString address = ui.CustomAddressCheck->isChecked() ?
                      ui.CustomAddressEdit->text() : "";
    Settings::set ("Custom Address", address);
    DriverStation::getInstance()->setCustomAddress (address);

    /* Load the application colors */
    loadApplicationColors();

    /* Tell the big brother to redraw its widgets */
    emit updateColors();
    emit settingsChanged();
}

void SettingsDialog::resetSettings() {
    Settings::set ("Base", d_ColorBase);
    Settings::set ("Highlight", d_ColorHighlight);
    Settings::set ("Background", d_ColorBackground);
    Settings::set ("Foreground", d_ColorForeground);

    readSettings();
}

void SettingsDialog::loadApplicationColors() {
    /* Get the saved colors */
    QColor base       = QColor (Settings::get ("Base",
                                d_ColorBase).toString());
    QColor highlight  = QColor (Settings::get ("Highlight",
                                d_ColorHighlight).toString());
    QColor background = QColor (Settings::get ("Background",
                                d_ColorBackground).toString());
    QColor foreground = QColor (Settings::get ("Foreground",
                                d_ColorForeground).toString());

    /* Modify the application palette */
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

    /* Apply the modified palette to the application */
    qApp->setPalette (palette);
}

//------------------------------------------------------------------------------
// REACT TO UI EVENTS
//------------------------------------------------------------------------------

void SettingsDialog::onResetClicked() {
    int ret = QMessageBox::question (this,
                                     tr ("Clear Settings"),
                                     tr ("Are you sure you want to clear the "
                                         "application settings?"));

    if (ret == QMessageBox::Yes)
        resetSettings();
}

void SettingsDialog::onApplyClicked() {
    hide();
    applySettings();
}

void SettingsDialog::onCancelClicked() {
    hide();
    readSettings();
}

void SettingsDialog::onSelectorClicked() {
    Colors emitter = getEmitter (QObject::sender());

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

void SettingsDialog::onColorChanged (QString color) {
    Colors emitter = getEmitter (QObject::sender());

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

//------------------------------------------------------------------------------
// GET COLOR TYPE BASED ON EXTERNAL VALUES
//------------------------------------------------------------------------------

QColor SettingsDialog::getColor (Colors type) {
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

SettingsDialog::Colors SettingsDialog::getEmitter (const QObject* object) {
    Colors emitter = Base;
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
