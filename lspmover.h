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

#ifndef LSPMOVER_H
#define LSPMOVER_H

#include <QObject>
#include <QUrl>
#include <QList>
#include <QHash>
#include <QVariant>
#include <QtWebKit/QWebPage>

class LSPMover : public QObject
{
    Q_OBJECT
public:
    explicit LSPMover(QObject *parent = 0);

signals:
    void finished();

public slots:
    void parse(const QUrl &url);

private slots:
    void parseItem(const QUrl &url);
    void processItems();
    void process();
    void processItem();

private:
    void output();
    void updateUrl(QWebElement &e, QUrl url, QString tag, QString attribute);
    void checkUrl(QWebElement &e, QUrl url);

private:
    QWebPage *page;
    QUrl baseUrl;
    QList<QUrl> itemUrls;

    QHash<QString, QVariant> blogInfo;
    QList< QHash<QString, QVariant> > blogEntries;

    QString outputFileName;
};

#endif // LSPMOVER_H
