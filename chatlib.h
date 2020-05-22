
#ifndef NEW_MESSENGER_CHATLIB_H
#define NEW_MESSENGER_CHATLIB_H

#define SIGNATURE "MPR\0"
#define SIG_SIZE 4
#define DEFAULT_PORT 5000
#define MAX_NAME_LEN 32
#define MAX_CONTENT_LEN 256
#define MAX_MEDIA_CONTENT_LEN 4096

typedef unsigned char byte;

struct sMessenger;
struct sMessagePacket;

typedef enum eMessageTypes {
    HELLO = 1,
    BYE,
    CONTENT,
    MEDIA_CONTENT
} MessageTypes;

typedef int (*f_send_message)(struct sMessenger *, const char *, enum eMessageTypes);
typedef struct sMessagePacket * (*f_recv_message)(struct sMessenger */*, char *, char **/);


typedef enum eParseErrors {
    CORRUPTED_PACKET = -2,
    INVALID_PACKET = -3
} ParseErrors;

// структура общего для всех типов сообщений заголовка
typedef struct sHeader {
    char signature[SIG_SIZE];
    MessageTypes message_type;
} Header;


// содержание системного hello-сообщения
typedef struct sHelloMessage {
    char name[MAX_NAME_LEN];
} HelloMessage;

// содержание системного bye-сообщения
typedef struct sByeMessage {
    char name[MAX_NAME_LEN];
} ByeMessage;

// содержание текстового сообщения
typedef struct sContentMessage {
    char name[MAX_NAME_LEN];
    char content[MAX_CONTENT_LEN];
} ContentMessage;

// содержание медиа сообщения
typedef struct sMediaContentMessage {
    char name[MAX_NAME_LEN];
    char media_content[MAX_MEDIA_CONTENT_LEN];
} MediaContentMessage;

// набор содержаний сообщений различных типов
typedef union uContent {
    HelloMessage hello_message;
    ByeMessage bye_message;
    ContentMessage content_message;
    MediaContentMessage media;
} Content;

// структура пакета сообщения
typedef struct sMessagePacket {
    Header head;
    Content content;
    unsigned short crc;
    unsigned short end;
} MessagePacket;

// структура, упрощающая работу функций приема и отправки с необходимыми данными
typedef struct sMessenger {
    char name[MAX_NAME_LEN];
    int send_socket;
    int receive_socket;
    struct sockaddr_in *send_addr;
    struct sockaddr_in *recv_addr;

    f_recv_message receiver;
    f_send_message sender;
} Messenger;

// возвращает готовый к работе указатель на структуру sMesenger (aka Messenger),
// в случае ошибки инициализации какого-либо из полей возвращает NULL,
// при передаче пустого имени возвращает NULL
// при передаче невалидного имени возвращает NULL
// при передаче занятого порта возвращает NULL
Messenger *init_messenger(unsigned port, const char name[]);

// освобождает память, выделенную init_messenger(...)
void delete_messenger(Messenger *_this);
// приверяет имя на валидность, в случае невалидности имени
// вернет -1, иначе 0
int validate_name(const char name[]);
// аналогично validate_name, только для сообщений
int validate_content(const char content[]);
#endif //NEW_MESSENGER_CHATLIB_H
