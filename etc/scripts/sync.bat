REM As you may have noticed, I implemented the DS library and the Falcon QML
REM widgets as submodules. I still cannot say if that was a good idea...
REM
REM Run this script often to ensure that we use the lastest versions of the
REM registered submodules.
git submodule foreach git pull origin master

