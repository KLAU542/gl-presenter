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
#include "BeamerWidget.h"

class PresenterWidget : public GeneralWidget
{
	Q_OBJECT

public:
	PresenterWidget(const QGLFormat &format, PDFThread *pdfthread, Animator *animator, QWidget *parent=0, const QGLWidget *shareWidget=0, Qt::WFlags flags=0);
	~PresenterWidget();
	void initScreens();
	void swapScreens();
public slots:
	void updateTimes();
	void hideMouseCursor();
	void startTimer();
private:
	void renderCenteredText(QString text, double posx, double posy);
	void drawHelp();
        void drawPage(int pagenumber, float xpos = 0.0, float ypos = 0.0, float width=1.0, float height=1.0, bool thumb=false);
	BeamerWidget *beamerwidget;
        QFont qfont,qfont2,qfont3;
        QFontMetrics qfontmetrics,qfontmetrics3;
	QTimer *timer;
	QTimer *mousetimer;
	QString clockstr, timerstr;
protected:
	void paintComments();
	void paintPresenterMode();
	void paintZoomMode();
	void paintTwoPageMode();
	void paintSelectionMode();
	void paintSelectionModeUpdate();
	void paintGL();
	void initializeGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	QGLFramebufferObject *overviewfbo;
	bool overviewfboinitialized;
	bool doubleexit;
	QPoint oldpos;
	bool zoommode, scrollmode;
};

