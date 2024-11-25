#include <QApplication>
#include "filetransferapp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    FileTransferApp window;
    window.show();
    return app.exec();
}
