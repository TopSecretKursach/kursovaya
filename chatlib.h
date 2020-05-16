//
// Created by w1ckedente on 05.05.2020.
//

#ifndef NEW_MESSENGER_CHATLIB_H
#define NEW_MESSENGER_CHATLIB_H

#define SIGNATURE "MPR\0"
#define SIG_SIZE 4
#define DEFAULT_PORT 5000
#define MAX_NAME_LEN 32
#define MAX_CONTENT_LEN 256

typedef unsigned char byte;

struct sMessenger;
struct sMessagePacket;

//enum eMessageTypes;
typedef enum eMessageTypes {
    HELLO = 1,
    BYE,
    CONTENT
} MessageTypes;

typedef int (*f_send_message)(struct sMessenger *, const char *, enum eMessageTypes);
typedef struct sMessagePacket * (*f_recv_message)(struct sMessenger */*, char *, char **/);


typedef enum eParseErrors {
    CORRUPTED_PACKET = -2,
    INVALID_PACKET = -3
} ParseErrors;

typedef struct sHeader {
    char signature[SIG_SIZE];
//    char name[MAX_NAME_LEN];
    MessageTypes message_type;
} Header;


typedef struct sHelloMessage {
    char name[MAX_NAME_LEN];
} HelloMessage;

typedef struct sByeMessage {
    char name[MAX_NAME_LEN];
} ByeMessage;

typedef struct sContentMessage {
    char name[MAX_NAME_LEN];
    char content[MAX_CONTENT_LEN];
} ContentMessage;

typedef union uContent {
    HelloMessage hello_message;
    ByeMessage bye_message;
    ContentMessage content_message;
} Content;

typedef struct sMessagePacket {
    Header head;
    Content content;
    unsigned short crc;
    unsigned short end;
} MessagePacket;


typedef struct sMessenger {
    char name[MAX_NAME_LEN]; // all fields of this struct is constant,
    int send_socket;         // because instance of Messenger can be use in several threads without mutexes
    int receive_socket;
    struct sockaddr_in *send_addr;
    struct sockaddr_in *recv_addr;

    f_recv_message receiver;
    f_send_message sender;
} Messenger;

Messenger *init_messenger(unsigned port, const char name[]);

void delete_messenger(Messenger *_this);

MessagePacket *receive_message(Messenger *);

//int validate_name(const char name[]) {
//    return 0;
//}
//
//int validate_content(const char content[]) {
//    return 0;
//}

#endif //NEW_MESSENGER_CHATLIB_H
