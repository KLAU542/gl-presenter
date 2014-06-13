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

greaterThan(QT_MAJOR_VERSION, 4) {
win32 {
    LIBS += C:\\ProgramData\\KDE\\lib\\libpoppler-qt5.dll.a
}
unix {
    LIBS += -lpoppler-qt5 -lGLU
}
}
else {
win32 {
    LIBS += C:\\ProgramData\\KDE\\lib\\libpoppler-qt4.dll.a
}
unix {
    LIBS += -lpoppler-qt4 -lGLU
}
}

HEADERS += Animator.h \
    GeneralWidget.h \
    PresenterWidget.h \
    BeamerWidget.h \
    CommentLoader.h \
    PDFThread.h
SOURCES += gl-presenter.cpp \
    Animator.cpp \
    GeneralWidget.cpp \
    PresenterWidget.cpp \
    BeamerWidget.cpp \
    CommentLoader.cpp \
    PDFThread.cpp
RESOURCES += gl-presenter.qrc
QT += opengl
OTHER_FILES += DejaVuSans.ttf

binary.files = gl-presenter
binary.path = $$[QT_INSTALL_BINS]

INSTALLS += binary

#CONFIG += debug
exists( gl-presenter_paths.pri ) {
    include (gl-presenter_paths.pri)
}
