#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "prime.h"
#include "hash_table.h"

// Used to mark the bucket which containing deleted values
static ht_item HT_DELETED_ITEM = {NULL, NULL};

static const int HT_PRIME_1 = 151;
static const int HT_PRIME_2 = 163;


static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* i = malloc(sizeof(ht_item));  // allocates a chunk of memory the size of an item
    i->key = strdup(k);  // duplicating a string as key to new chunk of memory
    i->value = strdup(v);  // duplicating string as value to new chunk of memory
    return i;
}

ht_hash_table* ht_new() { //init new hash table
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));

    ht->size = 53;
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*)); // fills allocated
    // memory with NULL bytes (indicated that the bucket is empty)
    return ht;
}

// Deletes the item i
static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}


static ht_hash_table* ht_new_sized(const int size_index) {  //updating our function to support creating hash tables of a certain size.
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht->size_index = size_index;

    const int base_size = 50 << ht->size_index;
    ht->size = next_prime(base_size);

    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}


void ht_del_hash_table (ht_hash_table* ht) { // deleting func which prevent memory leaks
    for (int i; i < ht->size; i++) {  // iterate the hash table and deleting items which not NULL
        ht_item* item = ht->items[i];
        if (item!= NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}


// the variable a is prime number larger than size of alphabet
// we're hashing ASCII strings (alphabet of 128), so we need prime larger than that
static int ht_hash(const char* s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash+= (long)pow(a, len_s - (i+1)) * s[i]; // returns an int which represent the character
        hash = hash % m;
    }
    return (int)hash;
}


static int ht_get_hash(
        const char* s, const int num_buckets, const int attempt //double hashing func for handling collisions
) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;  // adding +1 and hash_b never 0 (prevent inserting into the same bucket)
}


void ht_insert(ht_hash_table* ht, const char* key, const char* value) {  // iterate through indexes until...
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }
    ht_item* item = ht_new_item(key, value);  // find an empty bucket
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    // preventing colliding keys when deleting item
    while (cur_item != NULL) {
        if (cur_item != &HT_DELETED_ITEM) {
            if (strcmp(cur_item->key, key)== 0) {
                // deleting item and insert next one in the next bucket
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }
    ht->items[index] = item;
    ht->count++;  // incrementing to indicate that new item has been added
}


char* ht_search(ht_hash_table* ht, const char* key) {  // searching item through the hashtable
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 0;
    while (item != NULL) {
        if(item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {  // if item key and key in hashtable are the same
                return item->value;  // return value of this key
            }
            index = ht_get_hash(key, ht->size, i);
            item = ht->items[index];
            i++;
        }
    }
    return NULL;
}


void ht_delete(ht_hash_table* ht, const char* key) {
    int index = ht_get_hash(key,ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if(item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {  // if item key and key in hashtable are the same
                ht_del_item(item);  // deleting from hashtable
                ht->items[index] = &HT_DELETED_ITEM;  // pushing to deleted items
            }
        }
        // changing index +1 and searching item we need
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    ht->count--;  // number of items -1
}




// We change ht_new to call ht_new_sized with the default starting size.
ht_hash_table* ht_new() {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}


static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for ( int i; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;

    // To delete new_ht we give it ht's size and items
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->size;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}


// To simplify resizing, we define two small functions for resizing up and down
static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}
