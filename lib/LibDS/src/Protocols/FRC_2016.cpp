/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "FRC_2016.h"

/**
 * Returns the display name of the protocol
 */
QString FRC_2016::name() {
    return QObject::tr ("FRC 2016 Protocol");
}

/**
 * Return a list with the following robot addresses:
 *     - roboRIO-TEAM-FRC.local
 *     - roboRIO-TEAM.local
 *     - 172.22.11.2
 *     - 10.TE.AM.2
 */
QString FRC_2016::defaultRobotAddress() {
    return QString ("roboRIO-%1-FRC.local").arg (config()->team());
}
