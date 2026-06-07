#include <stdio.h>
#include <stdlib.h>
#include "max_heap.h"

void max_heap_init(struct MaxHeap* h, int initial_capacity){
    if (initial_capacity <= 0){
        initial_capacity = 8;
    }
    h->heap_array = (struct PatientInQueue**)malloc(sizeof(struct PatientInQueue*) * (size_t)initial_capacity);
    if (h->heap_array == NULL) {
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    h->size = 0;
    h->capacity = initial_capacity;
}


void max_heap_free(struct MaxHeap* h, int free_items){
    int i;
    if (free_items){
        for (i = 0; i < h->size; i++) {
            free(h->heap_array[i]);
        }
    }
    free(h->heap_array);
    h->heap_array = NULL;
    h->size = 0;
    h->capacity = 0;
}


void swap_ptrs(void** a, void** b){
    void* temp;
    temp = *a;
    *a = *b;
    *b = temp;
}


int priority_compare(struct PatientInQueue* a, struct PatientInQueue* b){
    if (a->queue_priority_score != b->queue_priority_score){
        return a->queue_priority_score - b->queue_priority_score;
    }
    /* Neu diem bang nhau, muc do khan cap nho hon se duoc goi truoc. */
    if (a->medical_priority != b->medical_priority){
        return b->medical_priority - a->medical_priority;
    }
    if (a->registration_time < b->registration_time){
        return 1;
    }
    if (a->registration_time > b->registration_time){
        return -1;
    }
    return b->checkin_number - a->checkin_number;
}


void max_heap_sift_up(struct MaxHeap* h, int index){
    int parent;
    while (index > 0){
        parent = (index - 1) / 2;
        if (priority_compare(h->heap_array[index], h->heap_array[parent]) <= 0){
            break;
        }
        swap_ptrs((void**)&h->heap_array[index], (void**)&h->heap_array[parent]);
        index = parent;
    }
}


void max_heap_insert(struct MaxHeap* h, struct PatientInQueue* item){
    struct PatientInQueue** new_array;
    if (h->size >= h->capacity) {
        h->capacity = h->capacity * 2;
        if (h->capacity <= 0){
            h->capacity = 8;
        }
        new_array = (struct PatientInQueue**)realloc(h->heap_array, sizeof(struct PatientInQueue*) * (size_t)h->capacity);
        if (new_array == NULL) {
            printf("Loi cap phat vung nho.\n");
            exit(1);
        }
        h->heap_array = new_array;
    }
    h->heap_array[h->size] = item;
    max_heap_sift_up(h, h->size);
    h->size++;
}


void max_heapify(struct MaxHeap* h, int index){
    int largest;
    int left;
    int right;
    while (1) {
        largest = index;
        left = index * 2 + 1;
        right = index * 2 + 2;
        if (left < h->size && priority_compare(h->heap_array[left], h->heap_array[largest]) > 0){
            largest = left;
        }
        if (right < h->size && priority_compare(h->heap_array[right], h->heap_array[largest]) > 0){
            largest = right;
        }
        if (largest == index){
            break;
        }
        swap_ptrs((void**)&h->heap_array[index], (void**)&h->heap_array[largest]);
        index = largest;
    }
}


struct PatientInQueue* max_heap_extract_max(struct MaxHeap* h){
    struct PatientInQueue* top;
    if (h->size <= 0) {
        return NULL;
    }
    top = h->heap_array[0];
    h->size--;
    if (h->size > 0) {
        h->heap_array[0] = h->heap_array[h->size];
        max_heapify(h, 0);
    }
    return top;
}
