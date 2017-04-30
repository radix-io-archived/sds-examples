#ifndef PHONEBOOK_H
#define PHONEBOOK_H

typedef struct phonebook_s* phonebook;

int phonebook_create(phonebook* pb);

int phonebook_delete(phonebook* pb);

int phonebook_insert(phonebook pb, const char* name, const char* phone);

const char* phonebook_find(phonebook pb, const char* name);

#endif
