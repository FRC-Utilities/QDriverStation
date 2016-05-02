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

#ifndef _QDS_MW_OPERATOR_H
#define _QDS_MW_OPERATOR_H

#include <QWidget>
#include <LibDS.h>

class QLabel;
class QComboBox;
class QPushButton;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QProgressBar;
class QButtonGroup;

class Operator : public QWidget {
    Q_OBJECT

  public:
    explicit Operator (QWidget* parent);
    ~Operator();

  private slots:
    void createWidgets();
    void createLayouts();
    void configureStyles();
    void connectSlots();

    void updateEnableState();
    void updateWindowState();
    void updateProgressbars();
    void updateControlMode (int index);
    void updateEnableStyles (bool enabled);
    void updateCommunications (DS::DS_CommStatus comms);

  signals:
    void showDocked();
    void showUnDocked();
    void requestErrorAnimation();

  private:
    QPushButton* m_enable;
    QPushButton* m_dsable;
    QWidget* m_stateWidget;
    QButtonGroup* m_stateGroup;
    QHBoxLayout* m_stateLayout;

    QPushButton* m_test;
    QPushButton* m_teleop;
    QPushButton* m_practice;
    QPushButton* m_autonomous;
    QWidget* m_modesWidget;
    QButtonGroup* m_modesGroup;
    QVBoxLayout* m_modesLayout;

    QLabel* m_plugIcon;
    QLabel* m_elapsedTime;
    QProgressBar* m_cpu;
    QProgressBar* m_battery;

    QLabel* m_cpuLabel;
    QLabel* m_batteryLabel;
    QLabel* m_windowModeLabel;
    QLabel* m_elapsedTimeLabel;
    QLabel* m_teamStationLabel;

    QComboBox* m_teamStation;

    QPushButton* m_windowNormal;
    QPushButton* m_windowDocked;
    QButtonGroup* m_windowGroup;
    QWidget* m_windowWidget;
    QHBoxLayout* m_windowModeLayout;

    QWidget* m_operationWidget;
    QWidget* m_indicatorsWidget;

    QHBoxLayout* m_mainLayout;
    QVBoxLayout* m_operationLayout;
    QGridLayout* m_indicatorsLayout;
};

#endif
