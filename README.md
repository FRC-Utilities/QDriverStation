# QDriverStation

### Introduction

QDriverStation is an open-source, cross-platform alternative to the FRC Driver Station.

The application looks and behaves almost identically to the FRC Driver Station, we may release some tutorials in the future. If you are already familiar with the FRC Driver Station, and like our project, we would welcome any contributions regarding the documentation and/or tutorials for new users.

Joystick input is achieved with SDL 2.0, which is a cross-platform library for C/C++.

# Progress

- [X] UI and application logic
- [X] Deploy configurations
- [X] Joystick input
- [ ] Communication protocol (check */lib/DriverStation* for more info)

### Desktop vs Mobile?

You may have noticed that we have two projects in this repository: a desktop version and a mobile version.
Consider the desktop version like a clone of the FRC DriverStation, while the mobile version is suited for touch screens.

Both versions use the same library to communicate with the robot (check the **lib** folder) but use a different UI (desktop uses traditional QWidgets, while mobile is based on QML).

### Screenshots (desktop)

![image](doc/screenshots/MainWindow.tiff)

![image](doc/screenshots/Joysticks.tiff)

![image](doc/screenshots/Settings.tiff)

### Screenshots (mobile)

![image](doc/screenshots/Mobile.tiff)

### Folder and File structure

**Folders:**

- <code>doc</code>: contains Doxygen configuration files
- <code>etc</code>: contains various files used for deployment of the application and resources
- <code>lib</code>: contains various libraries used by the application, including the **DriverStation** library.
- <code>src</code>: contains the C++ source code of the application

### Contributing

You can contribute to the project in many ways. For example:

**Developers:**

If you know a bit of programming, you can contribute directly to the project. If you are new to Git, [check this](https://help.github.com/) page.

**Media:**

Spread the word about our application to other FRC teams around the community.

**Give Feedback:**

Try out the application and let us know what you think and what we can improve.

**Designers:**

You can create an awesome and distinctive icon for our project. If you are familiar with web design, you could also help us out by creating the website of the QDriverStation.

### Installation

#### Compilation Requirements

The only requirement to compile the application is to have [Qt](http://www.qt.io/download-open-source/) installed in your system. The desktop application will compile with both Qt4 and Qt5, while the mobile application will require you to have at least Qt 5.4 installed.

- If you are using Linux, make sure that you have installed the following packages:
    - libsdl2-dev
    
The project already contains the compiled SDL libraries for Windows and Mac.

#### Compiling the application

Once you have Qt installed, open *Desktop.pro* in Qt Creator and click the "Run" button.

Alternatively, you can also use the following commands:
- qmake
- make
- **Optional:** sudo make install

#### Configuration

Once you finish installing the software, you can launch it and begin driving your robot. Just be sure to input your team number and to verify that the joysticks are working correctly. Note that Mac users will be prompted to download an additional driver for Xbox 360 controllers to work.

If your team uses a custom IP address for your roboRIO (or mDNS is not working, try 10.XX.YY.26), navigate to the 'Preferences' tab and click the 'Advanced Settings...' button to configure the driver station.

### Credits

This application was created by FRC team 3794 "WinT" from Metepec, Mexico. We sincerely hope that you enjoy our application and we would love some feedback from your team about it.

### Contact

To contact us, you can send us an e-mail at [team3794@outlook.com](mailto:team3794@outlook). If you would like to report an issue, or a bug, please use the [Issues](https://github.com/wint-3794/QDriverStation/issues) page.

### License

This project is licensed under the MIT license. See the COPYING file for more information.
