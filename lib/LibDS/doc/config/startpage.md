# Introduction

The DriverStation library allows you to connect and manage a robot easily by providing an abstraction layer between an application and the network comununications between the robot and the host computer.

The library is written in C++ and the Qt framework, allowing it to be used in many platforms, including (but not limited to) Windows, Linux/UNIX distros, Mac OS X, Android and iOS.

### Project sections

This library consists of three basic sections:

- Core section
- Protocols section
- Driver Station section

#### Core section

The core section is in charge of implementing all the support system of the library. In this section you will find:

- A parallel network scanner
- Definitions for DS-specific types
- Configurable NetConsole listener/sender
- A watchdog for reseting the DS when not receiving input from the robot
- The protocol abstract template
- The base signaling/event system of the library

#### Protocols section

This section contains the different protocols used for communicating with any FRC robot.

This section includes:

- The 2014 communication protocol
- The 2015/2016 communication protocol

#### Driver Station section

This section provides client access to the library, while preserving all the features for safe robot operation. Think of this section as the 'public' side of the library, while the rest can be considered as the 'internal' side.

### License

This project is licensed under the MIT license.
