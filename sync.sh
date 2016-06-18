# 
# Copyright (c) 2016 Alex Spataru <alex_spataru>
#
# Ugly hack to sync every submodule when the code is changed.
# Don't do this at home.
#

# Sync LibDS code
clear
cd lib/LibDS
echo "--------------------------------------------------------------------------------"
echo " Submodule: LibDS"
echo "--------------------------------------------------------------------------------"
git add -A
git commit -a -m "Sync code with QDriverStation"
git push
git submodule update
clear

# Sync QJoysticks code
cd ../QJoysticks
echo "--------------------------------------------------------------------------------"
echo " Submodule: QJoysticks"
echo "--------------------------------------------------------------------------------"
git add -A
git commit -a -m "Sync code with QDriverStation"
git push
git submodule update
clear

# Sync QSimpleUpdater code
cd ../QSimpleUpdater
echo "--------------------------------------------------------------------------------"
echo " Submodule: QSimpleUpdater"
echo "--------------------------------------------------------------------------------"
git add -A
git commit -a -m "Sync code with QDriverStation"
git push
git submodule update
clear

# Sync main code
cd ../../
echo "--------------------------------------------------------------------------------"
echo " Main"
echo "--------------------------------------------------------------------------------"
git add -A
git commit -a
git push
clear
