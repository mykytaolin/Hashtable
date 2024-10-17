//
// Created by olinm on 18.05.2024.
//

//#def HASHTABLEINC_HASH_TABLE_H
//#define HASHTABLEINC_HASH_TABLE_H

//#endifHTABLEINC_HASH_TABLE_H

typedef struct {
    char* key;
    char* value;
}ht_item; // items - key-value pairs

typedef struct {
    int size_index;
    int size;
    int count;
    ht_item** items;
} ht_hash_table; // stores array of pointers to items, size and how full it is.

void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);