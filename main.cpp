#include <QtGui/QApplication>

#include "lspmover.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LSPMover *mover = new LSPMover(&a);

    QObject::connect(mover, SIGNAL(finished()),
        &a, SLOT(quit()));

    mover->parse(QUrl("http://www.qihome.org/~liangqi/"));

    return a.exec();
}
