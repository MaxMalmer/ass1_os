/*
 * A implementation of a hash funtion given by the UmU CS faculty.
 */

/*==========================================Includes and Datatypes======================================*/

#include "../includes/hash.h"

/*==========================================Program Start==============================================*/

/**
 * The hash digestion.
 * 
 * param: The elemt to hash and length of the hash.
 * return: The hashed element.
 */
static hash_t digest(char* ssn, uint32_t len) {
    uint32_t hash = 5381;
    for (uint32_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + (uint32_t)ssn[i];
    }
    return (hash_t) (hash % 256);
}

/**
 * A function that hashes with a given method.
 * 
 * param: The element to hash.
 * return: The hashed element.
 */
hash_t hash_ssn(char* ssn) {
    return digest(ssn, 12);
}
