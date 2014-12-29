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

#pragma once
#include "PDFThread.h"
#include "Animator.h"
#include <QGLWidget>
#include <QtOpenGL>

class GeneralWidget : public QGLWidget
{
public:
	GeneralWidget(const QGLFormat &format, PDFThread *pdfthread, Animator *animator, QWidget *parent=0, const QGLWidget *shareWidget=0, Qt::WindowFlags flags=0);
	virtual ~GeneralWidget();
	virtual void swapScreens();
	void calculateAspects();
protected:
	void paintPage(float minx, float miny, float maxx, float maxy);
	void paintOldZoom();
	void checkForGlError(std::string location);
	PDFThread *pdfthread;
	void keyPressEvent(QKeyEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void calculateBeamerAspects();
	Animator *animator;
	double aspectx,aspecty;
	QRect deskRect[2];
	QSettings qsettings;
	int screen0,screen1;
};

