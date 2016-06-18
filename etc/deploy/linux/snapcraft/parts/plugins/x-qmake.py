import os

import snapcraft

class QmakePlugin(snapcraft.BasePlugin):

    def __init__(self, name, options, project):
        super().__init__(name, options, project)
        self.build_packages.extend(['qt5-qmake', 'make'])

    def build(self):
        super().build()

        # Run qmake to generate a Makefile
        self.run(['qmake', '-qt5'])

        # Run make to build the sources
        self.run(['make', '-j{}'.format(self.project.parallel_build_count)])

        # Now install it
        self.run(['make', 'install', 'INSTALL_ROOT={}'.format(self.installdir)])
