#include "lspmover.h"

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElementCollection>
#include <QtCore/QRegExp>

#include <QtDebug>

LSPMover::LSPMover(QObject *parent) :
    QObject(parent)
{
    page = new QWebPage(this);
}

void LSPMover::parse(const QUrl &url)
{
    qDebug() << "LSPMover::parse()";
    baseUrl = url;
    page->mainFrame()->load(baseUrl);
    connect(page, SIGNAL(loadFinished(bool)),
        this, SLOT(process()));
}

void LSPMover::process()
{
    qDebug() << "LSPMover::process()";
    disconnect(page, SIGNAL(loadFinished(bool)),
        this, SLOT(process()));

    QWebElement title = page->mainFrame()->findFirstElement("h1");
    qDebug() << "blog title=" << title.toPlainText();
    QWebElementCollection items = page->mainFrame()->findAllElements("a");
    foreach (QWebElement item, items) {
        QString link = item.attribute("href");
        qDebug() << "article link=" << link;
        QUrl relativeUrl(link);
        parseItem(baseUrl.resolved(relativeUrl));
        break;
    }
    //emit finished();
}

void LSPMover::parseItem(const QUrl &url)
{
    qDebug() << "LSPMover::parseItem()";
    page->mainFrame()->load(url);
    connect(page, SIGNAL(loadFinished(bool)),
        this, SLOT(processItem()));
}

void LSPMover::processItem()
{
    qDebug() << "LSPMover::processItem()";
    disconnect(page, SIGNAL(loadFinished(bool)),
        this, SLOT(processItem()));


    QWebElementCollection divs = page->mainFrame()->findAllElements("div");
    foreach (QWebElement div, divs) {
        QString id = div.attribute("id");
        if (id.contains("bp")) {
            QWebElement title = div.findFirst("h2");
            if (title.attribute("id").contains("Title"))
                qDebug() << "article title=" << title.toPlainText();
            QWebElement h5 = div.findFirst("h5");
            if (h5.attribute("id").contains("publish"))
                qDebug() << "article datetime=" << h5.toPlainText();
            QWebElementCollection bps = div.findAll("div");
            foreach (QWebElement bp, bps) {
                if (bp.attribute("class") == "blogpost") {
                    qDebug() << "article=" << bp.toPlainText();
                }
                if (bp.attribute("class") == "comments") {
                    QWebElementCollection comments = bp.findAll("div");
                    foreach (QWebElement comment, comments) {
                        if (comment.attribute("id").contains("comment")) {
                            QWebElement h5 = comment.findFirst("h5");
                            QStringList sl = h5.toPlainText().split("-");
                            if (sl.size() == 2) {
                                qDebug() << "comment author=" << sl[0];
                                qDebug() << "comment datetime=" << sl[1];
                            }
                            QString tmp = comment.toInnerXml();
                            qDebug() << "comment content=" << tmp.right(tmp.length()-tmp.indexOf("</h5>")-5); //a little dirty here
                        }
                    }
                }
            }
            break;
        }
    }

    emit finished();
}
