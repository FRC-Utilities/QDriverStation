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

#ifndef _QDS_SETTINGS_WINDOW_H
#define _QDS_SETTINGS_WINDOW_H

#include <QDialog>

class QLabel;
class QWidget;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QTabWidget;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow();
    ~SettingsWindow();

    enum Colors
    {
        Base,
        Highlight,
        Background,
        Foreground
    };

private slots:
    void createWidgets();
    void createLayouts();
    void connectSlots();

    void apply();
    void reset();
    void cancel();
    void readSettings();
    void applySettings();

    void updateTeam (int team);

    void onSelectorClicked();
    void onColorChanged (QString color);
    QColor getColorValue (Colors type);
    Colors getColorType (const QObject* object);

private:
    QLabel* m_networkingIcon;
    QWidget* m_addressWidget;
    QLineEdit* m_robotAddress;
    QGroupBox* m_networkingBox;
    QCheckBox* m_useCustomAddress;

    QTabWidget* m_container;
    QWidget* m_containerWidget;
    QGroupBox* m_appearanceBox;

    QCheckBox* m_autoUpdater;
    QCheckBox* m_soundEffects;
    QCheckBox* m_promptOnQuit;
    QLabel* m_otherSettingsIcon;
    QGroupBox* m_otherSettingsBox;
    QWidget* m_othersContainer;

    QLabel* m_languageLabel;
    QComboBox* m_languageBox;

    QLabel* m_baseLabel;
    QLabel* m_highlightLabel;
    QLabel* m_backgroundLabel;
    QLabel* m_foregroundLabel;

    QWidget* m_baseColor;
    QWidget* m_highlightColor;
    QWidget* m_backgroundColor;
    QWidget* m_foregroundColor;

    QPushButton* m_baseButton;
    QPushButton* m_highlightButton;
    QPushButton* m_backgroundButton;
    QPushButton* m_foregroundButton;

    QLineEdit* m_baseEdit;
    QLineEdit* m_highlightEdit;
    QLineEdit* m_backgroundEdit;
    QLineEdit* m_foregroundEdit;

    QPushButton* m_reset;
    QPushButton* m_ok;
    QPushButton* m_cancel;
    QWidget* m_buttonsWidget;

    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_addressLayout;
    QHBoxLayout* m_networkLayout;
    QHBoxLayout* m_buttonsLayout;
    QVBoxLayout* m_containerLayout;
    QHBoxLayout* m_networkingLayout;
    QGridLayout* m_appearanceLayout;
    QHBoxLayout* m_otherSettingsLayout;
    QVBoxLayout* m_otherSettingsWidgets;
};

#endif
