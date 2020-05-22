#include "chatlib.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int create_receiving_socket(unsigned port, struct sockaddr_in *recv_addr) {
    int recv_sock;
    if ((recv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        return -1;

    int n = 1;
    if (setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, (void *) &n,
                   sizeof(n)) < 0)
        return -1;

    memset(recv_addr, 0, sizeof(*recv_addr));
    recv_addr->sin_family = AF_INET;
    recv_addr->sin_addr.s_addr = htons(INADDR_ANY);
    recv_addr->sin_port = htons(port);

    if (bind(recv_sock, (struct sockaddr *) recv_addr, sizeof(struct sockaddr_in)) < 0)
        return -1;

    return recv_sock;
}

int create_sending_socket(unsigned port, struct sockaddr_in *send_addr) {
    int sock;
    int broadcast_permission = 1;

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        return  -1;

    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcast_permission,
                   sizeof(broadcast_permission)) < 0)
        return -1;


    memset(send_addr, 0, sizeof(*send_addr));

    send_addr->sin_family = AF_INET;
    send_addr->sin_addr.s_addr = INADDR_BROADCAST;
    send_addr->sin_port = htons(port);
    return sock;
}

byte *mp_to_bytes(const MessagePacket *mp) {
    byte *string = (byte *) malloc(sizeof(MessagePacket));
    if (!string)
        return NULL;
    memcpy(string, mp, sizeof(MessagePacket));
    return string;
}

unsigned short crc16(const byte *string) {
    size_t len = sizeof(MessagePacket) - 2 * sizeof(unsigned short);
    unsigned short crc = 0xffff;
    unsigned char i;

    while (len--) {

        crc ^= *string++ << 8;

        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

unsigned short mp_crc16(MessagePacket *mp) {
    byte *string = mp_to_bytes(mp);
    unsigned short crc = crc16(string);
    free(string);
    return crc;
}

MessagePacket *create_message_packet(const char *msg, const char *name, MessageTypes type) {
    MessagePacket *packet = (MessagePacket *) malloc(sizeof(MessagePacket));
    if (!packet)
        return NULL;

    memset(packet, 0x0, sizeof(MessagePacket));
    packet->head.message_type = type;
    strcpy(packet->head.signature, SIGNATURE);

    switch (type) {
        case (HELLO):
            strcpy(packet->content.hello_message.name, name);
            break;

        case (BYE):
            strcpy(packet->content.bye_message.name, name);
            break;

        case (CONTENT):
            strcpy(packet->content.content_message.name, name);
            if (msg) strcpy(packet->content.content_message.content, msg);
            break;

        case(MEDIA_CONTENT):
            strcpy(packet->content.media.name, name);
            memcpy(packet->content.media.media_content, msg, MAX_MEDIA_CONTENT_LEN);
            break;

        default:
            break;
    }

    packet->crc = mp_crc16(packet);

    return packet;
}

int parse_bytes(byte *string, size_t len, MessagePacket *mp) {
    byte *counter = string;

    if (len != sizeof(MessagePacket)) {
        return INVALID_PACKET;
    }

    char signature[SIG_SIZE];
    memcpy(signature, counter, SIG_SIZE);
    counter += SIG_SIZE;

    if (strcmp(signature, SIGNATURE) != 0) {
        return INVALID_PACKET;
    }

    MessageTypes type;
    memcpy(&type, counter, sizeof(MessageTypes));
    counter += sizeof(MessageTypes);


    Content c;
    memcpy(&c, counter, sizeof(Content));
    counter += sizeof(Content);


    unsigned short crc;
    memcpy(&crc, counter, sizeof(unsigned short));

    if (crc != crc16(string)) {
        return CORRUPTED_PACKET;
    }

    strcpy(mp->head.signature, SIGNATURE);
    mp->head.message_type = type;
    memcpy(&mp->content, &c, sizeof(Content));
    mp->crc = crc;
    mp->end = 0xff;
    return 1;
}

MessagePacket *receive_message(Messenger *_this) {
    byte received_bytes[sizeof(MessagePacket)];
    MessagePacket *mp = (MessagePacket *) malloc(sizeof(MessagePacket));
    int code;

    int len = recvfrom(_this->receive_socket, received_bytes, sizeof(MessagePacket), 0,
            NULL, NULL);
    if (len < 0)
        return NULL;

    if ((code = parse_bytes(received_bytes, len, mp)) < 0) {
        return NULL;
    }

    return mp;
}

int send_message(Messenger *_this, const char message[], MessageTypes type) {
    if (type == CONTENT && validate_content(message) < 0) {
        return -1;
    }

    MessagePacket *mp = create_message_packet(message, _this->name, type);
    byte *send_bytes = mp_to_bytes(mp);
    free(mp);

    int c = sendto(_this->send_socket, send_bytes, sizeof(MessagePacket), 0,
                  (struct sockaddr *)_this->send_addr, sizeof(*(_this->send_addr)));

    free(send_bytes);
    return c;
}

int validate_content(const char content[])
{
    if (strlen(content) > MAX_CONTENT_LEN)
        return -1;

    for (size_t i = 0; i < strlen(content); i++) {
        if (!((content[i] >= 'A' && content[i] <= 'Z') || (content[i] >= 'a' && content[i] <= 'z')
                || (content[i] >= '0' && content[i] <= '9') || (content[i] == ',')
                || (content[i] == '.') || (content[i] == ';') || (content[i] == '?')
                || (content[i] == '!') || (content[i] == ' ')))
            return -1;
    }
    return 0;
}

int validate_name(const char name[])
{
    if (strlen(name) > MAX_NAME_LEN)
        return -1;

    for (size_t i = 0; i < strlen(name); i++) {
            if (!((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z')
                    || (name[i] >= '0' && name[i] <= '9')))
                return -1;
        }
        return 0;
}

Messenger *init_messenger(unsigned port, const char name[]) {
    Messenger *m = (Messenger *) malloc(sizeof(Messenger));
    if (!m) return NULL;

    if (strlen(name) == 0)
        return NULL;

    if (validate_name(name) < 0)
        return NULL;

    strcpy(m->name, name);

    m->recv_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    m->send_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

    m->receive_socket = create_receiving_socket(port, m->recv_addr);
    m->send_socket = create_sending_socket(port, m->send_addr);

    if (m->receive_socket < 0 || m->send_socket < 0 ||
            !m->send_addr || !m->recv_addr) {
        free(m);
        return NULL;
    }

    m->receiver = receive_message;
    m->sender = send_message;

    return m;
}

void delete_messenger(Messenger  *_this) {
    if (_this) {
       if (_this->send_addr) free(_this->send_addr);
       if (_this->recv_addr) free(_this->recv_addr);
       free(_this);
    }
}

