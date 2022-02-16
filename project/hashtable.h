#ifndef HASHTABLE_H_
#define HASHTABLE_H_

/*==========================================Header Start======================================*/

#include <stdlib.h>
#include <string.h>

//Hashtable element structure
typedef struct hash_elem_t {
	struct hash_elem_t* next; // Next element in case of a collision
	void* data;	// Pointer to the stored element
	char key[]; 	// Key of the stored element
} hash_elem_t;

//Hashtabe structure
typedef struct {
	unsigned int capacity;	// Hashtable capacity (in terms of hashed keys)
	unsigned int range_min; // Max range of the hash_t 
	unsigned int range_max; // Min range of the hash_t
	unsigned int e_num;	// Number of element currently stored in the hashtable
	hash_elem_t** table;	// The table containaing elements
} hashtable_t;

//Structure used for iterations
typedef struct {
	hashtable_t* ht; 	// The hashtable on which we iterate
	unsigned int index;	// Current index in the table
	hash_elem_t* elem; 	// Curent element in the list
} hash_elem_it;

//char err_ptr;
//void* HT_ERROR = &err_ptr; // Data pointing to HT_ERROR are returned in case of error

// Inititalize hashtable iterator on hashtable 'ht'
#define HT_ITERATOR(ht) {ht, 0, ht->table[0]}

/*
 * Imported function to calc hash from hash.h.
 * 
 * param: The key that is to be hashed.
 * return: The hash of the key given. 
 */
uint8_t ht_calc_hash(char* key);

/* Create a hashtable with capacity 'capacity'
 * and return a pointer to it.
 *
 * param: The max-size of the hash table.
 * return: A empty hash table. 
 */
hashtable_t* ht_create(unsigned int capacity, uint8_t min_range, uint8_t max_range);

/* Store data in the hashtable. If data with the same key are already stored,
 * they are overwritten, and return by the function. Else it return NULL.
 * Return HT_ERROR if there are memory alloc error.
 * 
 * param: The hash table, the hash key and the data to insert.
 * return: The pointer to the data.
 */
void* ht_put(hashtable_t* hasht, char* key, void* data);

/* Retrieve data from the hashtable.
 *
 * param: The hashtable and the key.
 * return: A pointer to the element.
 */
void* ht_get(hashtable_t* hasht, char* key, int* not_found_flag);

/* Remove data from the hashtable. Return the data removed from the table
 * so that we can free memory if needed.
 *
 * param: The hash table and the key.
 * return: A pointer to the data.
 */
void* ht_remove(hashtable_t* hasht, char* key);

/* List keys. k should have length equals or greater than the number of keys 
 *
 * param: The hash table, the keys and the number of keys - 1.
 */
void ht_list_keys(hashtable_t* hasht, char** k, size_t len);

/* List values. v should have length equals or greater 
 * than the number of stored elements 
 * 
 * param: The hash table, list of values and the length of it.
 */
void ht_list_values(hashtable_t* hasht, void** v, size_t len);

/* Iterate through table's elements. 
 *
 * param: The iterator.
 * return: The current element.
 */
hash_elem_t* ht_iterate(hash_elem_it* iterator);

/* Iterate through keys.
 *
 * param: The iterator.
 * return: The key.
 */
char* ht_iterate_keys(hash_elem_it* iterator);

/* Iterate through values. 
 *
 * param: The iterator.
 * return: The pointer to the elements.
 */
void* ht_iterate_values(hash_elem_it* iterator);

/* Removes all elements stored in the hashtable.
 * if free_data, all stored datas are also freed.
 * 
 * param: The hash table to clear, data to free.
 */
void ht_clear(hashtable_t* hasht, int free_data);

/* Destroy the hash table, and free memory.
 * Data still stored are freed.
 * 
 * param: The hash table.
 */
void ht_destroy(hashtable_t* hasht);

#endif /* HASHTABLE_H_ */