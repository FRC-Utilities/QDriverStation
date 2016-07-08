# Introduction

The QDriverStation is a cross-platform and open-source alternative to the FRC Driver Station. It allows you to operate FRC robots with the major operating systems (Windows, Mac OSX and GNU/Linux). The QDriverStation is able to operate both 2009-2014 robots and 2015-2016 robots.

The QDriverStation uses LibDS to handle computer-to-robot and computer-to-FMS communications efficiently.

### Install notes

You can download the QDriverStation from both SourceForge and GitHub. We recommend you to download QDriverStation from GitHub, since SourceForge is known for serving misleading advertisements.

Once you finish installing the software, you can launch it and begin driving your robot. Just be sure to input your team number and to verify that the joysticks are working correctly.

Mac users will be prompted to download an additional driver for Xbox 360 controllers to work.

### Build instructions

#### Requirements

The only requirement to compile the application is to have Qt installed in your system. The desktop application will compile with Qt 5.2 or greater.

- If you are using Linux, make sure that you have installed the following packages:
    - libsdl2-dev

The project already contains the compiled SDL libraries for Windows and Mac.

#### Compiling the application

Once you have Qt installed, open *QDriverStation.pro* in Qt Creator and click the "Run" button.

Alternatively, you can also use the following commands:
- qmake
- make
- **Optional:** sudo make install

### License

The QDriverStation is released under the MIT license.
