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

#include "BeamerWidget.h"

BeamerWidget::BeamerWidget(const QGLFormat &format, PDFThread *pdfthread, Animator *animator, QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags flags) : GeneralWidget(format, pdfthread,animator,parent,shareWidget,flags)
{
	setWindowTitle(tr("Beamer"));
	setMouseTracking(true);
	initblend=false;
        waitingforcache=false;
	wasanimated=false;
}

BeamerWidget::~BeamerWidget()
{
}

void BeamerWidget::initializeGL() {
}

void BeamerWidget::paintGL() {
	if (!initblend && this->width() == deskRect[1].width() && this->height() == deskRect[1].height()) {
		animator->blendOut();
		initblend=true;
	}
//	printf("Paint Beamer\n");
	float minx,miny,maxx,maxy;
	glViewport(0,0,this->width(),this->height());
	glClear(GL_COLOR_BUFFER_BIT);

	float animationstate = animator->getAnimation();
	if (pdfthread->isCached(animator->getCurrentPage())) {
		if (waitingforcache) {
			animator->restartAnimation();
			animationstate = 0.0;
			waitingforcache = false;
		}
	}
	else {
		waitingforcache = true;
		animationstate = 0.0;
	}

	glPushMatrix();
	if (animator->getMode() == GLP_ZOOM_MODE) {
		double zoomfactor = animator->getZoomFactor();
		glScalef(zoomfactor,zoomfactor,zoomfactor);
		calculateAspects();
		glTranslatef(-animator->getZoomX()*aspectx,-animator->getZoomY()*aspecty,0.0);
	}

	if (!waitingforcache) {
		bool wasupdated = false;
		if (animator->isBlended()) {
			glColor3f(0.0,0.0,0.0);

			minx = -1.0;
			miny = -1.0;
			maxx = 1.0;
			maxy = 1.0;
		}
		else {
			wasupdated = pdfthread->isUpdated(animator->getCurrentPage());
			glEnable(GL_TEXTURE_2D);
			pdfthread->bindPageTexture(animator->getCurrentPage());

			minx = float(this->width() - pdfthread->getWidth(animator->getCurrentPage()))/this->width() - 1.0;
			miny = float(this->height() - pdfthread->getHeight(animator->getCurrentPage()))/this->height() - 1.0;
			maxx = -minx;
			maxy = -miny;
		}


		//	printf("%d %d %d %d\n",this->width(), this->height(), pdfthread->getWidth(0), pdfthread->getHeight(0));
		//	printf("%f %f %f %f\n",minx,miny,maxx,maxy);

		paintPage(minx,miny,maxx,maxy);
		glColor3f(1.0,1.0,1.0);
		glDisable(GL_TEXTURE_2D);
		if (wasupdated) {
			printf("wasUpdated\n");
			glFinish();
			animator->restartAnimation();
			animationstate = 0.0;
		}
	}

	if (animationstate < 1.0) {
		glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (animator->isLastBlended()) {
			glColor4f(0.0,0.0,0.0,1.0-animationstate);
			minx = -1.0;
			miny = -1.0;
			maxx = 1.0;
			maxy = 1.0;
		}
		else {
			glEnable(GL_TEXTURE_2D);
			pdfthread->bindPageTexture(animator->getLastPage());
			glColor4f(1.0,1.0,1.0,1.0-animationstate);

			minx = float(this->width() - pdfthread->getWidth(animator->getLastPage()))/this->width() - 1.0;
			miny = float(this->height() - pdfthread->getHeight(animator->getLastPage()))/this->height() - 1.0;
			maxx = -minx;
			maxy = -miny;
		}
		paintPage(minx,miny,maxx,maxy);
		
		// draw dark outside border, for page format changes
		glDisable(GL_TEXTURE_2D);
		glColor4f(0.0,0.0,0.0,1.0-animationstate);

		glBegin(GL_QUADS);
			glVertex2f(-1.0,-1.0);
			glVertex2f(-1.0, 1.0);
			glVertex2f(minx, 1.0);
			glVertex2f(minx,-1.0);
			
			glVertex2f(maxx,-1.0);
			glVertex2f(maxx, 1.0);
			glVertex2f( 1.0, 1.0);
			glVertex2f( 1.0,-1.0);
			
			glVertex2f(minx,-1.0);
			glVertex2f(minx,miny);
			glVertex2f(maxx,miny);
			glVertex2f(maxx,-1.0);

			glVertex2f(minx,maxy);
			glVertex2f(minx, 1.0);
			glVertex2f(maxx, 1.0);
			glVertex2f(maxx,maxy);
		glEnd();

		glDisable (GL_BLEND);
		glColor3f(1.0,1.0,1.0);

		wasanimated = true;

                if (pdfthread->isCached(animator->getCurrentPage())) {
                    update();
                }
	}
	else {
		if (wasanimated) {
			animator->updatePresenterWidget();
		}
	}
	glPopMatrix();
	checkForGlError("End of PaintGL in BeamerWidget ");
}
