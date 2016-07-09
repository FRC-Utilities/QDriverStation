/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_CRC32_H
#define _LIB_DS_CRC32_H

#include <QByteArray>

/**
 * \brief Computes the CRC32 data checksum of a data stream.
 *
 * Can be used to get the CRC32 over a stream if used with checked input/output
 * streams.
 */
class CRC32 {
  public:
    explicit CRC32();

    long value();
    void update (QByteArray buf);
    void update (QByteArray buf, int off, int len);

  private:
    int m_crc;
    int* m_crc_table;

  private:
    void make_crc_table();
};


#endif
