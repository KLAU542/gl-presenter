GL-Presenter
============

A multi-screen OpenGL PDF-Presenter.
It automatically uses display 0 as presenter screen and display 1 as beamer screen.
If only one display is available, the presenter screen is shown.

Usage
-----
```
gl-presenter [OPTION] presentation.pdf
```
Options:
```
  -a TIME,  --animation-duration=TIME   Set animation duration to TIME milliseconds.
  -c,       --config                    Show configuration dialog.
  -e,       --edit                      Show editor.
  -h,       --help                      Display this help and exit.
  -l LINES, --comment-lines=LINES       Set number of LINES for comments.
```

Features
--------
* multi-display-support
* preview of next two slides
* fade animation
* clock and timer
* selection mode
* notes
* bookmarks


Keys
----
```
Use the usual keys and mouse buttons for switching slides.
Tab	switch to slide selection
Z	switch to zoom mode
F	zoom to full page
H	fit page horizontally
V	fit page vertically
T	switch to 2-page mode
B	blank out
R	reset timer
?	mouse area help
M	re-enable mouse after command execution
Shift+X	swap screens
Shift+Esc,Shift+Q
	quit/close
F1-F4	execute predefined command
1-0	call bookmark
Ctrl+1	save bookmark

In slide selection:
arrow keys, mouse	move slide cursor
Esc, Tab, RMB		back to presenter mode
Enter, Return, LMB	select slide
```


Build
-----
```
qmake
make
```


Dependencies
------------
* Qt >=5.2
* glu
* poppler-qt5
* dejavu (font)

(Though the code is still compatible with Qt 4.)

### Debian build dependencies
* Qt5: `libqt5-dev libqt5opengl5-dev libglu-dev libpoppler-qt5-dev`
* Qt4: `libqt4-dev libqt4-opengl-dev libglu-dev libpoppler-qt4-dev`

### Windows build dependencies
Install Poppler (bin + dev) using the KDE for Windows installer.

Miscellaneous
-------------
i3 config (tiling window manager):
```
for_window [class="Gl_presenter"] floating enable
```

Notes
-----
Look at sample file in `doc/notes_sample.pdf.glp`
Filename must contain the complete PDF filename.

Originator
----------

Copyright (C) 2010 Klaus Denker `<kldenker@unix-ag.uni-kl.de>`

