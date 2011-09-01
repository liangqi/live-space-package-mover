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
