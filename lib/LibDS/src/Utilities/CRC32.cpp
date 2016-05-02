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

#include "LibDS/Utilities/CRC32.h"

using namespace DS_Utilities;

//==================================================================================================
// CRC32::CRC32
//==================================================================================================

CRC32::CRC32() {
    make_crc_table();
}

//==================================================================================================
// CRC32::value
//==================================================================================================

long CRC32::value() {
    return (long) m_crc & 0xFFFFFFFFL;
}

//==================================================================================================
// CRC32::update
//==================================================================================================

void CRC32::update (QByteArray buf) {
    update (buf, 0, buf.length());
}

//==================================================================================================
// CRC32::update
//==================================================================================================

void CRC32::update (QByteArray buf, int off, int len) {
    m_crc = 0;
    int c = ~m_crc;
    while (--len >= 0)
        c = m_crc_table[ (c ^ buf[off++]) & 0xff] ^ (((quint32) c) >> 8);
    m_crc = ~c;
}

//==================================================================================================
// CRC32::make_crc_table
//==================================================================================================

void CRC32::make_crc_table() {
    m_crc = 0;
    m_crc_table = new int[256];

    for (int n = 0; n < 256; n++) {
        int c = n;
        for (int k = 8; --k >= 0;) {
            if ((c & 1) != 0)
                c = 0xEDB88320 ^ (((quint32) c) >> 1);
            else
                c = ((quint32) c) >> 1;
        }

        m_crc_table[n] = c;
    }
}
