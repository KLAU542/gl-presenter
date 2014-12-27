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

#include <GL/glu.h>
#include <QtGui>
#include "GeneralWidget.h"
#include <iostream>

GeneralWidget::GeneralWidget(const QGLFormat &format, PDFThread *pdfthread, Animator *animator, QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags flags) : QGLWidget(format,parent,shareWidget,flags)
{
	this->pdfthread = pdfthread;
	this->animator = animator;
//	setCursor(Qt::BlankCursor);
	screen0 = qsettings.value("screens/screen0", 0).toInt();
	screen1 = qsettings.value("screens/screen1", 1).toInt();
	deskRect[0] = QApplication::desktop()->screenGeometry( screen0 );
	if (QApplication::desktop()->numScreens() >= 2) {
		deskRect[1] = QApplication::desktop()->screenGeometry( screen1 );
	}
}

GeneralWidget::~GeneralWidget()
{
}

void GeneralWidget::paintPage(float minx, float miny, float maxx, float maxy) {
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);
		glVertex2f(minx,miny);
	
		glTexCoord2f(0.0,1.0);
		glVertex2f(minx,maxy);
	
		glTexCoord2f(1.0,1.0);
		glVertex2f(maxx,maxy);
	
		glTexCoord2f(1.0,0.0);
		glVertex2f(maxx,miny);
	glEnd();
}

void GeneralWidget::checkForGlError(std::string location){
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		const char * errString = (const char *) gluErrorString(errCode);
		std::string outString = "[" + location + "] OpenGL Error: " + std::string(errString);
		std::cout << outString << std::endl;
		exit(2);
	}
}

void GeneralWidget::keyPressEvent(QKeyEvent *event) {
	bool handled = false;
	if (animator->getMode() == GLP_SELECTION_MODE) {
//		bool setmouse = false;
		switch(event->key()) {
			case Qt::Key_Right:
			case Qt::Key_Forward:
			case Qt::Key_PageDown:
			case Qt::Key_N:
				if (animator->getSelected() < animator->getPageCount()-1) {
					animator->setSelected(animator->getSelected()+1);
				}
//				setmouse = true;
				handled = true;
				break;
			case Qt::Key_Left:
			case Qt::Key_Back:
			case Qt::Key_PageUp:
			case Qt::Key_Backspace:
			case Qt::Key_P:
				if (animator->getSelected() > 0) {
					animator->setSelected(animator->getSelected()-1);
				}
//				setmouse = true;
				handled = true;
				break;
			case Qt::Key_Down:
				if (animator->getSelected() < animator->getPageCount()-animator->getRowCount()) {
					animator->setSelected(animator->getSelected()+animator->getRowCount());
				}
//				setmouse = true;
				handled = true;
				break;
			case Qt::Key_Up:
				if (animator->getSelected() >= animator->getRowCount()) {
					animator->setSelected(animator->getSelected()-animator->getRowCount());
				}
//				setmouse = true;
				handled = true;
				break;
			case Qt::Key_Space:
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape: // workaround for Logitech Presenter
				animator->setMode(GLP_PRESENTER_MODE);
				animator->setCurrentPage(animator->getSelected());
				handled = true;
				break;
		}
/*		if (setmouse) {
			int rowcount = animator->getRowCount();
			int linecount = animator->getLineCount();
			float xpos = (float(animator->getSelected()%rowcount)+0.5)/float(rowcount);
			float ypos = (float(animator->getSelected()/rowcount)+0.5)/float(linecount);
			int cursorx = deskRect[0].x() + deskRect[0].width()*xpos;
			int cursory = deskRect[0].y() + deskRect[0].height()*ypos;
			//TODO: remove this workaround for not always working setPos
			while (QCursor::pos().x() != cursorx || QCursor::pos().y() != cursory) {
				QCursor::setPos(cursorx, cursory);
			}
			return;
		}*/
		if (handled) {
			return;
		}
	}
	switch(event->key()) {
		case Qt::Key_Q:
		case Qt::Key_Escape:
			if (animator->getMode() == GLP_PRESENTER_MODE) {
				// TODO: are your sure ?
				if (event->modifiers()&Qt::ShiftModifier) {
					QApplication::exit(0);
				}
			}
			else {
				animator->setMode(GLP_PRESENTER_MODE);
			}
			handled=true;
			break;
		case Qt::Key_Forward:
		case Qt::Key_Right:
		case Qt::Key_Down:
		case Qt::Key_Space:
		case Qt::Key_PageDown:
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_N:
			animator->nextPage();
			handled=true;
			break;
		case Qt::Key_Back:
		case Qt::Key_Left:
		case Qt::Key_Up:
		case Qt::Key_PageUp:
		case Qt::Key_Backspace:
		case Qt::Key_P:
			animator->prevPage();
			handled=true;
			break;
		case Qt::Key_Home:
			animator->setCurrentPage(0);
			handled=true;
			break;
		case Qt::Key_End:
			animator->setCurrentPage(animator->getPageCount()-1);
			handled=true;
			break;
		case Qt::Key_R:
			animator->time.restart();
			handled=true;
			break;
		case Qt::Key_Tab:
		case Qt::Key_F5:
			if (animator->getMode() == GLP_SELECTION_MODE) {
				animator->setMode(GLP_PRESENTER_MODE);
			}
			else {
				animator->setSelected(animator->getCurrentPage());
				animator->setMode(GLP_SELECTION_MODE);

				// set mouse position
/*				int rowcount = animator->getRowCount();
				int linecount = animator->getLineCount();
				float xpos = (float(animator->getSelected()%rowcount)+0.5)/float(rowcount);
				float ypos = (float(animator->getSelected()/rowcount)+0.5)/float(linecount);
				int cursorx = deskRect[0].x() + deskRect[0].width()*xpos;
				int cursory = deskRect[0].y() + deskRect[0].height()*ypos;
				//TODO: remove this workaround for not always working setPos
				while (QCursor::pos().x() != cursorx || QCursor::pos().y() != cursory) {
					QCursor::setPos(cursorx, cursory);
				}
				update();*/
			}
			handled=true;
			break;
		case Qt::Key_Z:
			if (animator->getMode() == GLP_ZOOM_MODE) {
				animator->setMode(GLP_PRESENTER_MODE);
			}
			else {
				animator->setMode(GLP_ZOOM_MODE);
				
				// set mouse position
//				QRect deskRect = QApplication::desktop()->screenGeometry( 0 );
//				QCursor::setPos(deskRect.x() + deskRect.width()*0.5, deskRect.y() + deskRect.height()*0.5);
			}
			handled=true;
			break;
		case Qt::Key_T:
			if (animator->getMode() == GLP_TWOPAGE_MODE) {
				animator->setMode(GLP_PRESENTER_MODE);
			}
			else {
				animator->setMode(GLP_TWOPAGE_MODE);
			}
			handled=true;
			break;
		case Qt::Key_B:
		case Qt::Key_Period:
			animator->blendOut();
			handled = true;
			break;
		case Qt::Key_F:
			animator->resetZoom();
			animator->setMode(GLP_ZOOM_MODE);
			handled = true;
			break;
		case Qt::Key_H:
			calculateBeamerAspects();
			animator->setZoomX(1.0/aspectx);
			animator->setMode(GLP_ZOOM_MODE);
			handled = true;
			break;
		case Qt::Key_V:
			calculateBeamerAspects();
			animator->setZoomY(1.0/aspecty);
			animator->setMode(GLP_ZOOM_MODE);
			handled = true;
			break;
		case Qt::Key_M:
			foreach (QWidget *widget, QApplication::allWidgets()) {
				if (widget->windowTitle() == QString("Presenter")) {
					widget->setMouseTracking(true);
				}
			}
			handled = true;
		case Qt::Key_X:
			if (event->modifiers()&Qt::ShiftModifier) {
				swapScreens();
			}
		case Qt::Key_1:
		case Qt::Key_2:
		case Qt::Key_3:
		case Qt::Key_4:
		case Qt::Key_5:
		case Qt::Key_6:
		case Qt::Key_7:
		case Qt::Key_8:
		case Qt::Key_9:
		case Qt::Key_0:
			if (event->modifiers()&Qt::ControlModifier) {
				pdfthread->getComments()->setBookmark(event->key() - Qt::Key_1, animator->getCurrentPage());
			}
			else {
				int bmkey = event->key() - Qt::Key_1;
				if (event->key() == Qt::Key_0) bmkey=9;
				int bmvalue = pdfthread->getComments()->getBookmark(bmkey);
				animator->setCurrentPage(bmvalue);
				if (bmvalue >= 0) {
					printf("Jump to Bookmark %d -> %d\n", bmkey + 1, bmvalue + 1);
				}
			}
			handled=true;
			break;
		case Qt::Key_F1:
		case Qt::Key_F2:
		case Qt::Key_F3:
		case Qt::Key_F4:
			{
			QStringList progstr = pdfthread->getComments()->getKey(event->key() - Qt::Key_F1).split(' ');
			
			// set mouse position
			QRect deskRectX;
			if (QApplication::desktop()->numScreens() >= 2) {
				deskRectX = deskRect[1];
			}
			else {
				deskRectX = deskRect[0];
			}
			int cursorx = deskRectX.x() + deskRectX.width()*0.5;
			int cursory = deskRectX.y() + deskRectX.height()*0.5;
			//TODO: remove this workaround for not always working setPos
//			while (QCursor::pos().x() != cursorx || QCursor::pos().y() != cursory) {
				QCursor::setPos(cursorx, cursory);
//			}

			QString progname = progstr.takeFirst();
			if (progname != "") {
				setMouseTracking(false);
				printf("%s",progname.toLocal8Bit().data());
				for (int i=0; i<progstr.size(); i++) {
					printf(" %s", progstr[i].toLocal8Bit().data());
				}
				printf("\n");
				QProcess *myProcess = new QProcess(this);
				myProcess->start(progname,progstr);
				handled = true;
			}
			break;
			}
		case Qt::Key_Question:
			animator->helpoverlay = !animator->helpoverlay;
			animator->updatePresenterWidget();
			handled = true;
			break;
	}
	if (!handled) {
		QGLWidget::keyPressEvent(event);
	}
}

void GeneralWidget::mouseMoveEvent(QMouseEvent *event) {
	int x = QCursor::pos().x();
	int y = QCursor::pos().y();
	bool changed = false;

//	printf("Desktop: %d,%d %dx%d\n",deskRect.x(),deskRect.y(),deskRect.width(),deskRect.height());
//	printf("Mouse: %d,%d\n", x, y);

	if (x < deskRect[0].x()) {
		x = deskRect[0].x();
		changed = true;
	}
	if (y < deskRect[0].y()) {
		y = deskRect[0].y();
		changed = true;
	}
	if (x >= deskRect[0].x() + deskRect[0].width()) {
		x = deskRect[0].x() + deskRect[0].width() - 1;
		changed = true;
	}
	if (y >= deskRect[0].y() + deskRect[0].height()) {
		y = deskRect[0].y() + deskRect[0].height() - 1;
		changed = true;
	}

	if (changed) {
		//TODO: remove this workaround for not always working setPos
//		while (QCursor::pos().x() != x || QCursor::pos().y() != y) {
			QCursor::setPos(x, y);
//		}
//		printf("Set Mouse: %d,%d\n", x, y);
	}
}

void GeneralWidget::calculateAspects() {
	double aspectpage = double(pdfthread->getWidth(animator->getCurrentPage()))/
			double (pdfthread->getHeight(animator->getCurrentPage()));
	double aspectscreen = double(width()) / double(height());
	if (aspectscreen>aspectpage) {
		aspectx = aspectpage/aspectscreen;
		aspecty = 1.0;
	}
	else {
		aspecty = aspectscreen/aspectpage;
		aspectx = 1.0;
	}
}

void GeneralWidget::calculateBeamerAspects() {
	QWidget *wid = this;
	foreach (QWidget *widget, QApplication::allWidgets()) {
		if (widget->windowTitle() == QString("Beamer")) {
			wid = widget;
		}
	}
	double aspectpage = double(pdfthread->getWidth(animator->getCurrentPage()))/
			double (pdfthread->getHeight(animator->getCurrentPage()));
	double aspectscreen = double(wid->width()) / double(wid->height());
	if (aspectscreen>aspectpage) {
		aspectx = aspectpage/aspectscreen;
		aspecty = 1.0;
	}
	else {
		aspecty = aspectscreen/aspectpage;
		aspectx = 1.0;
	}
}

void GeneralWidget::swapScreens() {
	if (QApplication::desktop()->numScreens() >= 2) {
		QRect temp = deskRect[0];
		deskRect[0] = deskRect[1];
		deskRect[1] = temp;
		int tmpint;
		tmpint = screen1;
		screen1 = screen0;
		screen0 = tmpint;
		qsettings.setValue("screens/screen0", screen0);
		qsettings.setValue("screens/screen1", screen1);
	}
}
