#include "chatwrapper.h"
#include <exception>
#include <QPixmap>
#include <QBuffer>
#include <QFile>
#include "cchatlib.h"

ChatWrapper::ChatWrapper(const QString &name, QObject *parent) :
    QObject(parent)
  , _messenger(init_messenger(DEFAULT_PORT, name.toStdString().c_str()))
  , _flag(true)
  , _recv_thread([=] {
    if (_messenger)
        p_wait_new_message(*this);
    else
        throw std::runtime_error("Initialisation failed");
})
{
}

ChatWrapper::~ChatWrapper()
{
    _flag = false;
    _recv_thread.join();
    delete_messenger(_messenger);
}

void ChatWrapper::send_picture(const QPixmap &pic)
{
    if (!validate_image_size(pic))
        throw std::runtime_error("Oversize image");
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    pic.save(&buffer, "PNG");
    int c = _messenger->sender(_messenger, buffer.data().data(), MEDIA_CONTENT);

    if (c <= 0)
        throw std::runtime_error("Sending error");
}

void ChatWrapper::p_wait_new_message(ChatWrapper &w)
{
    if (!w._messenger) {
        throw std::runtime_error("Initialisation failed");
    }
    while (w._flag) {
        MessagePacket *mp = w._messenger->receiver(w._messenger);
        if (mp) {
            switch (mp->head.message_type) {
            case(HELLO):
               emit w.new_message(QString(mp->content.hello_message.name),
                                QString("joined to channel"), true);
                break;

            case (BYE):
                emit w.new_message(QString(mp->content.bye_message.name),
                                   QString("leave the channel"), true);
                break;

            case(CONTENT):
                emit w.new_message(QString(mp->content.content_message.name),
                                   QString(mp->content.content_message.content), false);
                break;

            case(MEDIA_CONTENT):
                QByteArray barray(mp->content.media.media_content, MAX_MEDIA_CONTENT_LEN);
                emit w.new_media_message(QString(mp->content.media.name),
                                       barray);
                break;
            }
        }
    }
}

bool ChatWrapper::flag() const
{
    return _flag;
}

void ChatWrapper::send_hello()
{
   int c = _messenger->sender(_messenger, nullptr, HELLO);

   if (c <= 0)
       throw std::runtime_error("Sending failed");
}

void ChatWrapper::send_bye()
{
    int c = _messenger->sender(_messenger, nullptr, BYE);

    if (c <= 0)
        throw std::runtime_error("Sending failed");
}

Messenger *ChatWrapper::messenger() const
{
    return _messenger;
}

bool ChatWrapper::is_valid() const
{
    return !!_messenger;
}

bool ChatWrapper::validate_image_size(const QPixmap &pic)
{
    return !(pic.size().width() * pic.size().height()  > MAX_MEDIA_CONTENT_LEN);
}

void ChatWrapper::send_message(const QString &message)
{
    int c = _messenger->sender(_messenger, message.toStdString().c_str(),
                       CONTENT);
    if (c <= 0)
        throw std::runtime_error("Sending failed");
}

