#include "core/dynamic_array.h"
#include <stdlib.h>
#include <string.h>

// Internal helper function to grow the array
static bool dynamic_array_grow(DynamicArray* array) {
    if (!array) return false;
    
    size_t new_capacity = array->capacity * DYNAMIC_ARRAY_GROWTH_FACTOR;
    void** new_items = realloc(array->items, new_capacity * sizeof(void*));
    
    if (!new_items) {
        return false; // Memory allocation failed
    }
    
    array->items = new_items;
    array->capacity = new_capacity;
    return true;
}

// Creation and destruction
DynamicArray* dynamic_array_create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = DYNAMIC_ARRAY_DEFAULT_CAPACITY;
    }
    
    DynamicArray* array = malloc(sizeof(DynamicArray));
    if (!array) {
        return NULL;
    }
    
    array->items = malloc(initial_capacity * sizeof(void*));
    if (!array->items) {
        free(array);
        return NULL;
    }
    
    array->size = 0;
    array->capacity = initial_capacity;
    return array;
}

DynamicArray* dynamic_array_create_default(void) {
    return dynamic_array_create(DYNAMIC_ARRAY_DEFAULT_CAPACITY);
}

void dynamic_array_destroy(DynamicArray* array) {
    if (!array) return;
    
    free(array->items);
    free(array);
}

// Basic operations
bool dynamic_array_push(DynamicArray* array, void* item) {
    if (!array) return false;
    
    // Check if we need to grow the array
    if (array->size >= array->capacity) {
        if (!dynamic_array_grow(array)) {
            return false;
        }
    }
    
    array->items[array->size] = item;
    array->size++;
    return true;
}

void* dynamic_array_pop(DynamicArray* array) {
    if (!array || array->size == 0) {
        return NULL;
    }
    
    array->size--;
    return array->items[array->size];
}

void* dynamic_array_get(const DynamicArray* array, size_t index) {
    if (!array || index >= array->size) {
        return NULL;
    }
    
    return array->items[index];
}

bool dynamic_array_set(DynamicArray* array, size_t index, void* item) {
    if (!array || index >= array->size) {
        return false;
    }
    
    array->items[index] = item;
    return true;
}

bool dynamic_array_insert(DynamicArray* array, size_t index, void* item) {
    if (!array || index > array->size) {
        return false;
    }
    
    // Check if we need to grow the array
    if (array->size >= array->capacity) {
        if (!dynamic_array_grow(array)) {
            return false;
        }
    }
    
    // Shift elements to the right
    if (index < array->size) {
        memmove(&array->items[index + 1], &array->items[index], 
                (array->size - index) * sizeof(void*));
    }
    
    array->items[index] = item;
    array->size++;
    return true;
}

void* dynamic_array_remove(DynamicArray* array, size_t index) {
    if (!array || index >= array->size) {
        return NULL;
    }
    
    void* item = array->items[index];
    
    // Shift elements to the left
    if (index < array->size - 1) {
        memmove(&array->items[index], &array->items[index + 1], 
                (array->size - index - 1) * sizeof(void*));
    }
    
    array->size--;
    return item;
}

// Utility functions
size_t dynamic_array_size(const DynamicArray* array) {
    return array ? array->size : 0;
}

size_t dynamic_array_capacity(const DynamicArray* array) {
    return array ? array->capacity : 0;
}

bool dynamic_array_is_empty(const DynamicArray* array) {
    return !array || array->size == 0;
}

void dynamic_array_clear(DynamicArray* array) {
    if (array) {
        array->size = 0;
    }
}

bool dynamic_array_reserve(DynamicArray* array, size_t new_capacity) {
    if (!array || new_capacity <= array->capacity) {
        return false;
    }
    
    void** new_items = realloc(array->items, new_capacity * sizeof(void*));
    if (!new_items) {
        return false;
    }
    
    array->items = new_items;
    array->capacity = new_capacity;
    return true;
}

bool dynamic_array_shrink_to_fit(DynamicArray* array) {
    if (!array || array->size == 0) {
        return false;
    }
    
    if (array->size == array->capacity) {
        return true; // Already optimal
    }
    
    void** new_items = realloc(array->items, array->size * sizeof(void*));
    if (!new_items) {
        return false;
    }
    
    array->items = new_items;
    array->capacity = array->size;
    return true;
}

// Search functions
int dynamic_array_find(const DynamicArray* array, const void* item) {
    if (!array) return -1;
    
    for (size_t i = 0; i < array->size; i++) {
        if (array->items[i] == item) {
            return (int)i;
        }
    }
    
    return -1; // Not found
}

bool dynamic_array_contains(const DynamicArray* array, const void* item) {
    return dynamic_array_find(array, item) != -1;
}

// Iteration helpers
void dynamic_array_foreach(const DynamicArray* array, void (*callback)(void* item, size_t index)) {
    if (!array || !callback) return;
    
    for (size_t i = 0; i < array->size; i++) {
        callback(array->items[i], i);
    }
}