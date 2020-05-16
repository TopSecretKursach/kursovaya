#include "chatwrapper.h"
#include "chatlib.h"
#include <exception>

//ChatWrapper::ChatWrapper(QObject *parent) :
//    QObject(parent)
//  , _messenger(nullptr)
//  , _flag(false)
//  , _recv_thread()
//{
//}

ChatWrapper::ChatWrapper(const QString &name, QObject *parent) :
    QObject(parent)
  , _messenger(init_messenger(DEFAULT_PORT, name.toStdString().c_str()))
  , _flag(true)
  , _recv_thread([=] {
    if (_messenger)
        p_wait_new_message(*this);
    else throw std::runtime_error("Initialisation failed");
})
{
}

ChatWrapper::~ChatWrapper()
{
    _flag = false;
    _recv_thread.join();
    delete_messenger(_messenger);
}

bool ChatWrapper::flag() const
{
    return _flag;
}

void ChatWrapper::set_flag(bool flag)
{
    _flag = flag;
}

Messenger *ChatWrapper::messenger() const
{
    return _messenger;
}

//void ChatWrapper::set_messenger(Messenger *m)
//{
//    _messenger = m;
//}

bool ChatWrapper::is_valid() const
{
    return !!_messenger;
}

void ChatWrapper::send_message(const QString &message)
{
    int c = _messenger->sender(_messenger, message.toStdString().c_str(),
                       CONTENT);
    if (c <= 0)
        throw std::runtime_error("Sending failed");
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
            }
        }
    }
}
