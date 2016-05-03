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

#ifndef _LDS_PROTOCOL_2016_H
#define _LDS_PROTOCOL_2016_H

#include "FRC_2015.h"

namespace DS_Protocols {

class LIB_DS_DECL FRC_Protocol2016 : public FRC_Protocol2015 {
    Q_OBJECT

  public:
    inline virtual QString name() {
        return "FRC 2016 Protocol";
    }

    inline virtual QStringList additionalRobotIPs() {
        QStringList list;
        list.append (QString ("roboRIO-%1-FRC.local").arg (team()));
        list.append (QString ("roboRIO-%1.local").arg (team()));
        list.append (QString ("172.22.11.2"));
        return list;
    }
};
}

#endif
