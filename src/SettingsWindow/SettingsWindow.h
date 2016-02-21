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

#ifndef _QDS_SETTINGS_WINDOW_H
#define _QDS_SETTINGS_WINDOW_H

#include <QDialog>
#include <QPointer>

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

class SettingsWindow : public QDialog {
    Q_OBJECT

  public:
    explicit SettingsWindow();
    ~SettingsWindow();

    enum Colors {
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
    QPointer<QLabel> m_networkingIcon;
    QPointer<QWidget> m_addressWidget;
    QPointer<QLineEdit> m_robotAddress;
    QPointer<QGroupBox> m_networkingBox;
    QPointer<QCheckBox> m_useCustomAddress;

    QPointer<QTabWidget> m_container;
    QPointer<QWidget> m_containerWidget;
    QPointer<QGroupBox> m_appearanceBox;

    QPointer<QCheckBox> m_autoUpdater;
    QPointer<QCheckBox> m_soundEffects;
    QPointer<QCheckBox> m_promptOnQuit;
    QPointer<QLabel> m_otherSettingsIcon;
    QPointer<QGroupBox> m_otherSettingsBox;
    QPointer<QWidget> m_othersContainer;

    QPointer<QLabel> m_languageLabel;
    QPointer<QComboBox> m_languageBox;

    QPointer<QLabel> m_baseLabel;
    QPointer<QLabel> m_highlightLabel;
    QPointer<QLabel> m_backgroundLabel;
    QPointer<QLabel> m_foregroundLabel;

    QPointer<QWidget> m_baseColor;
    QPointer<QWidget> m_highlightColor;
    QPointer<QWidget> m_backgroundColor;
    QPointer<QWidget> m_foregroundColor;

    QPointer<QPushButton> m_baseButton;
    QPointer<QPushButton> m_highlightButton;
    QPointer<QPushButton> m_backgroundButton;
    QPointer<QPushButton> m_foregroundButton;

    QPointer<QLineEdit> m_baseEdit;
    QPointer<QLineEdit> m_highlightEdit;
    QPointer<QLineEdit> m_backgroundEdit;
    QPointer<QLineEdit> m_foregroundEdit;

    QPointer<QPushButton> m_reset;
    QPointer<QPushButton> m_ok;
    QPointer<QPushButton> m_cancel;
    QPointer<QWidget> m_buttonsWidget;

    QPointer<QVBoxLayout> m_mainLayout;
    QPointer<QVBoxLayout> m_addressLayout;
    QPointer<QHBoxLayout> m_networkLayout;
    QPointer<QHBoxLayout> m_buttonsLayout;
    QPointer<QVBoxLayout> m_containerLayout;
    QPointer<QHBoxLayout> m_networkingLayout;
    QPointer<QGridLayout> m_appearanceLayout;
    QPointer<QHBoxLayout> m_otherSettingsLayout;
    QPointer<QVBoxLayout> m_otherSettingsWidgets;
};

#endif
