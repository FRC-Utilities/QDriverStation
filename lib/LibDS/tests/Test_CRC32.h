/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef TEST_CRC32
#define TEST_CRC32

#include <QtTest>
#include <Utilities/CRC32.h>

//==============================================================================
// CRC32 TEST
//==============================================================================

class Test_CRC32 : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase() {
        CRC32 crc32;
        crc32.update (QString ("@Ahead)Together!FRC^2016").toUtf8());

        QVERIFY (crc32.value() == 0x0C5455B1);
    }
};

#endif
