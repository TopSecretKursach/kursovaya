#include "messenger.h"

CMessenger::CMessenger(QObject *parent) :
    QObject(parent)
  , _wrapper(nullptr)
  , _current_media()
{

}

CMessenger::CMessenger(const QString &name, QObject *parent) :
    QObject(parent)
  , _wrapper(nullptr)
  , _current_media()
{
   _wrapper = new ChatWrapper(name, this);

   connect(_wrapper, &ChatWrapper::new_message, this, &CMessenger::slot_new_text_message);
   connect(_wrapper, &ChatWrapper::new_media_message, this, &CMessenger::slot_new_media_message);
   _wrapper->send_hello();
}

CMessenger::~CMessenger()
{
    if (_wrapper && _wrapper->is_valid()) _wrapper->send_bye();
    if (_wrapper) delete _wrapper;
}

void CMessenger::set_name(const QString &name)
{
   if (_wrapper) delete _wrapper;
   _wrapper = new ChatWrapper(name, this);
   connect(_wrapper, &ChatWrapper::new_message, this, &CMessenger::new_text_message);
   connect(_wrapper, &ChatWrapper::new_message, this, &CMessenger::new_media_message);
   _wrapper->send_hello();
}

void CMessenger::send_text_message(const QString &message)
{
    if (_wrapper)
        _wrapper->send_message(message);
}

void CMessenger::send_media_message()
{
    if (_current_media.isNull())
        return;

    if (_wrapper)
        _wrapper->send_picture(_current_media);
    _current_media = QPixmap();
}

QPixmap CMessenger::current_media() const
{
    return _current_media;
}

void CMessenger::set_current_media(const QPixmap &picture)
{
    if (!_wrapper->validate_image_size(picture))
        throw std::runtime_error("Oversize image");
    _current_media = picture;
}


bool CMessenger::is_valid() const
{
    return !!_wrapper;
}

void CMessenger::slot_new_text_message(QString name, QString message, bool is_system)
{
    emit new_text_message(name, message, is_system);
}

void CMessenger::slot_new_media_message(QString name, QByteArray barray)
{
    QPixmap p;
    p.loadFromData(barray);
    emit new_media_message(name, p);
}






