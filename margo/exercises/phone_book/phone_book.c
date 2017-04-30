#include "uthash.h"
#include "phone_book.h"

typedef struct phone_entry {
	char name[128];
	char phone[128];
	UT_hash_handle hh;
} phone_entry;

struct phonebook_s {
	phone_entry* root;
};

int phonebook_create(phonebook* pb) {
	*pb = (phonebook)malloc(sizeof(struct phonebook_s));
	(*pb)->root = NULL;
	return 0;
}

int phonebook_delete(phonebook* pb) 
{
	phone_entry* s;
	phone_entry* tmp;
	HASH_ITER(hh, (*pb)->root, s, tmp) {
      HASH_DEL((*pb)->root, s);
      free(s);
  }
	free(*pb);
	*pb = NULL;
	return 0;
}

int phonebook_insert(phonebook pb, const char* name, const char* phone)
{
	phone_entry* entry;
	HASH_FIND_STR(pb->root, name, entry);
	if(entry != NULL) {
			strncpy(entry->phone,phone,128);
	} else {
		entry = (phone_entry*)malloc(sizeof(phone_entry));
		strncpy(entry->name,name,128);
		strncpy(entry->phone,phone,128);
		HASH_ADD_STR(pb->root, name, entry);
	}
	return 0;
}

const char* phonebook_find(phonebook pb, const char* name)
{
	phone_entry* entry;
	HASH_FIND_STR(pb->root, name, entry);
	if(entry == NULL) return NULL;
	return entry->phone;
}

