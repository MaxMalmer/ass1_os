#ifndef HASH_H_
#define HASH_H_

/*==========================================Header Start======================================*/

#include <inttypes.h>

#define hash_t uint8_t

/**
 * A function that hashes with a given method.
 * 
 * param: The element to hash.
 * return: The hashed element.
 */
hash_t hash_ssn(char* ssn);

#endif /* HASH_H_ */
