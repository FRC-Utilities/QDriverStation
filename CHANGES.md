### QDriverStation 0.12:

- Initial release

### QDriverStation 0.13:

- Map E-STOP to SHIFT key
- Implement auto-updater
- Remove Gamepad API from joystick reader

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

### QDriverStation 0.15

- Support for 2014 protocol
- Support for multiple languages (Spanish, English and German)
- Better robot detection code (by looping through all DHCP addresses in less than 20 seconds). Beat that FRC DS!
- Dynamic control of the Watchdog timer
- Fix crash issue on Mac OS X
- Implement a simple logger
- Fix some minor issues in the UI
- Add base framework for implementing FMS protocols