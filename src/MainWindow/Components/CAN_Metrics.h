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

#ifndef _QDS_MW_CAN_METRICS_H
#define _QDS_MW_CAN_METRICS_H

#include <QWidget>
#include <LibDS/DriverStation.h>

class QLabel;
class QGridLayout;

class CAN_Metrics : public QWidget
{
    Q_OBJECT

public:
    explicit CAN_Metrics (QWidget* parent);

private slots:
    void createWidgets();
    void createLayouts();
    void onMetricsReceived (DS_CAN can);

private:
    QLabel* m_titleLabel;
    QLabel* m_utilLabel;
    QLabel* m_busOffLabel;
    QLabel* m_txFullLabel;
    QLabel* m_receiveLabel;
    QLabel* m_transmitLabel;

    QLabel* m_util;
    QLabel* m_busOff;
    QLabel* m_txFull;
    QLabel* m_receive;
    QLabel* m_transmit;

    QGridLayout* m_layout;
};

#endif
