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

#include "Animator.h"
#include <stdio.h>

Animator::Animator() {
	pagecount = 0;
	currentpage = 0;
	mode = 0;
	lastpage = 0;
	secondlastpage = 0;

	animationduration = qsettings.value("animator/duration", 400).toInt();
	animationtime.start();
	currentselected = 0;

	zoomx=0.0;
	zoomy=0.0;
	zoomfactor=1.0;

	blended = true;
	lastblended = true;

	horizontallock=false;
	verticallock = false;

	helpoverlay = false;
}

Animator::~Animator() {
}

int Animator::getCurrentPage() {
	return currentpage;
}

int Animator::getNextPage() {
	if (currentpage + 1 >= pagecount) return -1;
	return (currentpage + 1);
}

int Animator::getSecondNextPage() {
	if (currentpage + 2 >= pagecount) return -1;
	return (currentpage + 2);
}

int Animator::getXNextPage(int x) {
	if (currentpage + x < 0) return -1;
	if (currentpage + x >= pagecount) return -1;
	return (currentpage + x);
}

int Animator::getLastPage() {
	return lastpage;
}

int Animator::getSecondLastPage() {
	return secondlastpage;
}

int Animator::getPageCount() {
	return pagecount;
}

void Animator::setPageCount(int i) {
	pagecount = i;
}

void Animator::setCurrentPage(int i) {
    if (i<0 || i>= pagecount) return;
    if (animationtime.elapsed()>float(animationduration) || (!lastblended)) {
        lastblended = blended;

        secondlastpage = lastpage;
        lastpage = currentpage;
    }
    currentpage = i;
    animationtime.restart();
    updateWidgets();
}

void Animator::nextPage() {
	if (currentpage < pagecount - 1) {
		setCurrentPage(currentpage+1);
	}
}

void Animator::prevPage() {
	if (currentpage > 0) {
		setCurrentPage(currentpage-1);
	}
}

void Animator::blendOut() {
	lastblended = blended;
	blended = !blended;

	secondlastpage = lastpage;
	lastpage = currentpage;

	animationtime.restart();
	updateWidgets();
}

bool Animator::isBlended() {
	return blended;
}

bool Animator::isLastBlended() {
	return lastblended;
}

void Animator::setSelected(int i) {
	currentselected = i;
	updatePresenterWidget();
}

int Animator::getSelected() {
	return currentselected;
}

int Animator::getMode() {
	return mode;
}

void Animator::restartAnimation() {
    animationtime.restart();
}

float Animator::getAnimation() {
	if (animationduration == 0) {
		return 2.0; // >= 1.0
	}
        return float(animationtime.elapsed()) / float(animationduration);
}

void Animator::setMode(int mode) {
	int oldmode = this->mode;
	this->mode = mode;
	if (mode == GLP_ZOOM_MODE || oldmode == GLP_ZOOM_MODE) {
		updateWidgets();
	}
	else {
		updatePresenterWidget();
	}
}

void Animator::updateWidgets() {
	foreach (QWidget *widget, QApplication::allWidgets()) {
//		if (mode != GLP_SELECTION_MODE || widget->windowTitle() == QString("Beamer")) {
			widget->update();
//		}
	}
}

void Animator::updatePresenterWidget() {
	foreach (QWidget *widget, QApplication::allWidgets()) {
		if (widget->windowTitle() == QString("Presenter")) {
			widget->update();
/*			switch(mode) {
				case GLP_PRESENTER_MODE:
					widget->setCursor(Qt::BlankCursor);
					break;
				case GLP_SELECTION_MODE:
					widget->setCursor(Qt::PointingHandCursor);
					break;
				case GLP_ZOOM_MODE:
					widget->setCursor(Qt::CrossCursor);
					break;
				case GLP_TWOPAGE_MODE:
					widget->setCursor(Qt::BlankCursor);
					break;
			}*/
		}
	}
}

void Animator::setRowCount(int i) {
	rowcount = i;
}

void Animator::setLineCount(int i) {
	linecount = i;
}

int Animator::getRowCount() {
	return rowcount;
}

int Animator::getLineCount() {
	return linecount;
}

double Animator::getZoomX() {
	return zoomx;
}

double Animator::getZoomY() {
	return zoomy;
}

double Animator::getZoomFactor() {
	return zoomfactor;
}

void Animator::limitzoom() {
	if (zoomfactor < 1.0) zoomfactor = 1.0;
	if (zoomx < -1.0) zoomx = -1.0;
	if (zoomy < -1.0) zoomy = -1.0;
	if (zoomx >  1.0) zoomx = 1.0;
	if (zoomy >  1.0) zoomy = 1.0;
	if (horizontallock) zoomx = 0.0;
	if (verticallock) zoomy = 0.0;
}

void Animator::zoom(double zx, double zy, double zfactor) {
	zoomfactor *= 1.0 + zfactor;
	zoomx += zfactor * zx / zoomfactor;
	zoomy += zfactor * zy / zoomfactor;
	horizontallock=false;
	verticallock = false;
	limitzoom();
	updateWidgets();
//	printf("Animator::zoom: %f,%f,%f\n",zoomfactor,zoomx,zoomy);
}

void Animator::move(double zx, double zy) {
	zoomx -= zx / zoomfactor;
	zoomy -= zy / zoomfactor;
	limitzoom();
	updateWidgets();
//	printf("Animator::move: %f,%f,%f\n",zoomfactor,zoomx,zoomy);
}

void Animator::setZoom(double zx, double zy, double zfactor) {
	zoomfactor = zfactor;
	zoomx = zx;
	zoomy = zy;
	limitzoom();
	updateWidgets();
//	printf("Animator::setZoom: %f,%f,%f\n",zoomfactor,zoomx,zoomy);
}

void Animator::setZoomX(double zfactor) {
	horizontallock=true;
	verticallock = false;
	setZoom(0.0,zoomy,zfactor);
}

void Animator::setZoomY(double zfactor) {
	horizontallock=false;
	verticallock=true;
	setZoom(zoomx,0.0,zfactor);
}

void Animator::resetZoom() {
	zoomx = 0.0;
	zoomy = 0.0;
	zoomfactor = 1.0;
	horizontallock=false;
	verticallock = false;
	updateWidgets();
}
