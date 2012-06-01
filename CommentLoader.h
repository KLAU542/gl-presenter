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
#include <QStringList>
#include <QFile>

class CommentLoader
{
public:
	void loadFile(const QString fn, const int pagecount);
	QString readNote(int slide, int line);
	QString getKey(int keynum);
	int getBookmark(int bmnum);
	void setBookmark(int bmnum, int value);
private:
        QFile datafile;
	QStringList* notes;
	QStringList keys;
	QList<int> bookmarks;
	int pagecount;
};
