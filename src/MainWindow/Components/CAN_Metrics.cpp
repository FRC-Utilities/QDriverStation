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
#include <QGridLayout>

//=============================================================================
// Application includes
//=============================================================================

#include "CAN_Metrics.h"
#include "Global/Global.h"

//=============================================================================
// CAN_Metrics::CAN_Metrics
//=============================================================================

CAN_Metrics::CAN_Metrics (QWidget* parent) : QWidget (parent)
{
    createWidgets();
    createLayouts();

    connect (DS(), SIGNAL (CANInfoReceived   (DS_CAN)),
             this,   SLOT (onMetricsReceived (DS_CAN)));
}

//=============================================================================
// CAN_Metrics::createWidgets
//=============================================================================

void CAN_Metrics::createWidgets()
{
    m_util          = new QLabel ("0", this);
    m_busOff        = new QLabel ("0", this);
    m_txFull        = new QLabel ("0", this);
    m_receive       = new QLabel ("0", this);
    m_transmit      = new QLabel ("0", this);

    m_titleLabel    = new QLabel (tr ("CAN Metrics"),   this);
    m_utilLabel     = new QLabel (tr ("Utilization %"), this);
    m_busOffLabel   = new QLabel (tr ("Bus Off"),       this);
    m_txFullLabel   = new QLabel (tr ("TX Full"),       this);
    m_receiveLabel  = new QLabel (tr ("Receive"),       this);
    m_transmitLabel = new QLabel (tr ("Transmit"),      this);
}

//=============================================================================
// CAN_Metrics::createLayouts
//=============================================================================

void CAN_Metrics::createLayouts()
{
    m_layout = new QGridLayout    (this);
    m_layout->setSpacing          (DPI_SCALE (5));
    m_layout->addWidget           (m_titleLabel,    0, 0);
    m_layout->addWidget           (m_utilLabel,     1, 0);
    m_layout->addWidget           (m_busOffLabel,   2, 0);
    m_layout->addWidget           (m_txFullLabel,   3, 0);
    m_layout->addWidget           (m_receiveLabel,  4, 0);
    m_layout->addWidget           (m_transmitLabel, 5, 0);
    m_layout->addWidget           (m_util,          1, 1);
    m_layout->addWidget           (m_busOff,        2, 1);
    m_layout->addWidget           (m_txFull,        3, 1);
    m_layout->addWidget           (m_receive,       4, 1);
    m_layout->addWidget           (m_transmit,      5, 1);

    m_util->setAlignment          (Qt::AlignHCenter);
    m_busOff->setAlignment        (Qt::AlignHCenter);
    m_txFull->setAlignment        (Qt::AlignHCenter);
    m_receive->setAlignment       (Qt::AlignHCenter);
    m_transmit->setAlignment      (Qt::AlignHCenter);

    m_titleLabel->setAlignment    (Qt::AlignRight);
    m_utilLabel->setAlignment     (Qt::AlignRight);
    m_busOffLabel->setAlignment   (Qt::AlignRight);
    m_txFullLabel->setAlignment   (Qt::AlignRight);
    m_receiveLabel->setAlignment  (Qt::AlignRight);
    m_transmitLabel->setAlignment (Qt::AlignRight);

    QFont font;
    font.setBold (true);
    m_titleLabel->setFont (font);
}

//=============================================================================
// CAN_Metrics::onMetricsReceived
//=============================================================================

void CAN_Metrics::onMetricsReceived (DS_CAN can)
{
    m_util->setText     (QString::number (can.util));
    m_busOff->setText   (QString::number (can.busOff));
    m_txFull->setText   (QString::number (can.txFull));
    m_receive->setText  (QString::number (can.receive));
    m_transmit->setText (QString::number (can.transmit));
}
