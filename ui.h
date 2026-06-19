#ifndef UI_H
#define UI_H

#include "hospital.h"

#define UI_WIDTH         85  
#define UI_WIDTH_SMALL   60   
#define UI_COL_SEP       " | " 

/* Ma ANSI mau (chi hoat dong tren terminal ho tro ANSI) */
#ifdef _WIN32
  /* Windows: su dung WinAPI de bat mau neu co the, neu khong bo qua */
  #define CLR_RESET   ""
  #define CLR_BOLD    ""
  #define CLR_RED     ""
  #define CLR_GREEN   ""
  #define CLR_YELLOW  ""
  #define CLR_CYAN    ""
  #define CLR_WHITE   ""
  #define CLR_BLUE    ""
#else
  #define CLR_RESET   "\033[0m"
  #define CLR_BOLD    "\033[1m"
  #define CLR_RED     "\033[31m"
  #define CLR_GREEN   "\033[32m"
  #define CLR_YELLOW  "\033[33m"
  #define CLR_CYAN    "\033[36m"
  #define CLR_WHITE   "\033[37m"
  #define CLR_BLUE    "\033[34m"
#endif

typedef enum {
    MSG_SUCCESS = 0,   
    MSG_ERROR   = 1,   
    MSG_WARNING = 2,   
    MSG_INFO    = 3    
} MessageType;


void ui_draw_line(int width);
void ui_draw_double_line(int width);
void ui_print_row(const char* text, int width);
void ui_print_box_title(const char* title, int width);
void ui_print_box_end(int width);

void ui_notify(MessageType type, const char* message);
int ui_confirm(const char* question);
void ui_show_loading(const char* message);

void ui_show_main_menu(void);
void ui_show_welcome(void);
void ui_show_goodbye(void);

void ui_print_patient_card(const struct Patient* p);
void ui_print_patient_row(int stt, const struct Patient* p);
void ui_print_patient_table_header(void);
void ui_print_patient_table_footer(int total);
void ui_print_visit_history(const struct Patient* p);

void ui_print_queue_table(struct Department* dept);
void ui_print_queue_entry(int rank, const struct PatientInQueue* entry);
void ui_print_queue_header(const struct Department* dept);
void ui_print_priority_legend(void);

void ui_print_doctor_row(int stt, const struct Doctor* d);
void ui_print_doctor_table_header(void);
void ui_print_dept_row(int stt, const struct Department* d);
void ui_print_dept_table_header(void);

void ui_print_daily_stats(struct MedicalSystemState* system);
void ui_print_system_summary(const struct MedicalSystemState* system);

int ui_validate_date(const char* date_str);
int ui_validate_phone(const char* phone);
int ui_validate_citizen_id(const char* citizen_id);
int ui_validate_email(const char* email);
int ui_validate_blood_type(const char* blood);
void ui_read_validated_date(const char* prompt, char* buffer, int size);
void ui_read_validated_phone(const char* prompt, char* buffer, int size);
void ui_read_validated_citizen_id(const char* prompt, char* buffer, int size);
void ui_read_validated_email(const char* prompt, char* buffer, int size);
void ui_read_validated_blood(const char* prompt, char* buffer, int size);
int ui_read_int_range(const char* prompt, int min_val, int max_val);
double ui_read_double_range(const char* prompt, double min_val, double max_val);

void ui_menu_add_department(struct MedicalSystemState* system);
void ui_menu_add_doctor(struct MedicalSystemState* system);
void ui_menu_add_patient(struct MedicalSystemState* system);
void ui_menu_register_exam(struct MedicalSystemState* system);
void ui_menu_call_next(struct MedicalSystemState* system);
void ui_menu_show_queue(struct MedicalSystemState* system);
void ui_menu_list_patients(struct MedicalSystemState* system);
void ui_menu_list_doctors(struct MedicalSystemState* system);
void ui_menu_list_departments(struct MedicalSystemState* system);
void ui_menu_daily_statistics(struct MedicalSystemState* system);

#endif /* UI_H */
