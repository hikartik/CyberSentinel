// update.h
#ifndef UPDATE_H
#define UPDATE_H

#include <stddef.h>

// Downloads and updates the signature database from an online source.
void update_signatures(void);

// Returns the signature database (an array of strings) and sets the count.
char **get_signature_db(size_t *count);

#endif // UPDATE_H

