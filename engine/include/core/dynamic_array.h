#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

// Default initial capacity if none specified
#define DYNAMIC_ARRAY_DEFAULT_CAPACITY 16

// Growth factor when expanding
#define DYNAMIC_ARRAY_GROWTH_FACTOR 2

typedef struct {
    void** items;
    size_t size;     // Number of items currently in the array
    size_t capacity; // Total capacity of the array
} DynamicArray;

// Creation and destruction
DynamicArray* dynamic_array_create(size_t initial_capacity);
DynamicArray* dynamic_array_create_default(void);
void dynamic_array_destroy(DynamicArray* array);

// Basic operations
bool dynamic_array_push(DynamicArray* array, void* item);
void* dynamic_array_pop(DynamicArray* array);
void* dynamic_array_get(const DynamicArray* array, size_t index);
bool dynamic_array_set(DynamicArray* array, size_t index, void* item);
bool dynamic_array_insert(DynamicArray* array, size_t index, void* item);
void* dynamic_array_remove(DynamicArray* array, size_t index);

// Utility functions
size_t dynamic_array_size(const DynamicArray* array);
size_t dynamic_array_capacity(const DynamicArray* array);
bool dynamic_array_is_empty(const DynamicArray* array);
void dynamic_array_clear(DynamicArray* array);
bool dynamic_array_reserve(DynamicArray* array, size_t new_capacity);
bool dynamic_array_shrink_to_fit(DynamicArray* array);

// Search functions
int dynamic_array_find(const DynamicArray* array, const void* item);
bool dynamic_array_contains(const DynamicArray* array, const void* item);

// Iteration helpers
void dynamic_array_foreach(const DynamicArray* array, void (*callback)(void* item, size_t index));

#endif // DYNAMIC_ARRAY_H