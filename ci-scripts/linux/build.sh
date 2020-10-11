wget -q -O qt5.zip https://github.com/francescmm/ci-utils/releases/download/qt/qt5.zip;
unzip -qq qt5.zip;
export QTDIR=$PWD/qt5
export PATH=$QTDIR/bin:$PATH;
export QT_PLUGIN_PATH=$PWD/qt5/plugins;
mkdir build
cd build
g++ --version
qmake --version
$QTDIR/bin/qmake ../QDriverStation.pro
make -j 4
