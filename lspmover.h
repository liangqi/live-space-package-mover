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

private:
    QWebPage *page;
    QUrl baseUrl;
    QList<QUrl> itemUrls;

    QHash<QString, QVariant> blogInfo;
    QList< QHash<QString, QVariant> > blogEntries;

    QString outputFileName;
};

#endif // LSPMOVER_H
