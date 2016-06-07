STEPS
-----

* Assuming you have downloaded NSIS (http://nsis.sf.net)

1. Compile the application
2. Run "windeployqt"
3. Place the output files in a folder named "bin"
4. Create a new folder with the application name
5. Place the "bin" folder in the application folder
6. Place the files in the "extra-files" folder in the application folder
7. Copy the script and place it one directory above the application folder
8. Compile the script, do not release it if you have any warning

INPUT FOLDER STRUCTURE
----------------------

$root
    install-script.nsi
    \QDriverStation
       LICENSE.html
       CONTRIBUTORS.html
       CHANGES.html
       README-SDL.txt      
       uninstall.exe
       \bin
           QDriverStation.exe
           {dependencies}

OUTPUT FOLDER STRUCTURE
-----------------------

$program-files
    \QDriverStation
        LICENSE.txt
        README-QDS.txt
        README-SDL.txt        
        uninstall.exe
        \bin
            QDriverStation.exe
            {dependencies}