#ifndef CCHATLIB_H
#define CCHATLIB_H

extern "C" {
    #include "chatlib.h"
    Messenger *init_messenger(unsigned port, const char name[]);
    void delete_messenger(Messenger *);
    int validate_name(const char name[]);
    int validate_content(const char content[]);
}

#endif // CCHATLIB_H
