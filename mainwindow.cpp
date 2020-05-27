#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messenger.h"
#include <QFileDialog>
#include <QPixmap>
#include <QFile>
#include <QMessageBox>
#include "validators.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _messenger(nullptr)
{
    ui->setupUi(this);
    ui->le_name->setValidator(new NameValidator());
    ui->le_message->setValidator(new ContentValidator());
}

MainWindow::~MainWindow()
{
    if (_messenger) delete _messenger;
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{
    QString name = ui->le_name->text();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Warning", "This field must be non-empty");
        return;
    }

    ui->gb_start->setEnabled(false);
    ui->gb_chating->setEnabled(true);
    ui->gb_media->setEnabled(true);
    ui->graphicsView->setScene(new QGraphicsScene());
    try {
        _messenger = new CMessenger(name, this);
    } catch(std::runtime_error &) {
        QMessageBox::warning(this, "Fatal error", "Initialisation failed");
        ui->gb_start->setEnabled(true);
        ui->gb_media->setEnabled(false);
        ui->gb_chating->setEnabled(false);
        return;
    }

    connect(_messenger, &CMessenger::new_text_message, this, &MainWindow::new_text_message);
    connect(_messenger, &CMessenger::new_media_message, this, &MainWindow::new_media_message);
}

void MainWindow::on_pb_send_clicked()
{
    QString message = ui->le_message->text();
    try {
        if (_messenger->current_media().isNull()) {
            if (!message.isEmpty())
                _messenger->send_text_message(message);
        } else {
            _messenger->send_media_message();
        }
    } catch (std::runtime_error &e) {
        QMessageBox::warning(this, "Error", e.what());
    }
    ui->le_message->clear();
}

void MainWindow::on_pb_media_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);
    QPixmap p(file);
    try {
        _messenger->set_current_media(p);
    } catch (std::runtime_error &e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void MainWindow::new_text_message(QString name, QString message, bool is_system)
{
    QListWidgetItem *item = new QListWidgetItem(name + ": " + message);
    if (is_system)
        item->setForeground(QBrush(Qt::yellow));
    ui->listWidget->addItem(item);
}

void MainWindow::new_media_message(QString name, QPixmap picture)
{
    ui->graphicsView->scene()->clear();
    QListWidgetItem *item = new QListWidgetItem(name + " sent picture");
    item->setForeground(QBrush(Qt::green));
    ui->listWidget->addItem(item);
    ui->graphicsView->scene()->addPixmap(picture);
}
