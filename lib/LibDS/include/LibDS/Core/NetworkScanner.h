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

#ifndef _LDS_NETWORK_SCANNER_H
#define _LDS_NETWORK_SCANNER_H

#include "LibDS/Core/Common.h"

namespace DS_Core {

///
/// The \c NetworkScanner class allows us to send and detect responses from
/// the robot controller with a given IP list.
///
/// The idea behind this is to use a multiple I/O sockeT pairs in order to
/// make the network scanning process much faster and efficient than if we
/// were to use a single I/O socket pair to scan the network.
///
class NetworkScanner : public QObject {
    Q_OBJECT

  public:
    explicit NetworkScanner();

    int inputPort() const;
    int outputPort() const;
    bool isEnabled() const;
    int scannerCount() const;

  public slots:
    void update();
    void setInputPort (int port);
    void setOutputPort (int port);
    void setEnabled (bool enabled);
    void sendData (QByteArray data);
    void setScannerCount (int scanner_count);
    void setScanningList (const QStringList& list);

  private slots:
    void readInputData();

  signals:
    void dataReceived (QString address, QByteArray data);

  private:
    int m_input;
    int m_output;
    int m_iterator;
    bool m_enabled;
    int m_scannerCount;
    QStringList m_list;
    QList<QUdpSocket*> m_inputSockets;
    QList<QUdpSocket*> m_outputSockets;
};

}

#endif
