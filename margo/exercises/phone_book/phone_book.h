#ifndef PHONEBOOK_H
#define PHONEBOOK_H

<<<<<<< HEAD
/**
 * Definition of the "phonebook" type.
 */
typedef struct phonebook_s* phonebook;

/**
 * Creates a new phonebook object.
 */
int phonebook_create(phonebook* pb);

/**
 * Delete a phonebook object.
 */
int phonebook_delete(phonebook* pb);

/**
 * Inserts a new entry in the phonebook.
 */
int phonebook_insert(phonebook pb, const char* name, const char* phone);

/**
 * Finds an entry in the phonebook. Will return NULL if no entry
 * was found. You do not have to free the returned pointer.
 */
=======
typedef struct phonebook_s* phonebook;

int phonebook_create(phonebook* pb);

int phonebook_delete(phonebook* pb);

int phonebook_insert(phonebook pb, const char* name, const char* phone);

>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7
const char* phonebook_find(phonebook pb, const char* name);

#endif
