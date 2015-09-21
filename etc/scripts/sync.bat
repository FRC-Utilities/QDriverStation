:: As you may have noticed, I implemented the DS library and the Falcon QML
:: widgets as submodules. I still cannot say if that was a good idea...
::
:: Run this script often to ensure that we use the lastest versions of the
:: registered submodules.
::
git submodule foreach git pull origin master

