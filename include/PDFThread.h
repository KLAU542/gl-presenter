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
#include <poppler/qt5/poppler-qt5.h>
#include "Animator.h"
#include "CommentLoader.h"
#include <QThread>
#include <QImage>
#include <QtOpenGL>

class PDFThread : public QThread
{
public:
	PDFThread();
	void run();

	void setAnimator(Animator *animator);
	bool loadFile(QString fn);
	void initPages(int width, int height, int twidth, int theight, int rowcnt, int linecnt);
	void initZoom(double aspectx, double aspecty);
	int getPageCount();
	void bindPageTexture(int i);
	bool bindOldZoomTexture(int i);
	void bindThumbTexture(int i);
	int getWidth(int i);
	int getHeight(int i);
	int getMaxWidth();
	int getMaxHeight();
	bool isCached(int i);
	bool wasZoomCached(int i);
	bool isZoomCached(int i);
	bool isZoomCached();
	bool isUpdated(int i);
	int getThumbnailed();
	CommentLoader *getComments();
	void quitLoop();

	bool newthumbs;
private:
	QImage getPageImage(int i, int width, int height);
	QImage getPageImageZoom(int i, int width, int height);
	void cacheZoom(int i);
	void renderPages();

	Poppler::Document *doc;
	Poppler::Page **page;
	int pagecount;
	int screenwidth;
	int screenheight;
	int thumbwidth;
	int thumbheight;
	GLuint *thumbtex;
	GLuint *pagetex;
	int *pagewidth;
	int *pageheight;
	bool *updatedthumb;
	bool *updatedpage;
	QList<QImage> thumbimage;
	QList<QImage> pageimage;

	QImage zoomimage;
	int zoomcachepage;
	GLuint zoomtex, zoomtexold;
	bool updatedzoomcache;
	double zoom_factor;
	int zoom_x, zoom_y, zoom_w, zoom_h;
	int zoom_x_cached, zoom_y_cached, zoom_w_cached, zoom_h_cached;
	bool zoom_caching;

	Animator *animator;
	QMap<int,int> pagecache;

	CommentLoader comments;

	bool quit;
	int thumbnailed;
};
