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

#include "CommentLoader.h"
#include <QTextStream>

void CommentLoader::loadFile(const QString filename, const int pagecount) {
	this->pagecount = pagecount;
	notes = new QStringList[pagecount];
	datafile.setFileName(filename);
	if (datafile.exists()) {
		datafile.open(QIODevice::ReadOnly | QIODevice::Text);
		if (datafile.isOpen()) {
			QTextStream datastream(&datafile);
			int slide = 0;
			while (!datastream.atEnd()) {
				QString line = datastream.readLine();
				if (line.startsWith("slide ")) {
					QStringList params=line.split(" ");
					if (params.count() > 1) {
						slide = params[1].toInt() - 1;
					}
				}
				if (line.startsWith("comment {")) {
					line = datastream.readLine();
					while (!datastream.atEnd() && !line.startsWith("}")) {
						if (slide < pagecount) {
							notes[slide].append(line.trimmed());
						}
						line = datastream.readLine();
					}
				}
				if (line.startsWith("keys {")) {
					line = datastream.readLine();
					while (!datastream.atEnd() && !line.startsWith("}")) {
						keys.append(line.trimmed());
						line = datastream.readLine();
					}
				}
				if (line.startsWith("bookmarks {")) {
					line = datastream.readLine();
					while (!datastream.atEnd() && !line.startsWith("}")) {
						QStringList bmarks=line.split(" ");
						for (int i=0; i<bmarks.size(); i++) {
							bookmarks.append(bmarks[i].toInt() - 1);
						}
						line = datastream.readLine();
					}
				}
			}
		}
	}
}

QString CommentLoader::readNote(int slide, int line) {
	if (slide < pagecount && notes[slide].size()>line) {
		return notes[slide][line];
	}
	return QString();
}

QString CommentLoader::getKey(int keynum) {
	if (keys.size()>keynum) {
		return keys[keynum];
	}
	return QString();
}

int CommentLoader::getBookmark(int bmnum) {
	if (bmnum < bookmarks.size()) {
		return bookmarks[bmnum];
	}
	return -1;
}

void CommentLoader::setBookmark(int bmnum, int value) {
	while (bookmarks.size()<=bmnum) {
		bookmarks.append(-1);
	}
	bookmarks[bmnum] = value;
}
