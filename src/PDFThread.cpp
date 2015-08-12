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

#include "PDFThread.h"

// TODO: configurable cache size
#define CACHE_SIZE 50 // absolute minimum: 8
#define MAX_ZOOM 4 // zoom cache takes MAX_ZOOM^2 normal page memory

#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif

PDFThread::PDFThread() {
	thumbnailed = 0;
}

void PDFThread::run()
{
	quit = false;
	zoomcachepage = -1;
	zoom_factor = 1.0;
	zoom_x = 0;
	zoom_y = 0;
	zoom_w = 640;
	zoom_h = 480;

	zoom_x_cached = -1;
	zoom_y_cached = -1;
	zoom_w_cached = -1;
	zoom_h_cached = -1;

	zoom_caching = false;

	// start event loop
	while (!quit) {
		renderPages();
		sleep(0.1); // TODO: wait-condition, wake
	}
}

void PDFThread::setAnimator(Animator *animator) {
    this->animator = animator;
}

bool PDFThread::loadFile(QString filename) {
	doc = Poppler::Document::load(filename);
	if (doc == NULL) {
		printf("Error loading PDF file.\n");
		return false;
	}

	doc->setRenderHint(Poppler::Document::Antialiasing);
	doc->setRenderHint(Poppler::Document::TextAntialiasing);
	doc->setRenderHint(Poppler::Document::TextHinting);

	pagecount = doc->numPages();
	printf("Loaded %d pages.\n",pagecount);
	page = new Poppler::Page*[pagecount];
	for (int i=0; i<pagecount; i++) {
		page[i] = doc->page(i);
//		printf("%d: %f,%f\n",i,page[i]->pageSizeF().width(),page[i]->pageSizeF().height());
	}
	thumbtex = new GLuint[pagecount];
	pagetex = new GLuint[CACHE_SIZE];
	pagewidth = new int[pagecount];
	pageheight = new int[pagecount];
        updatedthumb = new bool[pagecount];
        updatedpage = new bool[pagecount];

	comments.loadFile(filename.append(".glp"), pagecount);
	
	for (int i=0; i<pagecount; i++) {
		QSize psize = page[i]->pageSize();
		pagewidth[i]= psize.width();
		pageheight[i]= psize.height();
                updatedthumb[i] = false;
                updatedpage[i] = false;
                thumbimage.append(QImage());
	}
        for (int i=0; i<CACHE_SIZE; i++) {
            pageimage.append(QImage());
        }
        newthumbs = false;
	return true;
}

QImage PDFThread::getPageImage(int i, int width, int height) {
	double factor1 = width / page[i]->pageSizeF().width();
	double factor2 = height / page[i]->pageSizeF().height();
	double factor = (factor1<factor2?factor1:factor2);
	return page[i]->renderToImage(72.0 * factor,72.0 * factor);
}

QImage PDFThread::getPageImageZoom(int i, int width, int height) {
	double factor1 = width / page[i]->pageSizeF().width();
	double factor2 = height / page[i]->pageSizeF().height();
	double factor = (factor1<factor2?factor1:factor2);
	factor *= zoom_factor;
	zoom_x_cached = zoom_x;
	zoom_y_cached = zoom_y;
	zoom_w_cached = zoom_w;
	zoom_h_cached = zoom_h;
	return page[i]->renderToImage(72.0 * factor, 72.0 * factor, zoom_x, zoom_y, zoom_w, zoom_h);
}

void PDFThread::initPages(int width, int height, int twidth, int theight, int rowcount, int linecount) {
	// delete cache
	thumbnailed = 0;
	pagecache.clear();
	for (int i=0; i<pagecount; i++) {
		updatedthumb[i] = false;
		updatedpage[i] = false;
	}

	screenwidth = width;
	screenheight = height;
	thumbwidth = ceil(twidth / rowcount * 0.99);
	thumbheight = ceil(theight / linecount * 0.99);

	for (int i=0; i<pagecount; i++) {
		glGenTextures(1, thumbtex+i);

		double screenfactor1 = double(screenwidth)/double(pagewidth[i]);
		double screenfactor2 = double(screenheight)/double(pageheight[i]);
		double screenfactor = (screenfactor1<screenfactor2?screenfactor1:screenfactor2);
		pagewidth[i]=round(screenfactor*pagewidth[i]);
		pageheight[i]=round(screenfactor*pageheight[i]);
	}

	for (int i=0; i<CACHE_SIZE; i++) {
		glGenTextures(1, pagetex+i);
	}
	glGenTextures(1, &zoomtex);
	glGenTextures(1, &zoomtexold);
}

void PDFThread::initZoom(double aspectx, double aspecty) {
	zoom_factor = animator->getZoomFactor();
	zoom_w = screenwidth;
	zoom_h = screenheight;
	zoom_x = (double)screenwidth * (animator->getZoomX() + 1.0) * 0.5 * aspectx * zoom_factor - (double)screenwidth * 0.5;
	zoom_y = -(double)screenheight * (animator->getZoomY() - 1.0) * 0.5 * aspecty *zoom_factor - (double)screenheight * 0.5;

	// reset cache
//	zoomcachepage = -1;

//	printf("initZoom1 %f,%d,%d,%d,%d\n",zoom_factor,zoom_x,zoom_y,zoom_w,zoom_h);
//	printf("initZoom2 %f,%f,%f,%d,%d\n",animator->getZoomFactor(),animator->getZoomX(),animator->getZoomY(),screenwidth, screenheight);
//	printf("initZoom3 %f,%f\n",aspectx, aspecty);
}

int PDFThread::getPageCount() {
	if (doc == NULL) return 0;
	return pagecount;
}

void PDFThread::bindPageTexture(int i) {
    // get full page from cache
    // TODO: Mutex
    if (animator->getMode() == GLP_ZOOM_MODE && isZoomCached(i)) {
	    glBindTexture(GL_TEXTURE_2D,zoomtex);
	    if (updatedzoomcache) {
			// cache old zoom texture
			glBindTexture(GL_TEXTURE_2D,zoomtexold);
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zoomimage.width(),zoomimage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, zoomimage.bits());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			animator->saveOldZoom();

			glBindTexture(GL_TEXTURE_2D,zoomtex);
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zoomimage.width(),zoomimage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, zoomimage.bits());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		    printf("Zoomed page %d: \t%dx%d\n",i+1,zoomimage.width(),zoomimage.height());

		    updatedzoomcache=false;
	    }
	    return;
    }
    if (pagecache.contains(i)) {
        int cnum = pagecache[i];
        glBindTexture(GL_TEXTURE_2D,pagetex[cnum]);
        if (updatedpage[i]) {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pageimage[cnum].width(),pageimage[cnum].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pageimage[cnum].bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            printf("Page %d: \t%dx%d from %d\n",i+1,pageimage[cnum].width(),pageimage[cnum].height(),cnum);

            updatedpage[i]=false;
        }
    }
    else {
        bindThumbTexture(i);
    }
}

bool PDFThread::bindOldZoomTexture(int i) {
    // get old zoom texture
    if (animator->getMode() == GLP_ZOOM_MODE && wasZoomCached(i)) {
	    glBindTexture(GL_TEXTURE_2D,zoomtexold);
	    return true;
 	}
	return false;
}

void PDFThread::bindThumbTexture(int i) {
    if (i<0 || i>=pagecount || i>=thumbnailed) return;
    if (updatedthumb[i]) {
        glBindTexture(GL_TEXTURE_2D, thumbtex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, thumbimage[i].width(),thumbimage[i].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, thumbimage[i].bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        printf("Thumb %d: \t%dx%d\n",i+1,thumbimage[i].width(),thumbimage[i].height());
        updatedthumb[i]=false;
    }
    glBindTexture(GL_TEXTURE_2D,thumbtex[i]);
}

int PDFThread::getWidth(int i) {
	return pagewidth[i];
}

int PDFThread::getHeight(int i) {
	return pageheight[i];
}

int PDFThread::getMaxWidth() {
	int maxwidth = 0;
	for (int i=0; i<pagecount; i++) {
		if (pagewidth[i]>maxwidth) {
			maxwidth = pagewidth[i];
		}
	}
	return maxwidth;
}

int PDFThread::getMaxHeight() {
	int maxheight = 0;
	for (int i=0; i<pagecount; i++) {
		if (pageheight[i]>maxheight) {
			maxheight = pageheight[i];
		}
	}
	return maxheight;
}

bool PDFThread::isCached(int i) {
    return pagecache.contains(i);
}

bool PDFThread::wasZoomCached(int i) {
	return (
			i == zoomcachepage
			);
}

bool PDFThread::isZoomCached(int i) {
	return (
			i == zoomcachepage &&
			!zoom_caching &&
			zoom_x == zoom_x_cached &&
			zoom_y == zoom_y_cached &&
			zoom_w == zoom_w_cached &&
			zoom_h == zoom_h_cached
			);
}

bool PDFThread::isZoomCached() {
	return isZoomCached(animator->getCurrentPage());
}

bool PDFThread::isUpdated(int i) {
    return updatedpage[i];
}

int PDFThread::getThumbnailed() {
    return thumbnailed;
}

CommentLoader *PDFThread::getComments() {
	return &comments;
}

void PDFThread::cacheZoom(int i) {
//	zoomcachepage = -1;
	zoom_caching = true;

//	zoomimage = getPageImage(i,screenwidth*MAX_ZOOM,screenheight*MAX_ZOOM);
	zoomimage = getPageImageZoom(i,screenwidth,screenheight);
	zoomimage = QGLWidget::convertToGLFormat(zoomimage);

	zoomcachepage = i;
	zoom_caching = false;
	updatedzoomcache = true;
	printf("Loaded Zoom %d: \t%dx%d\n",i+1,zoomimage.width(),zoomimage.height());
}

void PDFThread::renderPages() {
    // Cache: current, next, second next, last, third next, secondlast, 4th next, 5th next, 6th next, 7th next, ....
    // Map: pagenumber -> Cachenumber
    int cachenumbers[CACHE_SIZE];
    cachenumbers[0]=animator->getCurrentPage();
    cachenumbers[1]=animator->getNextPage();
    cachenumbers[2]=animator->getSecondNextPage();
    cachenumbers[3]=animator->getXNextPage(-1);
    cachenumbers[4]=animator->getLastPage();
    cachenumbers[5]=animator->getXNextPage(-2);
    cachenumbers[6]=animator->getXNextPage(3);
    cachenumbers[7]=animator->getSecondLastPage();
    int ncount = 4;
    int pcount = 3;
	for (int i=8;i<CACHE_SIZE;i++) {
		if ((i%3)==1) {
			cachenumbers[i]=animator->getXNextPage(-pcount);
			pcount++;
		}
		else {
			cachenumbers[i]=animator->getXNextPage(ncount);
			ncount++;
		}
	}

    int p = 0;
    while (cachenumbers[p] == -1 || pagecache.contains(cachenumbers[p])) {
        p++;
        if (p >= CACHE_SIZE || p >= pagecount) {
            break;
        }
    }

    if (animator->getMode() == GLP_ZOOM_MODE && !isZoomCached()) { // && animator->getZoomFactor() > 1.000001) {
		cacheZoom(animator->getCurrentPage());
		animator->updateWidgets();
    }

    // load thumbnail, if all visible slides are ready
    // prefer current, next 2 and previous page over thumbnails
	if (thumbnailed < pagecount) { // && p >= 7) {
		int i = thumbnailed;
		thumbimage[i] = getPageImage(i,thumbwidth,thumbheight);
		thumbimage[i] = QGLWidget::convertToGLFormat(thumbimage[i]);
		updatedthumb[i] = true;
		newthumbs = true;
		thumbnailed++;
//		printf("Loaded Thumbnail %d: \t%dx%d\n",i,thumbimage[i].width(),thumbimage[i].height());
		animator->updateWidgets();
		return;
	}
    
    if (p >= CACHE_SIZE || p >= pagecount) {
	return;
    }

    // find page to remove
    int replacepage = -1;
    int replacecache = pagecache.count();
    if (pagecache.count()>=CACHE_SIZE) {
        bool found = true;
        for (QMap<int,int>::const_iterator cit=pagecache.begin(); cit!=pagecache.end(); ++cit) {
            found = false;
            for (int j=0; j<CACHE_SIZE && !found; j++) {
                found = (cit.key() == cachenumbers[j]);
            }
            if (!found) {
                replacepage = cit.key();
                replacecache = cit.value();
            }
        }
		if (replacepage > -1) {
			pagecache.remove(replacepage);
		}
    }

    // load page to cache
    int pnum = cachenumbers[p];
    pageimage[replacecache] = getPageImage(pnum,screenwidth,screenheight);
    pageimage[replacecache] = QGLWidget::convertToGLFormat(pageimage[replacecache]);
    pagecache.insert(pnum,replacecache);
    updatedpage[pnum]=true;
    printf("Loaded Page %d: \t%dx%d to %d\n",pnum+1,pageimage[replacecache].width(),pageimage[replacecache].height(),replacecache);
    animator->updateWidgets();
}

void PDFThread::quitLoop() {
    quit = true;
}
