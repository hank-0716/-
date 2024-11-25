#include "tcpfilesender.h"
#include "tcpfileserver.h"
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>

class FileTransferApp : public QMainWindow {
    Q_OBJECT

public:
    FileTransferApp(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("檔案傳輸應用");

        QWidget *centralWidget = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout;

        QPushButton *serverButton = new QPushButton("啟動伺服器");
        QPushButton *clientButton = new QPushButton("啟動客戶端");

        connect(serverButton, &QPushButton::clicked, this, &FileTransferApp::showServer);
        connect(clientButton, &QPushButton::clicked, this, &FileTransferApp::showClient);

        layout->addWidget(serverButton);
        layout->addWidget(clientButton);

        stackedWidget = new QStackedWidget;
        tcpFileSender = new TcpFileSender(this);
        tcpFileServer = new TcpFileServer(this);

        stackedWidget->addWidget(tcpFileSender);
        stackedWidget->addWidget(tcpFileServer);

        layout->addWidget(stackedWidget);
        centralWidget->setLayout(layout);
        setCentralWidget(centralWidget);
    }

private slots:
    void showServer() {
        stackedWidget->setCurrentWidget(tcpFileServer);
    }

    void showClient() {
        stackedWidget->setCurrentWidget(tcpFileSender);
    }

private:
    QStackedWidget *stackedWidget;
    TcpFileSender *tcpFileSender;
    TcpFileServer *tcpFileServer;
};
