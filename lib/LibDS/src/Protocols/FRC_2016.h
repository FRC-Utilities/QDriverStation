/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_FRC_2016_H
#define _LIB_DS_FRC_2016_H

#include <Protocols/FRC_2015.h>

class FRC_2016 : public FRC_2015 {
  public:
    virtual QString name();
    virtual QStringList defaultRobotAddresses();
};

#endif
