![Logo](etc/logo.png)

[![Build Status](https://travis-ci.org/WinT-3794/LibDS.svg?branch=master)](https://travis-ci.org/WinT-3794/LibDS)
[![BitCoin donate button](https://img.shields.io/badge/bitcoin-donate-yellow.svg)](https://blockchain.info/address/1K85yLxjuqUmhkjP839R7C23XFhSxrefMx "Donate once-off to this project using BitCoin")

The DriverStation library allows you to connect and manage a robot easily by providing an abstraction layer between an application and the network commununications between the robot and the host computer.

The library is written in C++ and the Qt framework, allowing it to be used in many platforms, including (but not limited to) Windows, Linux/UNIX distros, Mac OS X, Android and iOS.

The QDriverStation and LibDS have received a lot of support from users and developers like you. To see a complete list of the contributors to these projects, click [here](https://github.com/WinT-3794/QDriverStation/blob/master/CONTRIBUTORS.md).

### Project sections

This library consists of three basic sections:

- [Core](#core-section) Section
- [Protocols](#protocols-section) Section
- [DriverStation](#driverstation-section) Section

#### Core section

The core section is in charge of implementing the 'generic' networking side and defining the common operations regardless of the protocol that we are using.

This section includes:

- A configurable NetConsole client
- An dynamic watchdog that resets the current protocol if it expires
- A configurable IP/network scanner used by each protocol to find the robot.
- A 'base' protocol, which implements all the functions except interpreting robot packets and generating client packets

#### Protocols section

This section contains the different protocols used for communicating with any FRC robot.

This section includes:

- The 2014 communication protocol (under heavy development)
- The 2015 communication protocol
- The 2016 communication protocol

#### DriverStation section

This section provides an application access to the rest of the library. Think of this section as the 'public' side of the library, while the rest can be considered as the 'internal' side.

### Credits

This library was created by FRC team 3794 "WinT" from Metepec, Mexico. We sincerely hope that you enjoy our application and we would love some feedback from your team about it.

### Contact

To contact us, you can send us an e-mail at [team3794@outlook.com](mailto:team3794@outlook). If you would like to report an issue, or a bug, please use the [Issues](https://github.com/wint-3794/qdriverstation/issues) page.

### License

This project is licensed under the MIT license. See the LICENSE file for more information.
