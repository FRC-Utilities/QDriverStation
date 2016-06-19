#!/bin/sh

# Variables
license=mit
project="qdriverstation-16.06.1"
authorEmail="alex_spataru@outlook.com"

# Remove old build
if [ -d "$project" ]; then
    printf '%s\n' "Removing old build ($project)"
    rm -rf "$project"
fi

# Generate folders
mkdir deb_build
cd deb_build
mkdir $project
cd $project

# Build binary
mkdir build
cd build
qmake -qt5 ../../../../../../QDriverStation.pro
make -j4
mv qdriverstation ../
cd ..
rm -r build

# Copy icon & desktop file
cp ../../common/qdriverstation.png qdriverstation.png
cp ../../common/qdriverstation.desktop qdriverstation.desktop

# Copy debian config to build directory
cp -avr ../../debian debian

# Generate source build & debian package"
dh_make -s -c $license -e $authorEmail --createorig
dpkg-buildpackage
