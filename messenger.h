#ifndef MESSENGER_H
#define MESSENGER_H

#include <QObject>
#include <QPixmap>
#include "chatwrapper.h"



class CMessenger : public QObject
{
    Q_OBJECT
public:
    // создает "поломанный" объект класса CMessenger, никаких исключений не возбуждает
    explicit CMessenger(QObject *parent = nullptr);
    // создает валидный объект класса CMessenger, сам не возбуждает исключений,
    // но может возникнуть исключение runtime_error при инициализировании поля ChatWrapper *CMessenger::_wrapper;
    // коннектит сигналы _wrapper на соответствующие слоты CMessenger
    explicit CMessenger(const QString &name, QObject *parent = nullptr);
    ~CMessenger();

    // при уже проинициализированном поле ChatWrapper *CMessenger::_wrapper очищается память
    // и создается новый объект класса ChatWrapper, при неинициализированном поле создается новый
    // объект класса ChatWrapper с заданным в аргументе метода имени
    // сам не возбуждает исключений, но может возникнуть исключение runtime_error
    // при инициализации поля ChatWrapper *CMessenger::_wrapper
    // коннектит сигналы _wrapper на соответствующие слоты CMessenger
    void set_name(const QString &name);

    // отправляет текстовое сообщение, переданное в виде аргумента метода
    // сам не возбуждает исключений, но может возникнуть исключение runtime_error
    // при использовании семейства методов void send_...(...) у _wrapper
    // при неинициализированном _wrapper отправка игнорируется
    void send_text_message(const QString &message);
    // аналогично CMessenger::send_text_message(...),
    // но отправляет картинку, которая хранится в поле QPixmap CMessenger::_current_media и обуляет это поле
    void send_media_message();

    // возвращает текущую прикрепленную картинку
    // в случае отсутствия оной возвращается пустой объект класса QPixmap
    QPixmap current_media() const;
    // устанавливает текущую прикрепленную картинку
    // возбуждает исключение runtime_error при превышении допустимого размера
    void set_current_media(const QPixmap &picture);

    // возвращает статус экземпляра класса CMessenger (false - invalid, true - valid)
    bool is_valid() const;

signals:
    // сигнал, возбуждаемый при получении нового текстового сообщения
    // коннектится к слоту, обрабатывающему эти сообщения
    // описание сигнатуры сигнала:
    // QString name - имя отправителя
    // QString message - текст сообщения
    // bool is_system - флаг системного сообщения (true - HELLO, BYE, false - CONTENT)
    void new_text_message(QString name, QString message, bool is_system);
    // сигнал, возбуждаемый при получении нового медиа-сообщения
    // коннектится к слоту, обрабатывающему эти сообщения
    // описание сигнатуры сигнала:
    // QString name - имя отправителя
    // QPixmap picture - картинка
    void new_media_message(QString name, QPixmap picture);

private slots:
    // слот, привязанный на соответствующий сигнал _wrapper, просто возбуждает сигнал CMessenger::new_text_message(...)
    void slot_new_text_message(QString name, QString message, bool is_system);
    // слот, привязанный на соответствующий сигнал _wrapper, преобразует картинку из QByteArray в QPixmap
    // и возбуждает сигнал CMessenger::new_media_message(...)
    void slot_new_media_message(QString name, QByteArray barray);

private:
    ChatWrapper *_wrapper;
    QPixmap _current_media;
};

#endif // MESSENGER_H
