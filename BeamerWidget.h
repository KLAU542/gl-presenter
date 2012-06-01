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
#include "GeneralWidget.h"

class BeamerWidget : public GeneralWidget
{
	Q_OBJECT

public:
	BeamerWidget(const QGLFormat &format, PDFThread *pdfthread, Animator *animator, QWidget *parent=0, const QGLWidget *shareWidget=0, Qt::WFlags flags=0);
	~BeamerWidget();
protected:
	void paintGL();
	void initializeGL();
private:
	bool initblend;
        bool waitingforcache;
	bool wasanimated;
};

