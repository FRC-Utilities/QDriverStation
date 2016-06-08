/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "CRC32.h"

/**
 * Computes CRC32 data checksum of a data stream.
 * The actual CRC32 algorithm is described in RFC 1952
 * (GZIP file format specification version 4.3).
 * Can be used to get the CRC32 over a stream if used with checked input/output
 * streams.
 */
CRC32::CRC32() {
    make_crc_table();
}

/**
 * Returns the CRC32 data checksum computed so far.
 */
long CRC32::value() {
    return (long) m_crc & 0xFFFFFFFFL;
}

/**
 * Overloaded function. Adds the byte array to the data checksum.
 *
 * @param buf the data buffer
 */
void CRC32::update (QByteArray buf) {
    update (buf, 0, buf.length());
}

/**
 * Adds the byte array to the data checksum.
 *
 * @param buf the data buffer
 * @param off the offset in the buffer where the data starts
 * @param len the length of the data
 */
void CRC32::update (QByteArray buf, int off, int len) {
    m_crc = 0;
    int c = ~m_crc;
    while (--len >= 0)
        c = m_crc_table[ (c ^ buf[off++]) & 0xff] ^ (((quint32) c) >> 8);
    m_crc = ~c;
}

/**
 * Generates a table for fast CRC
 */
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
