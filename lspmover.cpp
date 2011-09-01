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

#include "lspmover.h"

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElementCollection>
#include <QtCore/QRegExp>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtDebug>

const QString outputHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!--\n\
    This is a WordPress eXtended RSS file generated by Live Space Package Mover \n\
    as an export of your blog. It contains information about your blog\'s posts, \n\
    comments, and categories. You may use this file to transfer that content from \n\
    one site to another. This file is not intended to serve as a complete backup \n\
    of your blog.\n\
    \n\
    To import this information into a WordPress blog follow these steps:\n\
    \n\
    1. Log into that blog as an administrator.\n\
    2. Go to Manage > Import in the blog\'s admin.\n\
    3. Choose \"WordPress\" from the list of importers.\n\
    4. Upload this file using the form provided on that page.\n\
    5. You will first be asked to map the authors in this export file to users \n\
       on the blog. For each author, you may choose to map an existing user on \n\
       the blog or to create a new user.\n\
    6. WordPress will then import each of the posts, comments, and categories \n\
       contained in this file onto your blog.\n\
-->\n\
\n\
<!-- generator=\"Live Space Package Mover 0.1\" created=\"%1\"-->\n\
<rss version=\"2.0\"\n\
    xmlns:content=\"http://purl.org/rss/1.0/modules/content/\"\n\
    xmlns:wfw=\"http://wellformedweb.org/CommentAPI/\"\n\
    xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n\
    xmlns:wp=\"http://wordpress.org/export/1.0/\"\n\
>\n\
\n\
<channel>\n\
    <title>%2</title>\n\
    <link>%3</link>\n\
    <description></description>\n\
    <pubDate>%4</pubDate>\n\
    <generator>Live Space Package Mover 0.1</generator>\n\
    <language>en</language>\n\
    <wp:wxr_version>1.0</wp:wxr_version>\n\
";
//need blogTitle, nowTime, blogURL

const QString outputFoot="\n</channel>\n</rss>\n";
const QString outputCategory="\n<wp:category><wp:category_nicename>msn</wp:category_nicename><wp:category_parent></wp:category_parent><wp:cat_name><![CDATA[MSN]]></wp:cat_name></wp:category>\n";
const QString outputEntry="<item>\n\
<title>%1</title>\n\
<link>%2</link>\n\
<pubDate>%3</pubDate>\n\
<dc:creator>%4</dc:creator>\n\
\n\
    <category><![CDATA[MSN]]></category>\n\
\n\
<guid isPermaLink=\"false\"></guid>\n\
<description></description>\n\
<content:encoded><![CDATA[%5]]></content:encoded>\n\
<wp:post_id>%6</wp:post_id>\n\
<wp:post_date>%7</wp:post_date>\n\
<wp:post_date_gmt>%8</wp:post_date_gmt>\n\
<wp:comment_status>open</wp:comment_status>\n\
<wp:ping_status>open</wp:ping_status>\n\
<wp:post_name>%9</wp:post_name>\n\
<wp:status>publish</wp:status>\n\
<wp:post_parent>0</wp:post_parent>\n\
<wp:menu_order>0</wp:menu_order>\n\
<wp:post_type>post</wp:post_type>\n\
%10\n\
</item>\n";
//need entryTitle, entryURL, entryAuthor, category, entryContent, entryId, postDate, pubDate

const QString outputComments="<wp:comment>\n\
<wp:comment_id>%1</wp:comment_id>\n\
<wp:comment_author>%2</wp:comment_author>\n\
<wp:comment_author_email>%3</wp:comment_author_email>\n\
<wp:comment_author_url>%4</wp:comment_author_url>\n\
<wp:comment_author_IP></wp:comment_author_IP>\n\
<wp:comment_date>%5</wp:comment_date>\n\
<wp:comment_date_gmt>%6</wp:comment_date_gmt>\n\
<wp:comment_content>%7</wp:comment_content>\n\
<wp:comment_approved>1</wp:comment_approved>\n\
<wp:comment_type></wp:comment_type>\n\
<wp:comment_parent>0</wp:comment_parent>\n\
</wp:comment>\n";
//need commentId, commentAuthor, commentEmail, commentURL,commentDate,commentContent

int commentId = 1;
int entryId = 1;

LSPMover::LSPMover(QObject *parent) :
QObject(parent)
{
page = new QWebPage(this);
}

void LSPMover::parse(const QUrl &url)
{
    baseUrl = url;
    page->mainFrame()->load(baseUrl);
    connect(page, SIGNAL(loadFinished(bool)),
        this, SLOT(process()));
}

void LSPMover::process()
{
    disconnect(page, SIGNAL(loadFinished(bool)),
        this, SLOT(process()));

    QWebElement title = page->mainFrame()->findFirstElement("h1");
    QWebElementCollection items = page->mainFrame()->findAllElements("a");
    foreach (QWebElement item, items) {
        QString link = item.attribute("href");
        QUrl relativeUrl(link);
        itemUrls.append(baseUrl.resolved(relativeUrl));
    }

    QDateTime dt = QDateTime::currentDateTime();
    blogInfo["blogURL"] = baseUrl;
    blogInfo["nowTime"] = dt;
    blogInfo["blogTitle"] = title.toPlainText().simplified();

    outputFileName = QString("output_") + dt.toString("yyyyMMdd-HHmm") + QString(".xml");

    processItems();
}

void LSPMover::output()
{
    typedef QHash<QString, QVariant> EntryType;
    typedef QHash<QString, QVariant> CommentType;
    QFile output(outputFileName);
    if (output.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&output);
        out << outputHead.arg(blogInfo["nowTime"].toDateTime().toString("yyyy-MM-dd hh:mm")).arg(blogInfo["blogTitle"].toString()).arg(blogInfo["blogURL"].toString()).arg(blogInfo["nowTime"].toDateTime().toString("ddd, dd MMM yyyy hh:mm:ss +0000"));
        out << outputCategory;
        foreach ( EntryType entry, blogEntries) {
            QList<QVariant> comments = entry["comments"].toList();
            QString commentsString;
            foreach ( QVariant tmp, comments) {
                QHash<QString, QVariant> comment = tmp.toHash();
                commentsString += outputComments.arg(commentId).arg(comment["author"].toString()).arg(comment["email"].toString()).arg(comment["url"].toString()).arg(comment["datetime"].toDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(comment["datetime"].toDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(comment["content"].toString());
                commentId += 1;
            }
            QString entryString = outputEntry.arg(entry["title"].toString()).arg(entry["url"].toString()).arg(entry["datetime"].toString()).arg("Meijun Li").arg(entry["content"].toString()).arg(entryId).arg(entry["datetime"].toDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(entry["datetime"].toDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(entry["title"].toString()).arg(commentsString);

            out << entryString;
            entryId += 1;
        }

        out << outputFoot;

        output.close();
    }
}

void LSPMover::processItems()
{
    if (itemUrls.size() > 0) {
        QUrl url = itemUrls.first();
        parseItem(url);
    } else {
        output();
        emit finished();
    }
}

void LSPMover::parseItem(const QUrl &url)
{
    page->mainFrame()->load(url);
    connect(page, SIGNAL(loadFinished(bool)),
        this, SLOT(processItem()));
}

void LSPMover::updateUrl(QWebElement &e, QUrl url, QString tag, QString attribute)
{
    QWebElementCollection items = e.findAll(tag);
    foreach (QWebElement item, items) {
        QString href = item.attribute(attribute);
        if (!href.isEmpty()) {
            QUrl tmpUrl(href);
            if (tmpUrl.isRelative()) {
                item.setAttribute(attribute, url.resolved(tmpUrl).toString());
            }
        }
    }
}

void LSPMover::checkUrl(QWebElement &e, QUrl url)
{
    updateUrl(e, url, "a", "href");
    updateUrl(e, url, "img", "src");
}

void LSPMover::processItem()
{
    disconnect(page, SIGNAL(loadFinished(bool)),
        this, SLOT(processItem()));

    QHash<QString, QVariant> entry;
    entry["url"] = itemUrls.takeFirst();
    QWebElementCollection divs = page->mainFrame()->findAllElements("div");
    foreach (QWebElement div, divs) {
        QString id = div.attribute("id");
        if (id.contains("bp")) {
            QWebElement title = div.findFirst("h2");
            if (title.attribute("id").contains("Title")) {
                entry["title"] = title.toPlainText().simplified();
            }
            QWebElement h5 = div.findFirst("h5");
            if (h5.attribute("id").contains("publish")) {
                entry["datetime"] = QDateTime::fromString(h5.toPlainText().simplified(), "yyyy/M/d H:mm:ss");
            }
            QWebElementCollection bps = div.findAll("div");
            foreach (QWebElement bp, bps) {
                if (bp.attribute("class") == "blogpost") {
                    checkUrl(bp, entry["url"].toUrl());
                    entry["content"] = bp.toInnerXml();
                }
                QList<QVariant> comments;
                if (bp.attribute("class") == "comments") {
                    QWebElementCollection cmts = bp.findAll("div");
                    foreach (QWebElement cmt, cmts) {
                        QHash<QString, QVariant> comment;
                        if (cmt.attribute("id").contains("comment")) {
                            QWebElement h5 = cmt.findFirst("h5");
                            QStringList sl = h5.toPlainText().split("-");
                            if (sl.size() == 2) {
                                comment["author"] = sl[0].simplified();
                                comment["datetime"] = QDateTime::fromString(sl[1].simplified(), "yyyy/M/d H:mm:ss");
                            }
                            QString tmp = cmt.toInnerXml();
                            comment["content"] = tmp.right(tmp.length()-tmp.indexOf("</h5>")-5).simplified(); //a little dirty here
                            comments.append(QVariant(comment));
                        }
                    }
                    entry["comments"] = comments;
                }
            }
            break;
        }
    }
    blogEntries.append(entry);

    processItems();
}
