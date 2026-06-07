#include <stdio.h>
#include <stdlib.h>
#include "dynamic_array.h"

void dynamic_array_init(struct DynamicArray* da, int initial_capacity){
    if (initial_capacity <= 0){
        initial_capacity = 4;
    }
    da->elements = (void**)malloc(sizeof(void*) * (size_t)initial_capacity);
    if (da->elements == NULL){
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    da->size = 0;
    da->capacity = initial_capacity;
}


void resize_array(struct DynamicArray* da){
    int new_capacity;
    void** new_elements;
    new_capacity = da->capacity * 2;
    if (new_capacity <= 0){
        new_capacity = 4;
    }
    new_elements = (void**)realloc(da->elements, sizeof(void*) * (size_t)new_capacity);
    if (new_elements == NULL) {
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    da->elements = new_elements;
    da->capacity = new_capacity;
}


void dynamic_array_append(struct DynamicArray* da, void* element){
    if (da->size >= da->capacity){
        resize_array(da);
    }
    da->elements[da->size] = element;
    da->size++;
}


void dynamic_array_free(struct DynamicArray* da){
    free(da->elements);
    da->elements = NULL;
    da->size = 0;
    da->capacity = 0;
}
