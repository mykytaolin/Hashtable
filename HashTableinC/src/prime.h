// We need prime numbers for resizing hashtable. We are creating new hashtable half or twice big as current
// and insert all non-deleted items into it.
// Our new array size should be a prime number roughly double or half the current size.
// To resize up, we double the base size, and find the first larger prime, and to resize down,
// we halve the size and find the next larger prime.

int is_prime(const int x);
int next_prime(int x);