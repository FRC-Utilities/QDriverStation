# Changes

### QDriverStation 17.01.1

- Fix issue in which joystick input was not detected

### QDriverStation 17.01

- Transition to LibDS-C, which does not depend on Qt and is more efficient
- Re-implement logging system
- Avoid crashes when inputting invalid network addresses
- Change settings dialog buttons to be more user friendly
- Minor QML UI improvements/changes

### QDriverStation 16.08

- Fix issue where we the QDriverStation could not detect robot code with 2009/2014 robots

### QDriverStation 16.07

- Add some CLI options
- Add integrated mDNS responder
- Save logs as JSON files
- Allow user to set custom radio & FMS IPs
- Improve handling of FMS packets
- Minor improvements in code structure
- Add code to interpret 2014 FMS packets
- Implement logs window (netconsole + app logs)
- Fix communication issues

### QDriverStation 16.06.2

- Minor UI improvements
- Add automatic DNS/host lookup
- Improve code for obtaining CPU usage & Battery status
- Fix docked windows for Linux
- Save blacklisted joysticks through sessions
- Put blacklisted joysticks at the bottom of the joystick list

### QDriverStation 16.06.1

- Fix an issue that did not allow the DS to connect to the robot in some networks

### QDriverStation 16.06

- Support for 2014 protocol
- Implement parallel sockets to detect the robot a LOT faster
- Better robot voltage reading code
- Dynamic control of the watchdog
- Fix crash issue on Mac OS X
- Implement a logger
- Fix some minor issues in the UI
- Add base framework for implementing FMS communications
- Redesign UI in QML
- New application icon
- FMS support for 2015 protocol (UNTESTED)
- Allow enabling/disabling joysticks
- Add voltage charts
- Rewrite of LibDS to be more extensible and efficient (if a protocol requires constant comms. with radio or runs on TCP, we've got it!)

### QDriverStation 0.14:

- Support for 2016 protocol
- POV/Hat support for joysticks
- Re-write of most of the code base
- Map E-STOP to SHIFT and SPACE keys
- 2015 protocol gets a lot of improvements
- UI is written from ground-up in a more modular way
- Implement global event filter to avoid issues with virtual joystick or E-STOP trigger
- Implement built-in mDNS support for better cross-platform operation
- Do not use XInput for joystick reading, it only makes everything worse
- Improvements in socket programming for the LibDS
- Scalable UI to any pixel density
- Implement UI sound effects
- More options in the preferences window

### QDriverStation 0.13:

- Map E-STOP to SHIFT key
- Implement auto-updater
- Remove Gamepad API from joystick reader

### QDriverStation 0.12:

- Initial release
