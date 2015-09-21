:: Don't be a douchebag, please run this script before making a commit!
::
:: This script will make sure that we use the lastest versions of the registered
:: submodules and will format the source code automatically

cd etc/scripts
sync.bat
format-code.bat
