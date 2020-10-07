![Header](etc/header/header.png)

The QDriverStation is a cross-platform and open-source alternative to the FRC Driver Station. It allows you to operate FRC robots with the major operating systems (Windows, Mac OSX and GNU/Linux). The QDriverStation is able to operate both 2009-2014 robots and 2015-2017 robots.

The actual code that operates a FRC robot is found in a [separate repository](https://github.com/FRC-Utilities/LibDS), which is written in C and can be used for your own projects or change it to support more communication protocols (such as [ROS](https://github.com/FRC-Utilities/QDriverStation/issues/21)).

You can find the online documentation of the QDriverStation and its sub-projects [here](http://frc-utilities.github.io/documentation/).

### NOTE: This is the legacy version</span>

Hopefully, the 2020 comm. protocol will be supported using [ds-rs](https://github.com/first-rust-competition/ds-rs). Work is underway, but I cannot guarantee anything.

### Install notes

You can download the QDriverStation from [GitHub](http://github.com/FRC-Utilities/QDriverStation-Legacy/releases).

Once you finish installing the software, you can launch it and begin driving your robot. Just be sure to input your team number and to verify that the joysticks are working correctly.

Mac users will be prompted to download an additional driver for Xbox 360 controllers to work.

###### Note for Linux users

For convenience, Linux releases are now handled with AppImages. To run the AppImage, simply download the latest release, make it executable and run it. 

Terminal commands below:

    cd Downloads
    chmod +x QDriverStation*.AppImage
    ./QDriverStation*.AppImage

More info can be found here: [https://appimage.org/](https://appimage.org/).

###### Warnings

If you are on Linux, the QDriverStation may detect some devices as a joystick ([more info...](https://gist.github.com/denilsonsa/978f1d842cf5430f57f6#file-51-these-are-not-joysticks-rules)). If that happens, just disable the faulty device by clicking on the power button next to its name.

### Build instructions

###### Requirements

The only requirement to compile the application is to have [Qt](http://www.qt.io/download-open-source/) installed in your system. The desktop application will compile with Qt 5.15 or greater.

- If you are using Linux, make sure that you have installed the following packages:
    - libsdl2-dev

The project already contains the compiled SDL libraries for Windows and Mac.

###### Compiling the application

Once you have Qt installed, open *QDriverStation.pro* in Qt Creator and click the "Run" button.

Alternatively, you can also use the following commands:
- qmake
- make
- **Optional:** sudo make install

You can check this [tutorial by Dakota Keeler](https://www.youtube.com/watch?v=G9DywTB9_cY) to compile the QDriverStation.

### Credits

This application was created by [Alex Spataru](http://github.com/alex-spataru).

Of course, many people contributed in different ways to this project, you can find more details in the [contributors list](CONTRIBUTORS.md). Finally, we want to thank you for trying this little project, we sincerely hope that you enjoy our application and we would love some of your feedback.
