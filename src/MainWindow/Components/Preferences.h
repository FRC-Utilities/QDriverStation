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

#ifndef _QDS_MW_PREFERENCES_H
#define _QDS_MW_PREFERENCES_H

#include <QWidget>
#include <QPointer>

class QLabel;
class QSpinBox;
class QSpingBox;
class QComboBox;
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;

class Preferences : public QWidget {
    Q_OBJECT

  public:
    explicit Preferences (QWidget* parent);
    ~Preferences();

  private slots:
    void createWidgets();
    void createLayouts();
    void connectSlots();

    void readSettings();

    void setProtocol (int index);
    void setDashboard (int index);
    void setTeamNumber (int team);

    void loadPracticeValues();
    void savePracticeValues (int unused);

  private:
    QPointer<QLabel> m_protocolLabel;
    QPointer<QLabel> m_dashboardLabel;
    QPointer<QLabel> m_teamNumberLabel;

    QPointer<QSpinBox> m_teamNumber;
    QPointer<QComboBox> m_protocols;
    QPointer<QComboBox> m_dashboards;

    QPointer<QGroupBox> m_practiceTimings;

    QPointer<QLabel> m_delayLabel;
    QPointer<QLabel> m_teleopLabel;
    QPointer<QLabel> m_endGameLabel;
    QPointer<QLabel> m_countdownLabel;
    QPointer<QLabel> m_autonomousLabel;

    QPointer<QSpinBox> m_delay;
    QPointer<QSpinBox> m_teleop;
    QPointer<QSpinBox> m_endGame;
    QPointer<QSpinBox> m_countdown;
    QPointer<QSpinBox> m_autonomous;

    QPointer<QWidget> m_leftContainer;

    QPointer<QVBoxLayout> m_leftLayout;
    QPointer<QHBoxLayout> m_mainLayout;
    QPointer<QGridLayout> m_practiceLayout;
};

#endif
