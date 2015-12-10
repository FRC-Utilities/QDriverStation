![Logo](extra/logo.png)

The DriverStation library allows you to connect and manage a robot easily by providing an abstraction layer between your application and the network commununications between the robot and the host computer.

The library is written in C++ and the Qt framework, allowing it to be used in many platforms, including (but not limited to) Windows, Linux/UNIX distros, Mac OS X, Android and iOS.

### Project sections

This library consists of three basic sections:

- [Core](#core-section) Section
- [Protocols](#protocols-section) Section
- [DriverStation](#driverstation-section) Section

#### Core section

The core section is in charge of implementing the 'generic' networking side and defining the common operations regardless of the protocol that we are using.

This section includes:

- A configurable UDP server/client
- A configurable NetConsole client
- A system for registering and managing joysticks
- An one-second watchdog that resets the current protocol if it expires
- A network discovery service, which also includes an integrated mDNS responder service
- A 'base' protocol, which implements all the functions except interpreting robot packets and generating client packets
- A threaded timer system, which regulates different processes in the library, your applicaiton can access this system too

#### Protocols section

This section contains the different protocols used for communicating with any FRC robot.

This section includes:

- The 2015 communication protocol

The 2009-2014 may be implemented soon.

#### DriverStation section

This section provides an application access to the rest of the library. Think of this section as the 'public' side of the library, while the rest can be considered as the 'internal' side.

### Credits

This library was created by FRC team 3794 "WinT" from Metepec, Mexico. We sincerely hope that you enjoy our application and we would love some feedback from your team about it.

### Contact

To contact us, you can send us an e-mail at [team3794@outlook.com](mailto:team3794@outlook). If you would like to report an issue, or a bug, please use the [Issues](https://github.com/wint-3794/qdriverstation/issues) page.

### License

This project is licensed under the MIT license. See the LICENSE file for more information.
