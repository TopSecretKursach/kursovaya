#ifndef CHATWRAPPER_H
#define CHATWRAPPER_H

#include <QObject>
#include <thread>
#include "chatlib.h"


class ChatWrapper : public QObject
{
    Q_OBJECT
public:
//    explicit ChatWrapper(QObject *parent = nullptr);
    explicit ChatWrapper(const QString &name, QObject *parent = nullptr);
    ~ChatWrapper();

    bool flag() const;
    void set_flag(bool flag);

    Messenger *messenger() const;
//    void set_messenger(Messenger *m);

    bool is_valid() const;

    void send_message(const QString &message);
    void send_hello();
    void send_bye();

signals:
    void new_message(QString name, QString message, bool is_system);

private:
    static void p_wait_new_message(ChatWrapper &w);
    Messenger *_messenger;
    volatile bool _flag;
    std::thread _recv_thread;
};



#endif // CHATWRAPPER_H
