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

#include "PresenterWidget.h"
#include <QtGui>

#define MOUSE_HIDE_TIMEOUT 2000

PresenterWidget::PresenterWidget(const QGLFormat &format, PDFThread *pdfthread, Animator *animator, QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags flags) : GeneralWidget(format,pdfthread,animator,parent,shareWidget,flags),
qfont("dejavu"),
qfont2("dejavu"),
qfont3("dejavu"),
qfontmetrics(qfont),
qfontmetrics3(qfont3)
{
	setWindowTitle(tr("Presenter"));
	setMouseTracking(true);
	deskRect[0] = QApplication::desktop()->screenGeometry( 0 );

	qfont.setPixelSize(deskRect[0].height()/13.5);
	qfont2.setPixelSize(deskRect[0].height()/40.0);
	qfont3.setPixelSize(deskRect[0].height()/20.0);

        // init font metric
        qfontmetrics = QFontMetrics(qfont);
        qfontmetrics3 = QFontMetrics(qfont3);

	overviewfbo = NULL;
	overviewfboinitialized = false;
	beamerwidget=NULL;
	doubleexit = false;
	zoommode = false;
	scrollmode = false;

	printf("screens: %d\n",QApplication::desktop()->numScreens());
	if (QApplication::desktop()->numScreens() >= 2) {
		beamerwidget = new BeamerWidget(format, pdfthread, animator, 0, this);
		if (!beamerwidget->isSharing()) {
			printf("Widget is not sharing context\n");
			exit(2);
			return;
		}
		deskRect[1] = QApplication::desktop()->screenGeometry( 1 );
		beamerwidget->showNormal();
		beamerwidget->move(deskRect[1].x()+100,deskRect[1].y()+100);
		beamerwidget->resize(deskRect[1].width(),deskRect[1].height());
	}
}

PresenterWidget::~PresenterWidget()
{
	if (beamerwidget != NULL) {
		delete beamerwidget;
	}
	if (overviewfbo != NULL) {
		delete overviewfbo;
	}
	delete timer;
}

void PresenterWidget::initializeGL() {
	int referencescreen;
	if (QApplication::desktop()->numScreens() >= 2) {
		referencescreen = 1;
	}
	else {
		referencescreen = 0;
	}
	animator->setPageCount(pdfthread->getPageCount());
	animator->setRowCount((int)ceil(sqrt(pdfthread->getPageCount()*float(width())/float(height())/(float(pdfthread->getMaxWidth())/float(pdfthread->getMaxHeight())))-0.00001));
	animator->setLineCount((int)ceil(float(pdfthread->getPageCount())/float(animator->getRowCount())-0.00001));
//	printf("rowcount: %d screenar: %f slidear: %f\n",animator->getRowCount(),float(width())/float(height()),float(pdfthread->getMaxWidth())/float(pdfthread->getMaxHeight()));
	
	pdfthread->initPages(deskRect[referencescreen].width(), deskRect[referencescreen].height(), deskRect[0].width(), deskRect[0].height(), animator->getRowCount(), animator->getLineCount());

	overviewfbo = new QGLFramebufferObject(width(),height(),QGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGB);
	
	updateTimes();

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateTimes()));
	QTimer::singleShot(2000 - QTime::currentTime().msec(), this, SLOT(startTimer()));
	
	mousetimer = new QTimer(this);
	connect(mousetimer, SIGNAL(timeout()), this, SLOT(hideMouseCursor()));
	mousetimer->start(MOUSE_HIDE_TIMEOUT);

	if (beamerwidget!=NULL) {
		beamerwidget->showFullScreen();
	}
	QCursor::setPos(deskRect[0].x() + deskRect[0].width()*0.5, deskRect[0].y() + deskRect[0].height()*0.5);
	checkForGlError("End of InitializeGL in PresenterWidget ");
        printf("Initialized OpenGL\n");
}

void PresenterWidget::startTimer() {
	timer->start(1000);
        printf("Timer started\n");
	pdfthread->start(QThread::LowestPriority);
	if (QApplication::desktop()->numScreens() < 2) {
		animator->blendOut();
	}
	animator->time.start();
	updateTimes();
}

void PresenterWidget::updateTimes() {
	timerstr = QString::fromUtf8("+").append(QTime((animator->time.elapsed()/3600000)%24,(animator->time.elapsed()/60000)%60,(animator->time.elapsed()/1000)%60,animator->time.elapsed()%1000).toString());
	clockstr = QString::fromUtf8("⌚").append(QTime::currentTime().toString());
	if (animator->getMode() == GLP_PRESENTER_MODE) {
		update();
	}
}

void PresenterWidget::hideMouseCursor() {
	if (hasMouseTracking()) {
		setCursor(Qt::BlankCursor);
	}
	else {
		setCursor(Qt::ArrowCursor);
	}
}

void PresenterWidget::renderCenteredText(QString text, double posx, double posy) {
        this->renderText(posx - float(qfontmetrics3.width(text))/float(width()), posy - float(qfontmetrics3.xHeight())/float(height()), 0.0, text, qfont3);
}

void PresenterWidget::drawHelp() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0,0.0,0.0,0.75);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);
		glVertex2f(-1.0,-1.0);

		glTexCoord2f(0.0,1.0);
		glVertex2f(-1.0, 1.0);

		glTexCoord2f(1.0,1.0);
		glVertex2f( 1.0, 1.0);

		glTexCoord2f(1.0,0.0);
		glVertex2f( 1.0,-1.0);
	glEnd();
	glColor4f(0.3333,0.6667,1.0,1.0);
	glDisable(GL_BLEND);

	switch(animator->getMode()) {
		case GLP_PRESENTER_MODE:
			glBegin(GL_LINES);
				glVertex2f(-5.0/9.0,    -1.0);
				glVertex2f(-5.0/9.0,     1.0);
				glVertex2f(-1.0/9.0,    -1.0);
				glVertex2f(-1.0/9.0,     1.0);
				glVertex2f( 1.0/3.0,    -1.0);
				glVertex2f( 1.0/3.0,     1.0);
				glVertex2f( 1.0/3.0, 1.0/3.0);
				glVertex2f(     1.0, 1.0/3.0);
				glVertex2f( 1.0/3.0,-1.0/3.0);
				glVertex2f(     1.0,-1.0/3.0);
				glVertex2f( 1.0/3.0,-2.0/3.0);
				glVertex2f(     1.0,-2.0/3.0);
			glEnd();
			renderCenteredText(  QString("previous"),-7.0/9.0,     0.0);
			renderCenteredText(      QString("zoom"),-3.0/9.0,     0.0);
			renderCenteredText(      QString("next"), 1.0/9.0,     0.0);
			renderCenteredText( QString("two pages"), 2.0/3.0, 2.0/3.0);
			renderCenteredText( QString("selection"), 2.0/3.0,     0.0);
			if (animator->isBlended()) {
				renderCenteredText( QString("blend in"), 2.0/3.0,-1.0/2.0);
			} else {
				renderCenteredText( QString("blend out"), 2.0/3.0,-1.0/2.0);
			}
			renderCenteredText(QString("reset timer"), 2.0/3.0,-5.0/6.0);
			break;
		case GLP_ZOOM_MODE:
			glBegin(GL_LINES);
				glVertex2f(-7.0/9.0, 1.0 - 2.0/9.0 * float(width()) / float(height()));
				glVertex2f(    -1.0, 1.0 - 2.0/9.0 * float(width()) / float(height()));
				glVertex2f( 7.0/9.0,                                             -1.0);
				glVertex2f( 7.0/9.0,                                              1.0);
				glVertex2f(-7.0/9.0,                                             -1.0);
				glVertex2f(-7.0/9.0,-1.0 + 2.0/9.0 * float(width()) / float(height()));
				glVertex2f(-7.0/9.0, 1.0 - 2.0/9.0 * float(width()) / float(height()));
				glVertex2f(-7.0/9.0,                                              1.0);
				glVertex2f(-7.0/9.0,-1.0 + 2.0/9.0 * float(width()) / float(height()));
				glVertex2f(    -1.0,-1.0 + 2.0/9.0 * float(width()) / float(height()));
			glEnd();
			renderCenteredText( QString("close"), -8.0/9.0, 1.0 - 1.0/9.0 * float(width()) / float(height()));
			renderCenteredText(  QString("fit"),  -8.0/9.0, -1.0 + 1.0/9.0 * float(width()) / float(height()));
			renderCenteredText(  QString("zoom"),  8.0/9.0, 0.0);
			renderCenteredText(QString("scroll"),-1.0/18.0, 0.0);
			break;
		case GLP_SELECTION_MODE:
			renderCenteredText(QString("select page"),0.0, 0.0);
			break;
		case GLP_TWOPAGE_MODE:
			glBegin(GL_LINES);
				glVertex2f(-7.0/9.0,                                              1.0);
				glVertex2f(-7.0/9.0, 1.0 - 2.0/9.0 * float(width()) / float(height()));
				glVertex2f(-7.0/9.0, 1.0 - 2.0/9.0 * float(width()) / float(height()));
				glVertex2f(    -1.0, 1.0 - 2.0/9.0 * float(width()) / float(height()));
				glVertex2f(-1.0/2.0,                                             -1.0);
				glVertex2f(-1.0/2.0,                                              1.0);
				glVertex2f(     0.0,                                             -1.0);
				glVertex2f(     0.0,                                              1.0);
				glVertex2f( 1.0/2.0,                                             -1.0);
				glVertex2f( 1.0/2.0,                                              1.0);
			glEnd();
			renderCenteredText(          QString("close"),-8.0/9.0, 1.0 - 1.0/9.0 * float(width()) / float(height()));
			renderCenteredText(QString("second previous"),-3.0/4.0,                                              0.0);
			renderCenteredText(       QString("previous"),-1.0/4.0,                                              0.0);
			renderCenteredText(           QString("next"), 1.0/4.0,                                              0.0);
			renderCenteredText(    QString("second next"), 3.0/4.0,                                              0.0);
			break;
	}
	glColor4f(1.0,1.0,1.0,1.0);
}

void PresenterWidget::drawPage(int pagenumber, float xpos, float ypos, float width, float height, bool thumb) {
        if ((!pdfthread->isCached(pagenumber) || thumb) && pdfthread->getThumbnailed()<=pagenumber) {
            return;
	}
	glEnable(GL_TEXTURE_2D);
	// draw thumb if only thumb available or texture update would break animation
        if (thumb || (pdfthread->isUpdated(pagenumber) && animator->getAnimation()<1.0 && QApplication::desktop()->numScreens() >= 2)) {
            pdfthread->bindThumbTexture(pagenumber);
        }
        else {
            pdfthread->bindPageTexture(pagenumber);
        }

	float factor1 = this->width()*width / pdfthread->getWidth(pagenumber);
	float factor2 = this->height()*height / pdfthread->getHeight(pagenumber);
	float factor;
	if (factor1 < factor2) {
		factor = factor1;
	}
	else {
		factor = factor2;
	}


	float minx = float(this->width() - factor*pdfthread->getWidth(pagenumber))/this->width() - 1.0;
	float miny = float(this->height() - factor*pdfthread->getHeight(pagenumber))/this->height() - 1.0;
	float maxx = -minx;
	float maxy = -miny;

	minx += xpos;
	maxx += xpos;
	miny += ypos;
	maxy += ypos;

//	printf("%d %d %d %d\n",width(), height(), pdfthread->getWidth(0), pdfthread->getHeight(0));
//	printf("%f %f %f %f\n",minx,miny,maxx,maxy);

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
	glDisable(GL_TEXTURE_2D);
}

void PresenterWidget::paintComments() {
	for (int l=0; l<10; l++) {
		QString line = pdfthread->getComments()->readNote(animator->getCurrentPage(),l);
                this->renderText(-0.99, -1.0/3.0 - 1.0/20.0 - float(l)/15.0, 0.0, line, qfont2);
	}
}

void PresenterWidget::paintPresenterMode() {
	if (animator->isBlended()) {
		glColor3f(0.25,0.25,0.25);
	}
	drawPage(animator->getCurrentPage(),-0.34,0.34,0.66,0.66);
	glColor3f(1.0,1.0,1.0);
	if (animator->getNextPage() > 0) {
		drawPage(animator->getNextPage(),0.675,0.675,0.325,0.325);
	}
	if (animator->getSecondNextPage() > 1) {
		drawPage(animator->getSecondNextPage(),0.675,0.005,0.325,0.325);
	}

	paintComments();

	glColor3f(0.0,0.0,0.0);
	glBegin(GL_QUADS);
		glVertex2f(0.33,-1.0);
		glVertex2f(0.33,-0.34);
		glVertex2f(1.0,-0.34);
		glVertex2f(1.0,-1.0);
	glEnd();
	glColor3f(1.0,1.0,1.0);

	QString positionstr = QString::number(animator->getCurrentPage() + 1);
	positionstr.append("/");
	positionstr.append(QString::number(animator->getPageCount()));

//	font->FaceSize(height()/12.0);

        this->renderText(2.0/3.0 - float(qfontmetrics.width(positionstr))/float(width()), -1.0/3.0 - 2.0/11.0, 0.0, positionstr, qfont);
        this->renderText(2.0/3.0 - float(qfontmetrics.width(QString::fromUtf8("++00:00:00")))/float(width()), -1.0/3.0 - 2.0/11.0 - 3.0/12, 0.0, timerstr, qfont);
	glColor3f(0.5,0.5,0.5);
        this->renderText(2.0/3.0 - float(qfontmetrics.width(QString::fromUtf8("⌚⌚00:00:00")))/float(width()), -1.0/3.0 - 2.0/11.0 - 5.0/12, 0.0, clockstr, qfont);

	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINES);
		glVertex2f(0.3333,-1.0);
		glVertex2f(0.3333, 1.0);
		glVertex2f(-1.0,-0.3333);
		glVertex2f( 1.0,-0.3333);
	glEnd();
}

void PresenterWidget::paintZoomMode() {
	if (animator->isBlended()) {
		glColor3f(0.25,0.25,0.25);
	}
	glPushMatrix();
	double zoomfactor = animator->getZoomFactor();
	glScalef(zoomfactor,zoomfactor,zoomfactor);
	calculateAspects();
	double zx = animator->getZoomX()*aspectx;
	double zy = animator->getZoomY()*aspecty;
	glTranslatef(-zx,-zy,0.0);
	drawPage(animator->getCurrentPage());
	glPopMatrix();
	glColor3f(1.0,1.0,1.0);
}

void PresenterWidget::paintTwoPageMode() {
	if (animator->isBlended()) {
		glColor3f(0.25,0.25,0.25);
	}
	float pagewidth = float(pdfthread->getWidth(animator->getCurrentPage())) / pdfthread->getHeight(animator->getCurrentPage()) / width() * height(); 
	if (pagewidth > 0.5) {
		pagewidth = 0.5;
	}
	drawPage(animator->getCurrentPage(),-pagewidth,0.0,pagewidth,1.0);
	if (animator->getNextPage() > 0) {
		pagewidth = float(pdfthread->getWidth(animator->getNextPage())) / pdfthread->getHeight(animator->getNextPage()) / width() * height(); 
		if (pagewidth > 0.5) {
			pagewidth = 0.5;
		}
		drawPage(animator->getNextPage(),pagewidth,0.0,pagewidth,1.0);
	}
	glColor3f(1.0,1.0,1.0);
}

void PresenterWidget::paintSelectionMode() {
	int rowcount = animator->getRowCount();
	int linecount = animator->getLineCount();
	float pagesizex = 1.0 / float(rowcount);
	float pagesizey = 1.0 / float(linecount);
	float xpos, ypos;
	for (int i=0; i<pdfthread->getPageCount(); i++) {
		glColor3f(0.5,0.5,0.5);
		xpos = (float(i%rowcount)+0.5)/float(rowcount) * 2.0 - 1.0;
		ypos = -(float(i/rowcount)+0.5)/float(linecount) * 2.0 + 1.0;
                drawPage(i,xpos,ypos,pagesizex*0.99,pagesizey*0.99,true);
	}
	glColor3f(1.0,1.0,1.0);
}

void PresenterWidget::paintSelectionModeUpdate() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,overviewfbo->texture());
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);
		glVertex2f(-1.0,-1.0);

		glTexCoord2f(0.0,1.0);
		glVertex2f(-1.0,1.0);

		glTexCoord2f(1.0,1.0);
		glVertex2f(1.0,1.0);

		glTexCoord2f(1.0,0.0);
		glVertex2f(1.0,-1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	int rowcount = animator->getRowCount();
	int linecount = animator->getLineCount();
	float pagesizex = 1.0 / float(rowcount);
	float pagesizey = 1.0 / float(linecount);
	float xpos, ypos;
	
	int i = animator->getSelected();
	glColor3f(1.0,1.0,1.0);
	xpos = (float(i%rowcount)+0.5)/float(rowcount) * 2.0 - 1.0;
	ypos = -(float(i/rowcount)+0.5)/float(linecount) * 2.0 + 1.0;
        drawPage(i,xpos,ypos,pagesizex*0.99,pagesizey*0.99,true);
}

void PresenterWidget::paintGL() {
	if (pdfthread->newthumbs) {
		pdfthread->newthumbs = false;
		overviewfboinitialized = false;
	}
	if (!overviewfboinitialized) {
		overviewfbo->bind();
		glViewport(0,0,width(),height());
		glClear(GL_COLOR_BUFFER_BIT);
		paintSelectionMode();
		overviewfbo->release();
		overviewfboinitialized = true;
	}

//	printf("Paint Presenter\n");
	glViewport(0,0,width(),height());
	glClear(GL_COLOR_BUFFER_BIT);
	switch(animator->getMode()) {
		case GLP_PRESENTER_MODE:
			paintPresenterMode();
			break;
		case GLP_ZOOM_MODE:
			paintZoomMode();
			break;
		case GLP_SELECTION_MODE:
			paintSelectionModeUpdate();
			break;
		case GLP_TWOPAGE_MODE:
			paintTwoPageMode();
			break;
	}
	if (animator->helpoverlay) {
		drawHelp();
	}
	checkForGlError("End of PaintGL in PresenterWidget ");
}

void PresenterWidget::mousePressEvent(QMouseEvent *event) {
	bool handled = false;
	float x = float(event->x()) / width();
	float y = float(event->y()) / height();
	switch (animator->getMode()) {
		case GLP_PRESENTER_MODE:
/*			if (x <= 1.0/9.0 && y <= 1.0/9.0 * width() / height()) {
				if (doubleexit) {
					QApplication::exit(0);
					return;
				}
				else {
					doubleexit = true;
				}
			}
			else {
				doubleexit = false;
			}*/
			if (x <= 2.0/9.0) {
				animator->prevPage();
			}
			else if (x <= 4.0/9.0) {
				animator->setMode(GLP_ZOOM_MODE);
			}
			else if (x <= 6.0/9.0) {
				animator->nextPage();
			}
			else {
				if (y <= 1.0/3.0) {
					animator->setMode(GLP_TWOPAGE_MODE);
				}
				else if (y <= 2.0/3.0) {
					animator->setMode(GLP_SELECTION_MODE);
				}
				else if (y <= 5.0/6.0) {
					animator->blendOut();
				}
				else {
					animator->time.restart();
				}
			}
			handled = true;
			break;
		case GLP_ZOOM_MODE:
			zoommode = false;
			scrollmode = false;
			if (x <= 1.0/9.0) {
				if (y <= 1.0/9.0 * width() / height()) {
					animator->setMode(GLP_PRESENTER_MODE);
				}
				else if (y <= 1.0 - 1.0/9.0 * width() / height()) {
					scrollmode = true;
				}
				else {
					animator->resetZoom();
					zoommode = false;
					scrollmode = false;
				}
			}
			else if (x <= 8.0/9.0) {
				scrollmode = true;
			}
			else {
				zoommode = true;
			}
			oldpos = event->pos();
			break;
		case GLP_SELECTION_MODE:
			if (event->buttons()==Qt::LeftButton) { 
				int rowcount = animator->getRowCount();
				int linecount = animator->getLineCount();
				int pagenum = (int)floor(x*rowcount) + (int)floor(y*linecount) * rowcount;
				if (pagenum >= animator->getPageCount()) {
					pagenum = animator->getPageCount()-1;
				}
	//			printf("click %f,%f page %d\n",x,y,pagenum);
				animator->setCurrentPage(pagenum);
				animator->setMode(GLP_PRESENTER_MODE);
				handled = true;
			}
			if (event->buttons()==Qt::RightButton) { 
				animator->setMode(GLP_PRESENTER_MODE);
				handled = true;
			}
			break;
		case GLP_TWOPAGE_MODE:
			if (x <= 1.0/9.0 && y <= 1.0/9.0 * width() / height()) {
				animator->setMode(GLP_PRESENTER_MODE);
			}
			else if (x <= 1.0/4.0) {
				if (animator->getCurrentPage()-2 < 0) {
					animator->setCurrentPage(0);
				}
				else {
					animator->setCurrentPage(animator->getCurrentPage()-2);
				}
			}
			else if (x <= 2.0/4.0) {
				animator->setCurrentPage(animator->getCurrentPage()-1);
			}
			else if (x <= 3.0/4.0) {
				animator->nextPage();
			}
			else {
				if (animator->getCurrentPage()+2 >= animator->getPageCount()) {
					animator->setCurrentPage(animator->getPageCount()-1);
				}
				else {
					animator->setCurrentPage(animator->getSecondNextPage());
				}
			}
			break;
	}
	if (!handled) {
		GeneralWidget::mousePressEvent(event);
	}
}

void PresenterWidget::mouseMoveEvent(QMouseEvent *event) {
	mousetimer->stop();
	mousetimer->start(MOUSE_HIDE_TIMEOUT);
	setCursor(Qt::ArrowCursor);

	float x = float(event->x()) / width();
	float y = float(event->y()) / height();
	int rowcount,linecount,pagenum;
	switch (animator->getMode()) {
		case GLP_SELECTION_MODE:
			rowcount = animator->getRowCount();
			linecount = animator->getLineCount();
			pagenum = (int)floor(x*rowcount) + (int)floor(y*linecount) * rowcount;
			if (pagenum < animator->getPageCount()) {
				animator->setSelected(pagenum);
			}
			break;
		case GLP_ZOOM_MODE:
			if (event->buttons() & Qt::LeftButton) {
				if (scrollmode) {
					calculateAspects();
					animator->move(double(event->x()-oldpos.x())/width()/aspectx * 2.0, -double(event->y()-oldpos.y())/height()/aspecty * 2.0);
				}
				else if (zoommode) {
					animator->zoom(0.0, 0.0, -double(event->y()-oldpos.y())/500.0);
				}
				oldpos = event->pos();
			}
			break;
	}
	// trap mouse on first screen
	GeneralWidget::mouseMoveEvent(event);
}

void PresenterWidget::wheelEvent(QWheelEvent *event) {
	if (animator->getMode() == GLP_ZOOM_MODE) {
		calculateAspects();
		animator->zoom((double(event->x())/width() * 2.0 - 1.0)/aspectx, -(double(event->y())/height() * 2.0 - 1.0)/aspecty, double(event->delta())/1000.0);
	}
}

void PresenterWidget::initScreens() {
	if (overviewfbo != NULL)
		delete overviewfbo;
	overviewfbo = new QGLFramebufferObject(width(),height(),QGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGB);
	overviewfboinitialized = false;
}

void PresenterWidget::swapScreens() {
	printf("swap screens ...\n");
	if (QApplication::desktop()->numScreens() >= 2) {
        	pdfthread->quitLoop();
		pdfthread->wait();

		QRect temp = deskRect[0];
		deskRect[0] = deskRect[1];
		deskRect[1] = temp;
		beamerwidget->swapScreens();

		beamerwidget->showNormal();
		beamerwidget->move(deskRect[1].x()+100,deskRect[1].y()+100);
		beamerwidget->resize(deskRect[1].width(),deskRect[1].height());
		beamerwidget->showFullScreen();
		beamerwidget->calculateAspects();

		showNormal();
		move(deskRect[0].x()+100,deskRect[0].y()+100);
		resize(deskRect[0].width(),deskRect[0].height());
		showFullScreen();
		calculateAspects();

		QCursor::setPos(deskRect[0].x() + deskRect[0].width()*0.5, deskRect[0].y() + deskRect[0].height()*0.5);
	
		qfont.setPixelSize(deskRect[0].height()/13.5);
		qfont2.setPixelSize(deskRect[0].height()/40.0);
		qfont3.setPixelSize(deskRect[0].height()/20.0);
        	
		// init font metric
	        qfontmetrics = QFontMetrics(qfont);
        	qfontmetrics3 = QFontMetrics(qfont3);

		// reset PDF pages and load in new Beamer size
		pdfthread->initPages(deskRect[1].width(), deskRect[1].height(), deskRect[0].width(), deskRect[0].height(), animator->getRowCount(), animator->getLineCount());
		QTimer::singleShot(2000 - QTime::currentTime().msec(), this, SLOT(startTimer()));
		
		delete overviewfbo;
		overviewfbo = new QGLFramebufferObject(width(),height(),QGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGB);
		overviewfboinitialized = false;
	}
}
