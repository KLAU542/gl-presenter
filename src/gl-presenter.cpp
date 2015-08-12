//  This file is part of GL-Presenter.
//  Copyright (C) 2010 Klaus Denker <kldenker@unix-ag.uni-kl.de>
//
//  GL-Presenter is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  GL-Presenter is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with GL-Presenter.  If not, see <http://www.gnu.org/licenses/>.

#include <QtGui>
#include "PresenterWidget.h"

#if QT_VERSION >= 0x050000
#include <QCommandLineParser>
#endif

using namespace std;

/*
QString getCliParameter(QString input, QString patternshort, QString patternlong, int &position) {
	if (input.startsWith(patternshort)) {
		if (input.length()==patternshort.length()) {
			position++;
			if (position >= qApp->arguments().size()-1)
				return QString();
			return qApp->arguments()[position];
		} else {
			return input.right(input.length()-patternshort.length());
		}
	} else if (input.startsWith(patternlong)) {
		if (input.length()==patternlong.length()) {
			position++;
			if (position >= qApp->arguments().size()-1)
				return QString();
			return qApp->arguments()[position];
		} else {
			return input.right(input.length()-patternlong.length()-1);
		}
	}
	return QString();
}
*/

void display_usage() {
	QFile readme(":/README.md");
	if (!readme.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QTextStream in(&readme);
	QTextStream out(stdout);
	out << endl;
	while (!in.atEnd()) {
		QString line = in.readLine();
		out << line << endl;
	}
}

int main(int argc, char* argv[])
{
	QCoreApplication::setOrganizationName("gl-presenter");
	QCoreApplication::setOrganizationDomain("unix-ag.uni-kl.de");
	QCoreApplication::setApplicationName("gl-presenter");

	QSettings qsettings;

	QApplication a( argc, argv );
	
	printf("Using Qt version %#x.\n",QT_VERSION);

	// OpenGL initialization
	QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
	if (flags == QGLFormat::OpenGL_Version_None) {
		printf("No OpenGL available.\n");
		printf("Requires at least OpenGL 2.0.\n");
		exit(10);
	}
	int major = 1;
	int minor = 0;
	printf("Supported OpenGL versions: 1.0");
	for (unsigned i = 0; i<32; i++) {
		unsigned value = 1<<i;
		if (flags & value)
			switch(value) {
				case QGLFormat::OpenGL_Version_1_1: printf(", 1.1"); major=1; minor=1; break;
				case QGLFormat::OpenGL_Version_1_2: printf(", 1.2"); major=1; minor=2; break;
				case QGLFormat::OpenGL_Version_1_3: printf(", 1.3"); major=1; minor=3; break;
				case QGLFormat::OpenGL_Version_1_4: printf(", 1.4"); major=1; minor=4; break;
				case QGLFormat::OpenGL_Version_1_5: printf(", 1.5"); major=1; minor=5; break;
				case QGLFormat::OpenGL_Version_2_0: printf(", 2.0"); major=2; minor=0; break;
				case QGLFormat::OpenGL_Version_2_1: printf(", 2.1"); major=2; minor=1; break;
				case QGLFormat::OpenGL_Version_3_0: printf(", 3.0"); major=3; minor=0; break;
				case QGLFormat::OpenGL_Version_3_1: printf(", 3.1"); major=3; minor=1; break;
				case QGLFormat::OpenGL_Version_3_2: printf(", 3.2"); major=3; minor=2; break;
				case QGLFormat::OpenGL_Version_3_3: printf(", 3.3"); major=3; minor=3; break;
				case QGLFormat::OpenGL_Version_4_0: printf(", 4.0"); major=4; minor=0; break;
#if QT_VERSION >= 0x050000
				case QGLFormat::OpenGL_Version_4_1: printf(", 4.1"); major=4; minor=1; break;
				case QGLFormat::OpenGL_Version_4_2: printf(", 4.2"); major=4; minor=2; break;
				case QGLFormat::OpenGL_Version_4_3: printf(", 4.3"); major=4; minor=3; break;
#endif
				case QGLFormat::OpenGL_ES_CommonLite_Version_1_0: printf(", ES 1.0 Common Lite"); break;
				case QGLFormat::OpenGL_ES_Common_Version_1_0: printf(", ES 1.0 Common"); break;
				case QGLFormat::OpenGL_ES_CommonLite_Version_1_1: printf(", ES 1.1 Common Lite"); break;
				case QGLFormat::OpenGL_ES_Common_Version_1_1: printf(", ES 1.1 Common"); break;
				case QGLFormat::OpenGL_ES_Version_2_0: printf(", ES 2.0"); break;
			}
	}
	printf(".\n");
	printf("Using OpenGL version %d.%d.\n",major,minor);
	if (major < 2) {
		printf("Requires at least OpenGL 2.0.\n");
		exit(10);
	}
	QGLFormat glFormat;
	glFormat.setVersion( major, minor );
	if ((major == 3 && minor >= 2) || major > 3) {
		glFormat.setProfile( QGLFormat::CompatibilityProfile ); // Requires >=Qt-4.8.0
//		glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
	}
	// Enable Antialiasing:
//	glFormat.setSampleBuffers( true );
//	glFormat.setSamples( 16 );
	PresenterWidget *pw;
	PDFThread *pdfthread;
	Animator *animator;

	/*
	if (qApp->arguments().size()<=1) {
		display_usage();
		return 1;
	}

	for ( int i = 0; i < qApp->arguments().size(); i++ ) {
		QString s = qApp->arguments()[i];
		if (s.startsWith("-h") || s.startsWith("--help")) {
			display_usage();
			return 0;
		}
		else if (s.startsWith("-c") || s.startsWith("--config")) {
			printf("Showing configuration dialog.\n");
			// TODO: start configuration
			return 0;
		}
		else if (s.startsWith("-e") || s.startsWith("--edit")) {
			printf("Showing editor.\n");
			// TODO: start editor
			return 0;
		}
		else if (i == qApp->arguments().size()-1) {
			break;
		}
		else if (s.startsWith("-a") || s.startsWith("--animation-duration")) {
			QString s2 = getCliParameter(s,"-a","--animation-duration",i);
			printf("Set animation duration to %u milliseconds.\n",s2.toUInt());
			qsettings.setValue("animator/duration", s2.toUInt());
		}
		else if (s.startsWith("-l") || s.startsWith("--comment-lines")) {
			QString s2 = getCliParameter(s,"-l","--comment-lines",i);
			printf("Set comment lines to %u.\n",s2.toUInt());
			// TODO: set comment line count
		}
	}
	*/

#if QT_VERSION >= 0x050000
	QCommandLineParser parser;
	parser.setApplicationDescription("GL-Presenter");
	parser.addHelpOption();
	parser.addVersionOption();

	parser.process(a);
	
	if (qApp->arguments().size()<=1) {
		parser.showHelp();
		return 1;
	}
#endif

	animator = new Animator();
	pdfthread = new PDFThread();
	pdfthread->setAnimator(animator);
	if (!pdfthread->loadFile(qApp->arguments()[qApp->arguments().size()-1])) {
		delete pdfthread;
		return 1;
	}
	
	pw = new PresenterWidget(glFormat,pdfthread, animator);

	pw->showNormal();
	int screen0 = qsettings.value("screens/screen0", 0).toInt();
	QRect deskRect = QApplication::desktop()->screenGeometry( screen0 );
	pw->move(deskRect.x()+100,deskRect.y()+100);
	pw->resize(deskRect.width(),deskRect.height());
	pw->showFullScreen();
	pw->initScreens();

	a.setActiveWindow( pw );
        int rt = a.exec();

	delete pw;
	delete animator;

        pdfthread->quitLoop();
	pdfthread->wait();
	delete pdfthread;
	
	return rt;
}
