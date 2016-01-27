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

#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>

//=============================================================================
// Application includes
//=============================================================================

#include "SettingsWindow.h"
#include "Utilities/Global.h"

//=============================================================================
// Global variables
//=============================================================================

const QString COLOR_RECTANGLE = "background-color: %1;";

//=============================================================================
// SettingsWindow::SettingsWindow
//=============================================================================

SettingsWindow::SettingsWindow()
{
    createWidgets();
    createLayouts();
    connectSlots();

    readSettings();
    applySettings();
}

//=============================================================================
// SettingsWindow::~SettingsWindow
//=============================================================================

SettingsWindow::~SettingsWindow()
{
    foreach (QWidget* widget, findChildren<QWidget*>())
        widget->deleteLater();
}

//=============================================================================
// SettingsWindow::createWidgets
//=============================================================================

void SettingsWindow::createWidgets()
{
    m_container = new QTabWidget (this);
    m_containerWidget = new QWidget (this);
    m_container->addTab (m_containerWidget, tr ("General"));

    m_addressWidget     = new QWidget   (this);
    m_robotAddress      = new QLineEdit (this);
    m_networkingIcon    = new QLabel    ("", this);
    m_otherSettingsIcon = new QLabel    ("", this);
    m_networkingBox     = new QGroupBox (tr ("Networking"), this);
    m_useCustomAddress  = new QCheckBox (tr ("Use custom robot address"), this);

    m_networkingIcon->setPixmap (QPixmap (":/icons/ethernet.png")
                                 .scaled (DPI_SCALE (48),
                                          DPI_SCALE (48),
                                          Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation));

    m_otherSettingsIcon->setPixmap (QPixmap (":/icons/other_settings.png")
                                    .scaled (DPI_SCALE (48),
                                            DPI_SCALE (48),
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation));

    m_buttonsWidget = new QWidget (this);
    m_reset  = new QPushButton (tr ("Reset"),  this);
    m_ok     = new QPushButton (tr ("OK"),     this);
    m_cancel = new QPushButton (tr ("Cancel"), this);
    m_ok->setDefault (true);

    m_appearanceBox = new QGroupBox (tr ("Colors"), this);

    m_baseLabel       = new QLabel (tr ("Base"), this);
    m_highlightLabel  = new QLabel (tr ("Highlight"), this);
    m_backgroundLabel = new QLabel (tr ("Background"), this);
    m_foregroundLabel = new QLabel (tr ("Foreground"), this);

    m_baseColor       = new QWidget (this);
    m_highlightColor  = new QWidget (this);
    m_backgroundColor = new QWidget (this);
    m_foregroundColor = new QWidget (this);

    QSize rectangle = QSize (DPI_SCALE (24), DPI_SCALE (24));
    QSize smallRect = QSize (DPI_SCALE (18), DPI_SCALE (18));
    m_baseColor->setFixedSize        (smallRect);
    m_highlightColor->setFixedSize   (smallRect);
    m_backgroundColor->setFixedSize  (smallRect);
    m_foregroundColor->setFixedSize  (smallRect);

    m_baseColor->setStyleSheet       (COLOR_RECTANGLE.arg ("#f00"));
    m_highlightColor->setStyleSheet  (COLOR_RECTANGLE.arg ("#f00"));
    m_backgroundColor->setStyleSheet (COLOR_RECTANGLE.arg ("#f00"));
    m_foregroundColor->setStyleSheet (COLOR_RECTANGLE.arg ("#f00"));

    m_baseButton       = new QPushButton ("...", this);
    m_highlightButton  = new QPushButton ("...", this);
    m_backgroundButton = new QPushButton ("...", this);
    m_foregroundButton = new QPushButton ("...", this);

    m_baseButton->setFixedSize       (rectangle);
    m_highlightButton->setFixedSize  (rectangle);
    m_backgroundButton->setFixedSize (rectangle);
    m_foregroundButton->setFixedSize (rectangle);

    m_baseEdit       = new QLineEdit (this);
    m_highlightEdit  = new QLineEdit (this);
    m_backgroundEdit = new QLineEdit (this);
    m_foregroundEdit = new QLineEdit (this);

    m_baseEdit->setFixedHeight (rectangle.height());
    m_highlightEdit->setFixedHeight (rectangle.height());
    m_backgroundEdit->setFixedHeight (rectangle.height());
    m_foregroundEdit->setFixedHeight (rectangle.height());

    m_otherSettingsContainer = new QWidget (this);
    m_otherSettingsBox = new QGroupBox (tr ("Other Settings"), this);
    m_autoUpdater   = new QCheckBox (tr ("Check for updates automatically"), this);
    m_soundEffects  = new QCheckBox (tr ("Enable UI sound effects"), this);
    m_promptOnQuit  = new QCheckBox (tr ("Prompt on quit"), this);

    m_robotAddress->setFixedWidth (DPI_SCALE (256));

    setWindowFlags (Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint);
}

//=============================================================================
// SettingsWindow::createLayouts
//=============================================================================

void SettingsWindow::createLayouts()
{
    QSpacerItem* spacer = new QSpacerItem (0, 0,
                                           QSizePolicy::MinimumExpanding,
                                           QSizePolicy::MinimumExpanding);

    /* Custom address checkbox & text */
    m_addressLayout = new QVBoxLayout (m_addressWidget);
    m_addressLayout->setSpacing (DPI_SCALE (5));
    m_addressLayout->addWidget (m_useCustomAddress);
    m_addressLayout->addWidget (m_robotAddress);

    /* Network icon & custom address */
    m_networkLayout = new QHBoxLayout (m_networkingBox);
    m_networkLayout->setSpacing (DPI_SCALE (5));
    m_networkLayout->setContentsMargins (MAIN_MARGINS());
    m_networkLayout->addWidget (m_networkingIcon);
    m_networkLayout->addWidget (m_addressWidget);
    m_networkLayout->addSpacerItem (spacer);

    /* Appearance box */
    m_appearanceLayout = new QGridLayout (m_appearanceBox);
    m_appearanceLayout->setVerticalSpacing (DPI_SCALE (5));
    m_appearanceLayout->setHorizontalSpacing (DPI_SCALE (10));
    m_appearanceLayout->setContentsMargins (MAIN_MARGINS());
    m_appearanceLayout->addWidget (m_baseLabel,        0, 0);
    m_appearanceLayout->addWidget (m_highlightLabel,   1, 0);
    m_appearanceLayout->addWidget (m_backgroundLabel,  2, 0);
    m_appearanceLayout->addWidget (m_foregroundLabel,  3, 0);
    m_appearanceLayout->addWidget (m_baseColor,        0, 1);
    m_appearanceLayout->addWidget (m_highlightColor,   1, 1);
    m_appearanceLayout->addWidget (m_backgroundColor,  2, 1);
    m_appearanceLayout->addWidget (m_foregroundColor,  3, 1);
    m_appearanceLayout->addWidget (m_baseEdit,         0, 2);
    m_appearanceLayout->addWidget (m_highlightEdit,    1, 2);
    m_appearanceLayout->addWidget (m_backgroundEdit,   2, 2);
    m_appearanceLayout->addWidget (m_foregroundEdit,   3, 2);
    m_appearanceLayout->addWidget (m_baseButton,       0, 3);
    m_appearanceLayout->addWidget (m_highlightButton,  1, 3);
    m_appearanceLayout->addWidget (m_backgroundButton, 2, 3);
    m_appearanceLayout->addWidget (m_foregroundButton, 3, 3);

    /* Other settings checkboxes */
    m_otherSettingsCheckboxes = new QVBoxLayout (m_otherSettingsContainer);
    m_otherSettingsCheckboxes->setSpacing (DPI_SCALE (2));
    m_otherSettingsCheckboxes->addWidget (m_autoUpdater);
    m_otherSettingsCheckboxes->addWidget (m_soundEffects);
    m_otherSettingsCheckboxes->addWidget (m_promptOnQuit);

    /* Other settings layout */
    m_otherSettingsLayout = new QHBoxLayout (m_otherSettingsBox);
    m_otherSettingsLayout->setSpacing (DPI_SCALE (5));
    m_otherSettingsLayout->setContentsMargins (MAIN_MARGINS());
    m_otherSettingsLayout->addWidget (m_otherSettingsIcon);
    m_otherSettingsLayout->addWidget (m_otherSettingsContainer);
    m_otherSettingsLayout->addSpacerItem (spacer);

    /* Everything except the buttons */
    m_containerLayout = new QVBoxLayout (m_containerWidget);
    m_containerLayout->addWidget (m_networkingBox);
    m_containerLayout->addWidget (m_otherSettingsBox);
    m_containerLayout->addWidget (m_appearanceBox);
    m_containerLayout->addSpacerItem (spacer);

    /* Reset, Apply & Cancel buttons */
    m_buttonsLayout = new QHBoxLayout (m_buttonsWidget);
    m_buttonsLayout->addWidget (m_reset);
    m_buttonsLayout->addSpacerItem (spacer);
    m_buttonsLayout->addWidget (m_cancel);
    m_buttonsLayout->addWidget (m_ok);

    /* Window layout */
    m_mainLayout = new QVBoxLayout (this);
    m_mainLayout->addWidget (m_container);
    m_mainLayout->addWidget (m_buttonsWidget);
}

//=============================================================================
// SettingsWindow::connectSlots
//=============================================================================

void SettingsWindow::connectSlots()
{
    connect (m_ok,     SIGNAL (clicked()), this, SLOT (apply()));
    connect (m_reset,  SIGNAL (clicked()), this, SLOT (reset()));
    connect (m_cancel, SIGNAL (clicked()), this, SLOT (cancel()));
    connect (m_useCustomAddress, SIGNAL (toggled    (bool)),
             m_robotAddress,       SLOT (setEnabled (bool)));

    connect (DS(), SIGNAL (protocolChanged()), this, SLOT (readSettings()));
}

//=============================================================================
// SettingsWindow::apply
//=============================================================================

void SettingsWindow::apply()
{
    applySettings();
    hide();
}

//=============================================================================
// SettingsWindow::reset
//=============================================================================

void SettingsWindow::reset()
{
    int answer = QMessageBox::question (this,
                                        tr ("Reset Settings"),
                                        tr ("Are you sure you want to reset "
                                            "these settings?"),
                                        QMessageBox::Yes,
                                        QMessageBox::No);
    if (answer == QMessageBox::Yes)
        {
            AppTheme::resetTheme();
            Settings::set ("Robot Address", "");

            readSettings();
        }
}

//=============================================================================
// SettingsWindow::cancel
//=============================================================================

void SettingsWindow::cancel()
{
    hide();
    readSettings();
}

//=============================================================================
// SettingsWindow::readSettings
//=============================================================================

void SettingsWindow::readSettings()
{
    /* Get saved robot address */
    QString address = Settings::get ("Robot Address", "").toString();

    /* Load address into the UI */
    m_robotAddress->setText (address);
    m_robotAddress->setEnabled (!address.isEmpty());
    m_useCustomAddress->setChecked (!address.isEmpty());

    /* Get saved theme */
    AppTheme::Theme theme = AppTheme::current();

    /* Change the text of the color line edits */
    m_baseEdit->setText       (theme.base);
    m_highlightEdit->setText  (theme.highlight);
    m_backgroundEdit->setText (theme.background);
    m_foregroundEdit->setText (theme.foreground);

    /* Change color of the color rectangles */
    m_baseColor->setStyleSheet       (COLOR_RECTANGLE.arg (theme.base));
    m_highlightColor->setStyleSheet  (COLOR_RECTANGLE.arg (theme.highlight));
    m_backgroundColor->setStyleSheet (COLOR_RECTANGLE.arg (theme.background));
    m_foregroundColor->setStyleSheet (COLOR_RECTANGLE.arg (theme.foreground));

    /* Get other settings */
    m_autoUpdater->setChecked  (Settings::get ("Auto Updater",   true).toBool());
    m_soundEffects->setChecked (Settings::get ("UI Sounds",      true).toBool());
    m_promptOnQuit->setChecked (Settings::get ("Prompt on Quit", true).toBool());

    /* Apply the loaded settings */
    applySettings();
}

//=============================================================================
// SettingsWindow::applySettings
//=============================================================================

void SettingsWindow::applySettings()
{
    /* Set the team number to be sure the address placeholder is good */
    DS()->setTeamNumber (Settings::get ("Team", 0).toInt());

    /* Only use a custom robot address if the checkbox is checked */
    QString ip = m_useCustomAddress->isChecked() ? m_robotAddress->text() : "";
    Settings::set ("Robot Address", ip);

    /* Apply the robot address */
    DS()->setRobotAddress (ip);
    m_robotAddress->setText (ip);
    m_robotAddress->setPlaceholderText (DS()->defaultRobotAddress());

    /* Apply other settings */
    Settings::set ("Auto Updater",   m_autoUpdater->isChecked());
    Settings::set ("UI Sounds",      m_soundEffects->isChecked());
    Settings::set ("Prompt on Quit", m_promptOnQuit->isChecked());

    /* Create and apply a theme from UI values */
    AppTheme::Theme theme;
    theme.base       = m_baseEdit->text();
    theme.highlight  = m_highlightEdit->text();
    theme.background = m_backgroundEdit->text();
    theme.foreground = m_foregroundEdit->text();
    AppTheme::apply (theme);
}
