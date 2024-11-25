#include "tcpfilesender.h"
#include <QHostAddress>
//#include <QRegExpValidator>

TcpFileSender::TcpFileSender(QWidget *parent)
    : QDialog(parent)
{
    loadSize = 1024 * 4;
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    clientProgressBar = new QProgressBar;
    clientStatusLabel = new QLabel(QStringLiteral("客戶端就緒"));
    startButton = new QPushButton(QStringLiteral("開始"));
    quitButton = new QPushButton(QStringLiteral("退出"));
    openButton = new QPushButton(QStringLiteral("開檔"));

    ipLineEdit = new QLineEdit(this);
    portLineEdit = new QLineEdit(this);

    startButton->setEnabled(false); // "開始"按鈕在文件選擇前無法使用



    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(openButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(clientProgressBar);
    mainLayout->addWidget(clientStatusLabel);
    mainLayout->addWidget(new QLabel(QStringLiteral("IP 地址:")));
    mainLayout->addWidget(ipLineEdit);
    mainLayout->addWidget(new QLabel(QStringLiteral("端口:")));
    mainLayout->addWidget(portLineEdit);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    setWindowTitle(QStringLiteral("檔案傳送"));

    // 連接信號和槽
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(&tcpClient, SIGNAL(connected()), this, SLOT(startTransfer()));
    connect(&tcpClient, SIGNAL(bytesWritten(qint64)), this, SLOT(updateClientProgress(qint64)));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
}

void TcpFileSender::openFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) startButton->setEnabled(true);
}

void TcpFileSender::start()
{
    QString ipAddress = ipLineEdit->text();
    QString portText = portLineEdit->text();

    // 簡單的IP和端口檢查
    if (ipAddress.isEmpty() || portText.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("錯誤"), QStringLiteral("請輸入有效的 IP 地址和端口號"));
        return;
    }

    bool ok;
    quint16 port = portText.toUShort(&ok);
    if (!ok || port == 0 || port > 65535) {
        QMessageBox::warning(this, QStringLiteral("錯誤"), QStringLiteral("請輸入有效的端口號 (1-65535)"));
        return;
    }

    startButton->setEnabled(false);
    bytesWritten = 0;
    clientStatusLabel->setText(QStringLiteral("連接中..."));
    tcpClient.connectToHost(QHostAddress(ipAddress), port);
}

void TcpFileSender::startTransfer()
{
    localFile = new QFile(fileName);
    if (!localFile->open(QFile::ReadOnly)) {
        QMessageBox::critical(this, QStringLiteral("文件打開失敗"),
                              QStringLiteral("無法打開文件:\n%1").arg(localFile->errorString()));
        return;
    }

    totalBytes = localFile->size();
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    QFileInfo fileInfo(fileName);  // 使用 QFileInfo 避免路徑錯誤
    QString currentFile = fileInfo.fileName();
    sendOut << qint64(0) << qint64(0) << currentFile;
    totalBytes += outBlock.size();

    sendOut.device()->seek(0);
    sendOut << totalBytes << qint64((outBlock.size() - sizeof(qint64) * 2));
    bytesToWrite = totalBytes - tcpClient.write(outBlock);
    clientStatusLabel->setText(QStringLiteral("已連接"));
    clientProgressBar->setMaximum(totalBytes);  // 設定最大進度
    outBlock.resize(0);
}

void TcpFileSender::updateClientProgress(qint64 numBytes)
{
    bytesWritten += (int)numBytes;
    if (bytesToWrite > 0) {
        outBlock = localFile->read(qMin(bytesToWrite, loadSize));
        bytesToWrite -= (int)tcpClient.write(outBlock);
        outBlock.resize(0);
    } else {
        localFile->close();
    }

    clientProgressBar->setMaximum(totalBytes);
    clientProgressBar->setValue(bytesWritten);
    clientStatusLabel->setText(QStringLiteral("已傳送 %1 Bytes").arg(bytesWritten));
}

TcpFileSender::~TcpFileSender()
{
}
