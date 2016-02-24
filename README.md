![Header](/etc/resources/logo.png)

[![Build Status](https://travis-ci.org/WinT-3794/QDriverStation.svg?branch=master)](https://travis-ci.org/WinT-3794/QDriverStation)
[![BitCoin donate button](https://img.shields.io/badge/bitcoin-donate-yellow.svg)](https://blockchain.info/address/1K85yLxjuqUmhkjP839R7C23XFhSxrefMx "Donate once-off to this project using BitCoin")
[![Github All Releases](https://img.shields.io/github/downloads/wint-3794/qdriverstation/total.svg)](https://github.com/wint-3794/qdriverstation/releases/latest)

QDriverStation is an open-source, cross-platform alternative to the FRC Driver Station.

The application looks and behaves almost identically to the FRC Driver Station, we may release some tutorials in the future. If you are already familiar with the FRC Driver Station, we would welcome any contributions regarding the documentation and/or tutorials for new users.

Joystick input is achieved with SDL 2.0, which is a cross-platform library for C/C++.

The actual code that moves and manages a FRC robot is found in a [separate repository](https://github.com/WinT-3794/LibDS), which you can use in your own Qt projects or change it to support older (and future) communication protocols.

We are also developing a **mobile version** of the QDriverStation. [Check it here](https://github.com/wint-3794/ds-mobile)!

### Download

You can download the QDriverStation from GitHub or SourceForge. If you got the QDriverStation from another website, it was probably modified by another person and may contain adware or some nasty junk in it.

We strongly recommend you to download the installers from GitHub and **not** from SourceForge.

**Download Links:**

- [Download from GitHub](https://github.com/wint-3794/qdriverstation/releases/latest)
- [Download from SourceForge](https://sourceforge.net/projects/qdriverstation/files/latest/download)

###### Note for Linux users

While for the moment we don't build Linux packages, there are some packages of the QDriverStation made available some members of the FRC community:

- [ArchLinux package by lopsided98](https://aur.archlinux.org/packages/qdriverstation-git/)
- [Launchpad/Ubuntu PPA by team 5256](https://launchpad.net/~bearzrobotics/+archive/ubuntu/ppaqdriverstation)

For more information regarding Linux installers, check [this issue](https://github.com/WinT-3794/QDriverStation/issues/9).

### Install notes

Once you finish installing the software, you can launch it and begin driving your robot. Just be sure to input your team number and to verify that the joysticks are working correctly. 

Mac users will be prompted to download an additional driver for Xbox 360 controllers to work.

###### Warning for Linux users

Due to a kernel bug, the QDriverStation may detect an external mouse as a joystick. For more information, check [this issue](https://github.com/WinT-3794/QDriverStation/issues/13).

### Build instructions

###### Requirements

The only requirement to compile the application is to have [Qt](http://www.qt.io/download-open-source/) installed in your system. The desktop application will compile with both Qt4 and Qt5.

- If you are using Linux, make sure that you have installed the following packages:
    - libsdl2-dev
 
The project already contains the compiled SDL libraries for Windows and Mac.

###### Compiling the application

Once you have Qt installed, open *QDriverStation.pro* in Qt Creator and click the "Run" button.

Alternatively, you can also use the following commands:
- qmake
- make
- **Optional:** sudo make install

### Translating the application

Before changing an existing translation, please do the following:

- Run the "lupdate" tool from terminal, this will "sync" the translations with the current code

If you want to create a new translation, please do the following:

- Open QDriverStation.pro with any text editor
- Go to the "Translations" section (at the bottom of the file)
- Add a new line with your language code:
    - For example, if you want to create a Russian translation, you would add this line:
    - > TRANSLATIONS += $$PWD/translations/qds_ru.ts
- Update the "translations.qrc" file (under the resources section)
- Do your translations and run the "lrelease" tool
- Update the "Languages" class (src/Utilities/Languages.h)

Once you finish creating or improving a translation, please do the following:

- Run the "lrelease" tool from terminal
- Recompile the QDriverStation

### Credits

This application was created by FRC team 3794 "WinT" from Metepec, Mexico. We sincerely hope that you enjoy our application and we would love some feedback from your team about it.
