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
#include <QPointer>

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

  signals:
    void showDocked();
    void showUnDocked();
    void requestErrorAnimation();

  private:
    QPointer<QPushButton> m_enable;
    QPointer<QPushButton> m_dsable;
    QPointer<QWidget> m_stateWidget;
    QPointer<QButtonGroup> m_stateGroup;
    QPointer<QHBoxLayout> m_stateLayout;

    QPointer<QPushButton> m_test;
    QPointer<QPushButton> m_teleop;
    QPointer<QPushButton> m_practice;
    QPointer<QPushButton> m_autonomous;
    QPointer<QWidget> m_modesWidget;
    QPointer<QButtonGroup> m_modesGroup;
    QPointer<QVBoxLayout> m_modesLayout;

    QPointer<QLabel> m_plugIcon;
    QPointer<QLabel> m_elapsedTime;
    QPointer<QProgressBar> m_cpu;
    QPointer<QProgressBar> m_battery;

    QPointer<QLabel> m_cpuLabel;
    QPointer<QLabel> m_batteryLabel;
    QPointer<QLabel> m_windowModeLabel;
    QPointer<QLabel> m_elapsedTimeLabel;
    QPointer<QLabel> m_teamStationLabel;

    QPointer<QComboBox> m_teamStation;

    QPointer<QPushButton> m_windowNormal;
    QPointer<QPushButton> m_windowDocked;
    QPointer<QButtonGroup> m_windowGroup;
    QPointer<QWidget> m_windowWidget;
    QPointer<QHBoxLayout> m_windowModeLayout;

    QPointer<QWidget> m_operationWidget;
    QPointer<QWidget> m_indicatorsWidget;

    QPointer<QHBoxLayout> m_mainLayout;
    QPointer<QVBoxLayout> m_operationLayout;
    QPointer<QGridLayout> m_indicatorsLayout;
};

#endif
