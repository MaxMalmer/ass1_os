/*
 * A implementation of a hash table based on a linked list.
 * 
 * Author : Max Malmer (max@growme.se)
 * Based on: https://gist.github.com/phsym/4605704
 */

/*==========================================Includes and Datatypes======================================*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../includes/hash.h"
#include "../includes/hashtable.h"

/*==========================================Program Start==============================================*/

/*
 * Imported function to calc hash from hash.h.
 * 
 * param: The key that is to be hashed.
 * return: The hash of the key given. 
 */
hash_t ht_calc_hash(char* key) {
	return hash_ssn(key);
}

/* Create a hashtable with capacity 'capacity'
 * and return a pointer to it.
 *
 * param: The max-size of the hash table.
 * return: A empty hash table. 
 */
hashtable_t* ht_create(unsigned int capacity, uint8_t min_range, uint8_t max_range) {
	hashtable_t* hasht = malloc(sizeof(hashtable_t));

	if (!hasht) {
		return NULL;
	}

	if ((hasht->table = malloc(4024*sizeof(hash_elem_t*))) == NULL) {
		free(hasht->table);
		return NULL;
	}
	hasht->capacity = capacity;
	hasht->range_min = min_range;
	hasht->range_max = max_range;
	hasht->e_num = 0;
	unsigned int i;

	for (i = 0; i < 4024; i++) {
		hasht->table[i] = NULL;
	}
	return hasht;
}

/* Store data in the hashtable. If data with the same key are already stored,
 * they are overwritten, and return by the function. Else it return NULL.
 * Return HT_ERROR if there are memory alloc error.
 * 
 * param: The hash table, the hash key and the data to insert.
 * return: The pointer to the data.
 */
void* ht_put(hashtable_t* hasht, char* key, void* data) {

	if (data == NULL) {
		return NULL;
	}
	hash_t h = hash_ssn(key);
	//fprintf(stderr, "elementet är: %d\n", h);

	if (hasht->range_min > h || h > hasht->range_max) {
		return NULL;
	}
	hash_elem_t* e = hasht->table[h];

	while (e != NULL) {

		if (!strcmp(e->key, key)) {
			void* ret = e->data;
			e->data = data;
			return ret;
		}
		e = e->next;
	}

	// Getting here means the key doesn't already exist

	if ((e = malloc(sizeof(hash_elem_t)+strlen(key)+1)) == NULL) {
		return NULL;
	}
	memcpy(e->key, key, 12);
	e->data = data;

	// Add the element at the beginning of the linked list
	e->next = hasht->table[h];
	hasht->table[h] = e;
	hasht->e_num ++;

	return e;
}

/* Retrieve data from the hashtable.
 *
 * param: The hashtable and the key.
 * return: A pointer to the element.
 */
void* ht_get(hashtable_t* hasht, char* key, int* not_found_flag) {
	hash_t h = hash_ssn(key);

	if (hasht->range_min > h || h > hasht->range_max) {
		return NULL;
	}
	hash_elem_t* e = hasht->table[h];

	while (e != NULL) {

		if (!strcmp(e->key, key)) {
			return e->data;
		}
		e = e->next;
	}
	*not_found_flag = 1;
	//fprintf(stderr, "Vi sätter flaggan.\n");
	return (void*)not_found_flag;
}

/* Remove data from the hashtable. Return the data removed from the table
 * so that we can free memory if needed.
 *
 * param: The hash table and the key.
 * return: A pointer to the data.
 */
void* ht_remove(hashtable_t* hasht, char* key) {
	hash_t h = hash_ssn(key);

	if (hasht->range_min > h || h > hasht->range_max) {
		return NULL;
	}
	hash_elem_t* e = hasht->table[h];
	hash_elem_t* prev = NULL;

	while (e != NULL) {

		if (!strcmp(e->key, key)) {
			void* ret = e->data;

			if (prev != NULL) {
				prev->next = e->next;
			} else {
				hasht->table[h] = e->next;
			}
			free(e);
			e = NULL;
			hasht->e_num --;
			return ret;
		}
		prev = e;
		e = e->next;
	}
	void *data = malloc(8);

	if (data == NULL) {
		fprintf(stderr, "Out of memory!\n");
	}
	data = (void*)1;
	return data;
}

/* List keys. k should have length equals or greater than the number of keys 
 *
 * param: The hash table, the keys and the number of keys - 1.
 */
void ht_list_keys(hashtable_t* hasht, char** k, size_t len) {

	if (len < hasht->e_num) {
		return;
	}
	int ki = 0; //Index to the current string in **k
	int i = hasht->capacity;

	while (--i >= 0) {
		hash_elem_t* e = hasht->table[i];

		while (e) {
			k[ki++] = e->key;
			e = e->next;
		}
	}
}

/* List values. v should have length equals or greater 
 * than the number of stored elements 
 * 
 * param: The hash table, list of values and the length of it.
 */
void ht_list_values(hashtable_t* hasht, void** v, size_t len) {

	if (len < hasht->e_num) {
		return;
	}
	int vi = 0; //Index to the current string in **v
	int i = hasht->capacity;

	while (--i >= 0) {
		hash_elem_t* e = hasht->table[i];

		while (e) {
			v[vi++] = e->data;
			e = e->next;
		}
	}
}

/* Iterate through table's elements. 
 *
 * param: The iterator.
 * return: The current element.
 */
hash_elem_t* ht_iterate(hash_elem_it* iterator) {

	while (iterator->elem == NULL) {

		if (iterator->index < 256) {
			iterator->index++;
			iterator->elem = iterator->ht->table[iterator->index];
		} else {
			return NULL;
		}
	}
	hash_elem_t* e = iterator->elem;

	if (e) {
		iterator->elem = e->next;
	}
	return e;
}

/* Iterate through keys.
 *
 * param: The iterator.
 * return: The key.
 */
char* ht_iterate_keys(hash_elem_it* iterator) {
	hash_elem_t* e = ht_iterate(iterator);
	return (e == NULL ? NULL : e->key);
}

/* Iterate through values. 
 *
 * param: The iterator.
 * return: The pointer to the elements.
 */
void* ht_iterate_values(hash_elem_it* iterator) {
	hash_elem_t* e = ht_iterate(iterator);
	return (e == NULL ? NULL : e->data);
}

/* Removes all elements stored in the hashtable.
 * if free_data, all stored datas are also freed.
 * 
 * param: The hash table to clear, data to free.
 */
void ht_clear(hashtable_t* hasht, int free_data) {
	hash_elem_it it = HT_ITERATOR(hasht);
	char* k = ht_iterate_keys(&it);

	while (k != NULL) {
		free_data ? free(ht_remove(hasht, k)) : ht_remove(hasht, k);
		k = ht_iterate_keys(&it);
	}
}

/* Destroy the hash table, and free memory.
 * Data still stored are freed.
 * 
 * param: The hash table.
 */
void ht_destroy(hashtable_t* hasht) {
	ht_clear(hasht, 1); // Delete and free all.
	free(hasht->table);
	free(hasht);
}