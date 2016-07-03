#!/bin/sh

# Variables
license=mit
project="qdriverstation-16.06.2"
authorEmail="alex_spataru@outlook.com"

# Remove old build (if found)
if [ -d "$project" ]; then
    printf '%s\n' "Removing old build ($project)"
    rm -rf "$project"
fi

# Generate build folders
mkdir deb_build
cd deb_build
mkdir $project
cd $project

# Compile the QDriverStation
mkdir build
cd build
qmake -qt5 ../../../../../../QDriverStation.pro
make -j4

# Move QDriverStation binary and remove build files
mv qdriverstation ../
cd ..
rm -r build

# Copy icon & desktop file
cp ../../common/qdriverstation.png qdriverstation.png
cp ../../common/qdriverstation.desktop qdriverstation.desktop

# Copy debian config to build directory
cp -avr ../../debian debian

# Generate the source build and the debian package
dh_make -s -c $license -e $authorEmail --createorig
dpkg-buildpackage
