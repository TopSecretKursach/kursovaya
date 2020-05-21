#ifndef MESSENGER_H
#define MESSENGER_H

#include <QObject>
#include <QPixmap>
#include "chatwrapper.h"



class CMessenger : public QObject
{
    Q_OBJECT
public:
    explicit CMessenger(QObject *parent = nullptr);
    explicit CMessenger(const QString &name, QObject *parent = nullptr);
    ~CMessenger();

    void set_name(const QString &name);
//    QString name() const;

    void send_text_message(const QString &message);
    void send_media_message();

    QPixmap current_media() const;
    void set_current_media(const QPixmap &picture);

    bool is_valid() const;

signals:
    void new_text_message(QString name, QString message, bool is_system);
    void new_media_message(QString name, QPixmap picture);

public slots:
    void slot_new_text_message(QString name, QString message, bool is_system);
    void slot_new_media_message(QString name, QByteArray barray);

private:
    ChatWrapper *_wrapper;
    QPixmap _current_media;
};

#endif // MESSENGER_H
