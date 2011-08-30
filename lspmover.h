#ifndef LSPMOVER_H
#define LSPMOVER_H

#include <QObject>
#include <QUrl>
#include <QList>
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
    void parseItem(const QUrl &url);
    void processItems();
    void process();
    void processItem();

private:
    QWebPage *page;
    QUrl baseUrl;
    QList<QUrl> itemUrls;
};

#endif // LSPMOVER_H
