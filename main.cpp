/****************************************************************************
**
** Copyright (C) 2011 Liang Qi <cavendish.qi@gmail.com>
**
** This file is part of the Live Space Package Mover project.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** (LICENSE file) along with this program; if not, write to
** the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
** Boston, MA  02111-1307  USA
**
****************************************************************************/

#include <QtGui/QApplication>

#include "lspmover.h"

#include <QtDebug>

#define LSPMOVER_VERSION_STR 0.1

static const char *error = 0;

void showHelp(const char *appName)
{
    fprintf(stderr, "Live Space Package Mover version %s\n", LSPMOVER_VERSION_STR);
    if (error)
        fprintf(stderr, "%s: %s\n", appName, error);

    fprintf(stderr, "Usage: %s <lspurl>\n\n"
            "\n", appName);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (argc != 2) {
        fprintf(stderr, "Wrong parameters!\n");
        showHelp(argv[0]);
        exit(0);
    }

    qDebug() << "argc=" << argc;
    for (int i =0; i < argc; ++i)
        qDebug() << "argv=" << argv[i];

    LSPMover *mover = new LSPMover(&a);

    QObject::connect(mover, SIGNAL(finished()),
        &a, SLOT(quit()));

    mover->parse(QUrl(argv[1]));

    return a.exec();
}
