QDriverStation README
=====================

This is the README file for the QDriverStation installer. 
If you are reading this, you've probably just run our installer 
and installed the QDriverStation on your system.


What should I do next?
----------------------

If you just want to drive your robot, you should be able to 
do so by running the  QDriverStation from the Start menu.
The Driver Station works with both 2015 robots and 2009-2014 robots.

If you want to drive a 2015 robot, we recommend you to download a 
mDNS service installer for your system. 
The recommended way to do that is by downloading the 
Bonjour Print Services from Apple: 

    https://support.apple.com/downloads/Bonjour_for_Windows

If you are running a version of Windows prior to Windows 8, we recommend you 
to download the Xbox joystick drivers from Microsoft:  
    
    https://www.microsoft.com/hardware/en-us/d/xbox-360-controller-for-windows


What do I do if it doesn't work?
--------------------------------

The most common way to repair this would be to:

    - Check that you are actually connected to the robot
    - Check that your team number is 

If that still doesn't work:

    - Open a command prompt
    - Ping the robot by typing (where XXYY is your team number):
      
          ping roboRIO-XXYY.local

    - If you get a response, copy the IP address of the roboRIO
    - Paste the IP address of the robot in the "Custom Address" field 
      in the settings dialog of the QDriverStation.


Contact information
-------------------

The QDriverStation home web site is: 
    
    https://qdriverstation.sourceforge.net

The e-mail of the lead developer is: 

    alex_spataru@outlook.com

The e-mail of the team is: 
    
    team3794@outlook.com
