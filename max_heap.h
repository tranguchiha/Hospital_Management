#ifndef MAX_HEAP_H
#define MAX_HEAP_H

struct Patient;

struct PatientInQueue {
    struct Patient* root_record;
    int medical_priority;
    int queue_priority_score;
    long registration_time;
    int queue_status;
    char current_dept_id[16];
    int checkin_number;
    int absent_count;
};

struct MaxHeap {
    struct PatientInQueue** heap_array;
    int size;
    int capacity;
};

void max_heap_init(struct MaxHeap* h, int initial_capacity);
void max_heap_free(struct MaxHeap* h, int free_items);
void swap_ptrs(void** a, void** b);
int priority_compare(struct PatientInQueue* a, struct PatientInQueue* b);
void max_heap_sift_up(struct MaxHeap* h, int index);
void max_heap_insert(struct MaxHeap* h, struct PatientInQueue* item);
void max_heapify(struct MaxHeap* h, int index);
struct PatientInQueue* max_heap_extract_max(struct MaxHeap* h);

#endif
