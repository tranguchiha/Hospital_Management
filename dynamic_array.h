#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

struct DynamicArray {
    void** elements;
    int size;
    int capacity;
};

void dynamic_array_init(struct DynamicArray* da, int initial_capacity);
void dynamic_array_append(struct DynamicArray* da, void* element);
void resize_array(struct DynamicArray* da);
void dynamic_array_free(struct DynamicArray* da);

#endif
