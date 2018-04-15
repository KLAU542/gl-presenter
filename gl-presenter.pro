#  This file is part of GL-Presenter.
#  Copyright (C) 2010 Klaus Denker <kldenker@unix-ag.uni-kl.de>
#
#  GL-Presenter is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  GL-Presenter is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with GL-Presenter.  If not, see <http://www.gnu.org/licenses/>.

# QMAKE_CFLAGS_DEBUG += -pg
# QMAKE_CXXFLAGS_DEBUG += -pg
# QMAKE_LFLAGS_DEBUG += -pg

win32 {
    LIBS += C:\\ProgramData\\KDE\\lib\\libpoppler-qt5.dll.a
}
unix {
    LIBS += -lpoppler-qt5 -lGLU
}

INCLUDEPATH += include
HEADERS += include/Animator.h \
    include/GeneralWidget.h \
    include/PresenterWidget.h \
    include/BeamerWidget.h \
    include/CommentLoader.h \
    include/PDFThread.h
SOURCES += src/gl-presenter.cpp \
    src/Animator.cpp \
    src/GeneralWidget.cpp \
    src/PresenterWidget.cpp \
    src/BeamerWidget.cpp \
    src/CommentLoader.cpp \
    src/PDFThread.cpp
RESOURCES += gl-presenter.qrc
QT += opengl
OTHER_FILES += DejaVuSans.ttf

binary.files = gl-presenter
win32 {
	binary.path = $$[QT_INSTALL_BINS]
}
unix {
	binary.path = $$[PREFIX]/usr/bin/
}

INSTALLS += binary

#CONFIG += debug
exists( gl-presenter_paths.pri ) {
    include (gl-presenter_paths.pri)
}
