# 
# Copyright (c) 2016 Alex Spataru <alex_spataru>
#
# Ugly hack to sync every submodule when the code is changed.
# Don't do this at home.
#

# Sync LibDS code
cd lib/LibDS
pwd
git add -A
git commit -a -m "Sync code with QDriverStation"
git push
git submodule update

# Sync QJoysticks code
cd ../QJoysticks
pwd
git add -A
git commit -a -m "Sync code with QDriverStation"
git push
git submodule update

# Sync QSimpleUpdater code
cd ../QSimpleUpdater
pwd
git add -A
git commit -a -m "Sync code with QDriverStation"
git push
git submodule update

# Sync main code
cd ../../
pwd
git add -A
git commit -a
git push
