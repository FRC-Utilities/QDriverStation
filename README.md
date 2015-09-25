# QDriverStation

[![Build Status](https://travis-ci.org/WinT-3794/QDriverStation.svg?branch=master)](https://travis-ci.org/WinT-3794/QDriverStation)
[![License](https://img.shields.io/github/license/wint-3794/qdriverstation.svg)](https://github.com/WinT-3794/QDriverStation/blob/master/LICENSE)
[![BitCoin donate button](https://img.shields.io/badge/bitcoin-donate-yellow.svg)](https://blockchain.info/address/1K85yLxjuqUmhkjP839R7C23XFhSxrefMx "Donate once-off to this project using BitCoin")

<script type='text/javascript' src='https://www.openhub.net/p/qdriverstation/widgets/project_basic_stats?format=js'></script>

### Introduction

QDriverStation is an open-source, cross-platform alternative to the FRC Driver Station.

The application looks and behaves almost identically to the FRC Driver Station, we may release some tutorials in the future. If you are already familiar with the FRC Driver Station, and like our project, we would welcome any contributions regarding the documentation and/or tutorials for new users.

Joystick input is achieved with SDL 2.0, which is a cross-platform library for C/C++.

The actual code that moves and manages a FRC robot is found in a [separate repository](https://github.com/WinT-3794/LibDS), which you can use in your own Qt projects or change it to support older (and future) communication protocols. Please check the [progress](https://github.com/WinT-3794/LibDS#progress) section for more info.

##### Other open-source Driver Stations from us...

###### Mobile Version

We are also working in a touch-friendly version of the Driver Station, which is intended for mobile phones and tablets. You can find more info about the Mobile Driver Station [here](http://github.com/wint-3794/ds-mobile)

###### 'Lite' Version

Did you ever want to drive your robot with your keyboard? We wanted too. If you just want to drive your robot without all the bells and whistles of the QDriverStation, we've got you covered! Click [here](http://github.com/wint-3794/ds-lite) for more information!

### Install notes

Once you finish installing the software, you can launch it and begin driving your robot. Just be sure to input your team number and to verify that the joysticks are working correctly. 

Notes:
- Mac users will be prompted to download an additional driver for Xbox 360 controllers to work.
- Windows users will be prompted to download Bonjour to get mDNS working, this is not necesary if you have the official FRC Driver Station already installed or if you use a static IP.

If your team uses a custom IP address for your roboRIO (or mDNS is not working, try 10.XX.YY.26), navigate to the 'Preferences' tab and click the 'Advanced Settings...' button to configure the driver station.

### Build instructions

###### Requirements

The only requirement to compile the application is to have [Qt](http://www.qt.io/download-open-source/) installed in your system. The desktop application will compile with both Qt4 and Qt5.

- If you are using Linux, make sure that you have installed the following packages:
    - libsdl2-dev
    
The project already contains the compiled SDL libraries for Windows and Mac.

###### Compiling the application

Once you have Qt installed, open *Desktop.pro* in Qt Creator and click the "Run" button.

Alternatively, you can also use the following commands:
- qmake
- make
- **Optional:** sudo make install

### Credits

This application was created by FRC team 3794 "WinT" from Metepec, Mexico. We sincerely hope that you enjoy our application and we would love some feedback from your team about it.
