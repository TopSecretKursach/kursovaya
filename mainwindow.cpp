#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chatwrapper.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _chat(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
//    ui->listWidget->clear();
    if (_chat) {
        _chat->send_bye();
        delete _chat;
    }
    delete ui;
}

void MainWindow::new_message(QString name, QString message, bool is_system)
{
    QListWidgetItem *item = new QListWidgetItem(name + ": " + message);
//    ui->listWidget->addItem(name + ": " + message);
//    if (is_system) {
//        ui->listWidget->currentItem()->setForeground(Qt::yellow);
//    }
    if (is_system)
        item->setForeground(QBrush(Qt::yellow));
    ui->listWidget->addItem(item);
}



void MainWindow::on_pb_start_clicked()
{
    QString name = ui->le_name->text();
    ui->gb_start->setEnabled(false);
    ui->gb_chating->setEnabled(true);
    _chat = new ChatWrapper(name);
    connect(_chat, &ChatWrapper::new_message, this, &MainWindow::new_message);
    _chat->send_hello();
}

void MainWindow::on_pb_send_clicked()
{
    QString message = ui->le_message->text();
    if (!message.isEmpty())
    _chat->send_message(message);
}
