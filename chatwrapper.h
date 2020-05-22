#ifndef CHATWRAPPER_H
#define CHATWRAPPER_H

#include <QObject>
#include <thread>
#include "cchatlib.h"
/*
 * Класс ChatWrapper является оберткой для библиотеки chatlib.h,
 * реализующей функционал чата в локальной сети
 * Принимает при создании своего экземпляра:
 * 	 Имя пользователя (QString)
 *
 * Возбуждает:
 * 	 runtime_error, информация о случившемся исключении содержится в самом ислючении
 * (const char * runtime_exception::what() const)
*/

class ChatWrapper : public QObject
{
    Q_OBJECT
public:
    // принимает имя пользователя, вошедшего в чат, за время существования объекта имя не меняется,
    // запускает поток приема сообщений, устанавливает флаг состояния принимающего потока
    //
    explicit ChatWrapper(const QString &name, QObject *parent = nullptr);
    // освобождает память, останавливает и присоединяет поток, принимающий новые сообщения,
    // сбрасывает флаг состояния этого потока
    ~ChatWrapper();

    // возвращает статус работы принимающего потока
    bool flag() const;

    // возвращает указатель на структуру Messenger, нужен для отладки
    Messenger *messenger() const;

    // возвращает статус класса, в нынешнем виде нужен для отладки
    bool is_valid() const;

    // возвращает статус возможности отправки растрового изображения (false - отправка невозможна, true - возможна)
    bool validate_image_size(const QPixmap &pic);

    // отправляет текстовое сообщение, принимает строку QString, с текстом сообщения
    // возбуждает исключение runtime_error в случае ошибки отправки
    void send_message(const QString &message);
    // отправляет приветственное сообщение, свидетельствующее о подключении к чату (системное)
    // возбуждает исключение runtime_error в случае ошибки отправки
    void send_hello();
    // отправляет сообщение, свидетельствующее об отключении из чата (системное)
    // возбуждает исключение runtime_error в случае ошибки отправки
    void send_bye();
    // отправляет изображение, принимает в аргументы изображение в виде QPixmap
    // возбуждает исключение runtime_error в случае ошибки отправки, или при превышении допустимого размера картинки
    void send_picture(const QPixmap &pic);

signals:
    // сигнал, возбуждаемый при получении нового текстовые сообщения
    // описание сигнатуры: name (QString) - имя отправителя, message - текст сообщения (QString)
    // is_system (bool) - флаг, указывающий статус сообщения (true - HELLO и BYE, false - CONTENT)
    void new_message(QString name, QString message, bool is_system);
    // сигнал, возбуждаемый при получении сообщения MEDIA_CONTENT,
    // описание сигнатуры: name (QString) - имя отправителя, media_bytes (QByteArray) -
    // набор байтов, преобразуемый в QPixmap с помощью QPixmap::loadFromData(const uchar *buf, uint len,
    // const char* format = nullptr, Qt::ImageConversionFlags flags = Qt::AutoColor);
    void new_media_message(QString name, QByteArray media_bytes);

private:
    // статический приватный метод, в котором "крутится" цикл приема новых сообщений,
    // при приеме нового сообщения возбуждает сигнал void ChatWrapper::new_message(QString name, QString message,
    // bool is_system), описанный выше, цикл останавливается при сбрасывании флага bool ChatWrapper::_flag
    static void p_wait_new_message(ChatWrapper &w);
    Messenger *_messenger;
    // флаг, контролирующий работу потока, принимающего новые сообщения
    volatile std::atomic<bool> _flag;
    // поток, принимающий новые сообщения
    std::thread _recv_thread;
};



#endif // CHATWRAPPER_H
