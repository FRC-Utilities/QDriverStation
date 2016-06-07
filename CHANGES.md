# Changes

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
