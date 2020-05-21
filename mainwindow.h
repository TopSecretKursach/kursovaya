#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "messenger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_start_clicked();

    void on_pb_send_clicked();

    void on_pb_media_clicked();

public slots:
    void new_text_message(QString name, QString message, bool is_system);
    void new_media_message(QString name, QPixmap picture);

private:
    Ui::MainWindow *ui;
    CMessenger *_messenger;;

};
#endif // MAINWINDOW_H
