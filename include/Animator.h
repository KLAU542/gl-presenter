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
#include <QTime>
#include <QApplication>
#include <QWidget>
#include <QSettings>

#define GLP_PRESENTER_MODE 0
#define GLP_ZOOM_MODE 1
#define GLP_SELECTION_MODE 2
#define GLP_TWOPAGE_MODE 3

class Animator
{
public:
	Animator();
	~Animator();
	int getCurrentPage();
	int getNextPage();
	int getSecondNextPage();
        int getXNextPage(int x);
        int getLastPage();
	int getSecondLastPage();
	int getPageCount();
	void setPageCount(int i);
	void setCurrentPage(int i);
	void nextPage();
	void prevPage();
	void blendOut();
	bool isBlended();
	bool isLastBlended();
	void setSelected(int i);
	int getSelected();
	int getMode();
	void setMode(int mode);
        void restartAnimation();
	float getAnimation();
	void setRowCount(int i);
	void setLineCount(int i);
	int getRowCount();
	int getLineCount();
	double getZoomX(bool old = false);
	double getZoomY(bool old = false);
	double getZoomFactor(bool old = false);
	void zoom(double zx, double zy, double zfactor);
	void move(double zx, double zy);
	void setZoom(double zx, double zy, double zfactor);
	void setZoomX(double zfactor);
	void setZoomY(double zfactor);
	void resetZoom();
	void saveOldZoom();
	QTime time;
        void updateWidgets();
        void updatePresenterWidget();
	bool helpoverlay;
private:
	void limitzoom();
	int currentpage;
	int lastpage;
	int secondlastpage;
	int pagecount;
	int mode;
	QTime animationtime;
	int animationduration;
	bool blended;
	bool lastblended;
	int currentselected;
	int rowcount;
	int linecount;
	double zoomfactor, zoomx, zoomy;
	double zoomfactorold, zoomxold, zoomyold;
	bool horizontallock,verticallock;
	QSettings qsettings;
};

