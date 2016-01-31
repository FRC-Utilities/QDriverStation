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

#ifndef _QDS_MW_STATUS_H
#define _QDS_MW_STATUS_H

#include <QWidget>
#include <LibDS/DriverStation.h>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QGridLayout;

class Status : public QWidget
{
    Q_OBJECT

public:
    explicit Status (QWidget* parent);

private slots:
    void createWidgets();
    void createLayouts();
    void configureStyles();
    void connectSlots();

    void updateJoysticks ();
    void updateTeam       (int team);
    void updateStatus     (QString status);
    void updateVoltage    (QString voltage);
    void updateCodeStatus (bool codeAvailable);
    void updateCommStatus (DS_CommStatus status);

    void doErrorAnimation();
    void toggleStatusColors();

private:
    QLabel* m_team;
    QLabel* m_voltage;
    QLabel* m_robotStatus;

    QPushButton* m_comm;
    QPushButton* m_code;
    QPushButton* m_sticks;

    QLabel* m_teamLabel;
    QLabel* m_commLabel;
    QLabel* m_codeLabel;
    QLabel* m_sticksLabel;
    QLabel* m_voltageLabel;

    QVBoxLayout* m_mainLayout;
    QGridLayout* m_LEDsLayout;
    QGridLayout* m_upperLayout;

    QWidget* m_LEDsWidget;
    QWidget* m_upperWidget;
};

#endif
