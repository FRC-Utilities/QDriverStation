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

//=============================================================================
// System includes
//=============================================================================

#include <QList>

//=============================================================================
// Library includes
//=============================================================================

#include "LibDS/Core/ProtocolBase.h"
#include "LibDS/Core/ProtocolManager.h"

using namespace DS_Core;

//=============================================================================
//ProtocolManager::DS_ProtocolManager
//=============================================================================

ProtocolManager::ProtocolManager (QObject* parent) : QObject (parent) {
    m_protocol = Q_NULLPTR;
}

//=============================================================================
//ProtocolManager::currentProtocol
//=============================================================================

ProtocolBase* ProtocolManager::currentProtocol() const {
    return isValid() ? m_protocol : Q_NULLPTR;
}

//=============================================================================
//ProtocolManager::isValid
//=============================================================================

bool ProtocolManager::isValid() const {
    return (m_protocol != Q_NULLPTR);
}

//=============================================================================
//ProtocolManager::joystickCount
//=============================================================================

int ProtocolManager::joystickCount() const {
    return isValid() ? currentProtocol()->joysticks()->count() : 0;
}

//=============================================================================
//ProtocolManager::setProtocol
//=============================================================================

void ProtocolManager::setProtocol (ProtocolBase* protocol) {
    if (protocol != Q_NULLPTR) {
        /* There is already a running protocol */
        if (isValid()) {
            /* Pass current protocol settings to new protocol */
            protocol->setTeam         (m_protocol->team());
            protocol->setRobotAddress (m_protocol->robotAddress());

            /* De-configure the old protocol */
            m_protocol->disconnect();
        }

        /* Re-assign protocol */
        m_protocol = protocol;

        /* Re-connect protocol signals/slots */
        connect (m_protocol, &ProtocolBase::emergencyStopped,
                 this,       &ProtocolManager::emergencyStopped);
        connect (m_protocol, &ProtocolBase::codeChanged,
                 this,       &ProtocolManager::codeChanged);
        connect (m_protocol, &ProtocolBase::radioCommChanged,
                 this,       &ProtocolManager::radioCommChanged);
        connect (m_protocol, &ProtocolBase::communicationsChanged,
                 this,       &ProtocolManager::communicationsChanged);
        connect (m_protocol, &ProtocolBase::robotAddressChanged,
                 this,       &ProtocolManager::robotAddressChanged);
        connect (m_protocol, &ProtocolBase::controlModeChanged,
                 this,       &ProtocolManager::controlModeChanged);
        connect (m_protocol, &ProtocolBase::diskUsageChanged,
                 this,       &ProtocolManager::diskUsageChanged);
        connect (m_protocol, &ProtocolBase::ramUsageChanged,
                 this,       &ProtocolManager::ramUsageChanged);
        connect (m_protocol, &ProtocolBase::cpuUsageChanged,
                 this,       &ProtocolManager::cpuUsageChanged);
        connect (m_protocol, &ProtocolBase::voltageChanged,
                 this,       &ProtocolManager::voltageChanged);
        connect (m_protocol, &ProtocolBase::voltageBrownoutChanged,
                 this,       &ProtocolManager::voltageBrownoutChanged);
        connect (m_protocol, &ProtocolBase::CANInfoReceived,
                 this,       &ProtocolManager::CANInfoReceived);
        connect (m_protocol, &ProtocolBase::fmsChanged,
                 this,       &ProtocolManager::fmsChanged);


        DS::log (DS::kLibLevel,
                 QString ("Protocol 0x%1 loaded into memory")
                 .arg ((quintptr)protocol,
                       QT_POINTER_SIZE * 2, 16, QChar ('0')));
    }

    else
        DS::log (DS::kWarnLevel, "Input protocol pointer is NULL");
}

//=============================================================================
//ProtocolManager::readRobotPacket
//=============================================================================

void ProtocolManager::readRobotPacket (QByteArray data) {
    if (isValid())
        currentProtocol()->readRobotPacket (data);
}
