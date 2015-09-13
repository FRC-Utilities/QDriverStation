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

#ifndef _DRIVER_STATION_CLIENT_PACKETS_H
#define _DRIVER_STATION_CLIENT_PACKETS_H

#include "Common.h"

class QByteArray;

/**
 * @brief The FRCCommonControlData struct
 * Taken from: https://github.com/TheChillerCraft/Driver-Station
 *
 * The structure may or may not be modified in the future
 */
struct DS_CommonControlPacket {
    uint16_t packetIndex;
    uint8_t control;

    uint8_t dsDigitalIn;
    uint16_t teamID;

    char dsID_Alliance;
    char dsID_Position;

    union {
        int8_t stick0Axes[6];
        struct {
            int8_t stick0Axis1;
            int8_t stick0Axis2;
            int8_t stick0Axis3;
            int8_t stick0Axis4;
            int8_t stick0Axis5;
            int8_t stick0Axis6;
        } axis;
    } stick0;
    uint16_t stick0Buttons;     // Left-most 4 bits are unused

    union {
        int8_t stick1Axes[6];
        struct {
            int8_t stick1Axis1;
            int8_t stick1Axis2;
            int8_t stick1Axis3;
            int8_t stick1Axis4;
            int8_t stick1Axis5;
            int8_t stick1Axis6;
        } axis;
    } stick1;
    uint16_t stick1Buttons;     // Left-most 4 bits are unused

    union {
        int8_t stick2Axes[6];
        struct {
            int8_t stick2Axis1;
            int8_t stick2Axis2;
            int8_t stick2Axis3;
            int8_t stick2Axis4;
            int8_t stick2Axis5;
            int8_t stick2Axis6;
        } axis;
    } stick2;
    uint16_t stick2Buttons;     // Left-most 4 bits are unused

    union {
        int8_t stick3Axes[6];
        struct {
            int8_t stick3Axis1;
            int8_t stick3Axis2;
            int8_t stick3Axis3;
            int8_t stick3Axis4;
            int8_t stick3Axis5;
            int8_t stick3Axis6;
        } axis;
    } stick3;
    uint16_t stick3Buttons;     // Left-most 4 bits are unused

    //Analog inputs are 10 bit right-justified
    uint16_t analog1;
    uint16_t analog2;
    uint16_t analog3;
    uint16_t analog4;

    uint64_t cRIOChecksum;

    uint64_t FPGAChecksum0;
    uint64_t FPGAChecksum1;

    char versionData[8];

    uint8_t highEndData[938];

    uint32_t CRC;
};

/**
 * @brief The DS_RobotDataPacket struct
 * Taken from: https://github.com/TheChillerCraft/Driver-Station
 *
 * The structure may or may not be modified in the future
 */
struct DS_RobotDataPacket {
    uint8_t control;

    uint8_t batteryVolts;
    uint8_t batteryMV;

    union {
        uint8_t dio;
        struct {
            uint8_t out1 : 1;
            uint8_t out2 : 1;
            uint8_t out3 : 1;
            uint8_t out4 : 1;
            uint8_t out5 : 1;
            uint8_t out6 : 1;
            uint8_t out7 : 1;
            uint8_t out8 : 1;
        } outputs;
    } digitalOutput;

    uint8_t unknown1[4];
    uint16_t teamNumber;

    char macAddr[6];

    uint8_t unknow2[14];
    uint16_t packetCount;

    uint8_t highData[988];

    uint32_t crc;

    uint16_t unknown3;

    char userLine1[21];
    char userLine2[21];
    char userLine3[21];
    char userLine4[21];
    char userLine5[21];
    char userLine6[21];
};

/**
 * Generates a 6-byte packet that will be sent to the roboRIO at a rate of
 * 50 Hz (20 times per second).
 *
 * The packet will contain:
 *     - Bytes 1 & 2: Ping data
 *     - Byte 3: 0x01 (its magic)
 *     - Byte 4: Control mode (Autonomous, TeleOp, Test, etc)
 *     - Byte 5: Robot status (OK, RESTART_CODE or REBOOT)
 *     - Byte 6: Alliance and position of robot
 */
QByteArray DS_CommonControlPacket (DS_Status status, DS_Alliance alliance,
                                   DS_ControlMode mode);

/**
 * Sends a generated packet to the robot that contains the desired robot status,
 * its control mode, alliance and position.
 */
void DS_SendCommonControlPacket (DS_Status status, DS_Alliance alliance,
                                 DS_ControlMode mode, QString robotAddress);

#endif /* _DRIVER_STATION_CLIENT_PACKETS_H */
