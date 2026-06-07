#ifndef HOSPITAL_H
#define HOSPITAL_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#ifdef _WIN32
#include <direct.h>
#define GETCWD _getcwd
#define PATH_SEP "\\"
#else
#include <unistd.h>
#define GETCWD getcwd
#define PATH_SEP "/"
#endif

#include "dynamic_array.h"
#include "max_heap.h"

#define TEXT_SMALL 64
#define TEXT_MEDIUM 128
#define TEXT_LARGE 512
#define INPUT_SIZE 512
#define TRUE 1
#define FALSE 0

#define MEDICAL_PRIORITY_HIGHEST 1
#define MEDICAL_PRIORITY_LOWEST 5
#define SOCIAL_PRIORITY_BONUS 1
#define LONG_WAIT_MINUTES 60
#define LONG_WAIT_BONUS 1
#define MAX_ABSENT_COUNT 3

struct MedicalVisit {
    char visit_id[15];
    char visit_date[15];
    char diagnosis[200];
    char prescription[500];
    char doctor_id[15];
    char department_id[7];
};

struct Patient {
    char patient_id[12];
    char full_name[50];
    char birth_date[15];
    int gender;
    char citizen_id[15];
    char phone_number[15];
    char email[50];
    char address[100];
    char blood_type[5];
    struct DynamicArray allergies;
    struct DynamicArray comorbidities;
    int is_priority_object;
    char priority_type[30];
    struct DynamicArray visit_history;
};

struct Department {
    char dept_id[7];
    char dept_name[50];
    struct MaxHeap wait_queue;
};

struct Doctor {
    char doctor_id[15];
    char doctor_name[50];
    char specialization[50];
    char assigned_dept_id[7];
};

struct MedicalSystemState {
    struct DynamicArray patient_records_array;
    struct DynamicArray doctor_records_array;
    struct DynamicArray dept_records_array;
    int next_patient_id_counter;
    int next_doctor_id_counter;
    int next_checkin_number;
};

int assign_medical_priority(double spo2, int pulse, int systolic_bp, double temperature);
int calculate_queue_priority_score(struct PatientInQueue* entry, long current_time);
void register_for_examination(struct MedicalSystemState* system, struct Patient* patient_ptr, const char* dept_id, double spo2, int pulse, int bp, double temp);
void update_priority_for_long_waiters(struct Department* dept, long current_time);
void handle_absent_patient(struct Department* dept, struct PatientInQueue* entry);
void rebuild_heap(struct MaxHeap* heap);
int hoare_partition(void** arr, int low, int high);
void quick_sort_patients(void** arr, int low, int high);

void safe_copy(char* dest, size_t dest_size, const char* src);
char* duplicate_text(const char* s);
void read_line(const char* prompt, char* buffer, int size);
int read_int(const char* prompt);
double read_double(const char* prompt);
void press_enter(void);
void add_text_to_array(struct DynamicArray* arr, const char* text);

void system_state_init(struct MedicalSystemState* system);
void system_state_free(struct MedicalSystemState* system);
struct Department* find_department(struct MedicalSystemState* system, const char* dept_id);
struct Patient* find_patient(struct MedicalSystemState* system, const char* patient_id);
struct Doctor* find_doctor_in_dept(struct MedicalSystemState* system, const char* dept_id);
struct Department* create_department(const char* dept_id, const char* dept_name);
struct Doctor* create_doctor(struct MedicalSystemState* system, const char* name, const char* specialization, const char* dept_id);
struct Patient* create_patient(struct MedicalSystemState* system, const char* name, const char* birth, int gender, const char* citizen_id, const char* phone, const char* email, const char* address, const char* blood, int priority_object, const char* priority_type);
void add_visit(struct Patient* p, const char* doctor_id, const char* dept_id, const char* diagnosis, const char* prescription);
void print_patient(const struct Patient* p);
void print_queue_of_department(struct Department* dept);

void menu_add_department(struct MedicalSystemState* system);
void menu_add_doctor(struct MedicalSystemState* system);
void menu_add_patient(struct MedicalSystemState* system);
void menu_register_exam(struct MedicalSystemState* system);
void menu_call_next(struct MedicalSystemState* system);
void menu_show_queue(struct MedicalSystemState* system);
void menu_list_patients(struct MedicalSystemState* system);
void menu_list_doctors(struct MedicalSystemState* system);
void menu_list_departments(struct MedicalSystemState* system);
void menu_daily_statistics(struct MedicalSystemState* system);

int file_exists(const char* filename);
void build_path_from_exe(char* dest, int dest_size, const char* argv0, const char* filename);
void build_sibling_path(char* dest, int dest_size, const char* source_path, const char* filename);
void choose_input_file(char* dest, int dest_size, const char* argv0);
void choose_output_file(char* dest, int dest_size, const char* input_path);
void trim_newline(char* s);
int split_fields(char* line, char fields[][TEXT_LARGE], int max_fields);
void load_data_from_file(struct MedicalSystemState* system, const char* filename);
void save_data_to_file(struct MedicalSystemState* system, const char* filename);
void save_all_data_files(struct MedicalSystemState* system, const char* input_file, const char* output_file);

void clear_screen_simple(void);
void print_box_line(const char* text, int width);
void show_menu(void);

#endif
