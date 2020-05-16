#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatwrapper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void new_message(QString name, QString message, bool is_system);

private slots:
    void on_pb_start_clicked();

    void on_pb_send_clicked();

private:
    Ui::MainWindow *ui;
    ChatWrapper *_chat;

};
#endif // MAINWINDOW_H
