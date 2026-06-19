#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ui.h"
   
void ui_draw_line(int width) {
    int i;
    printf("+");
    for (i = 0; i < width; i++) printf("-");
    printf("+\n");
}

void ui_draw_double_line(int width) {
    int i;
    printf("+");
    for (i = 0; i < width; i++) printf("=");
    printf("+\n");
}

void ui_print_row(const char* text, int width) {
    int len;
    int pad;
    int i;
    len = (text != NULL) ? (int)strlen(text) : 0;
    if (len > width - 2) len = width - 2;
    printf("| ");
    for (i = 0; i < len; i++) putchar(text[i]);
    pad = width - 2 - len;
    for (i = 0; i < pad; i++) putchar(' ');
    printf(" |\n");
}

void ui_print_box_title(const char* title, int width) {
    int title_len;
    int left_pad;
    int right_pad;
    int i;
    title_len = (title != NULL) ? (int)strlen(title) : 0;
    if (title_len > width - 4) title_len = width - 4;
    left_pad  = (width - title_len) / 2;
    right_pad = width - title_len - left_pad;
    ui_draw_double_line(width);
    printf("|");
    for (i = 0; i < left_pad; i++) putchar(' ');
    for (i = 0; i < title_len; i++) putchar(title[i]);
    for (i = 0; i < right_pad; i++) putchar(' ');
    printf("|\n");
    ui_draw_double_line(width);
}

void ui_print_box_end(int width) {
    ui_draw_line(width);
}

void ui_notify(MessageType type, const char* message) {
    const char* tag;
    switch (type) {
        case MSG_SUCCESS: tag = "[ OK ]"; break;
        case MSG_ERROR:   tag = "[LOI] "; break;
        case MSG_WARNING: tag = "[!!!] "; break;
        case MSG_INFO:    tag = "[ i ] "; break;
        default:          tag = "      "; break;
    }
    printf("\n  %s %s\n\n", tag, (message != NULL) ? message : "");
}

int ui_confirm(const char* question) {
    char buf[64];
    printf("  %s (C/K): ", (question != NULL) ? question : "Xac nhan?");
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    return (buf[0] == 'C' || buf[0] == 'c' || buf[0] == 'Y' || buf[0] == 'y') ? 1 : 0;
}

void ui_show_loading(const char* message) {
    printf("  >> %s...\n", (message != NULL) ? message : "Dang xu ly");
}

void ui_show_welcome(void) {
    time_t now;
    struct tm* tm_info;
    char date_buf[32];
    char time_buf[32];

    now = time(NULL);
    tm_info = localtime(&now);

    if (tm_info != NULL) {
        strftime(date_buf, sizeof(date_buf), "%d/%m/%Y", tm_info);
        strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);
    } else {
        safe_copy(date_buf, sizeof(date_buf), "--/--/----");
        safe_copy(time_buf, sizeof(time_buf), "--:--:--");
    }

    clear_screen_simple();
    printf("\n");
    ui_draw_double_line(UI_WIDTH);
    ui_print_row("", UI_WIDTH);
    ui_print_row("                             HE THONG QUAN LY BENH VIEN ", UI_WIDTH);
    ui_print_row("      SU DUNG HANG DOI UU TIEN DE PHAN LOAI BENH NHAN THEO TINH TRANG SUC KHOE", UI_WIDTH);
    ui_print_row("", UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    {
        char line_buf[512];
        sprintf(line_buf, "                   Ngay: %s           Gio: %s", date_buf, time_buf);
        ui_print_row(line_buf, UI_WIDTH);
    }
    ui_print_row("                            |  Nhom sinh vien thuc hien  |", UI_WIDTH);
    ui_draw_double_line(UI_WIDTH);
    printf("\n");
}

void ui_show_main_menu(void) {
    clear_screen_simple();
    printf("\n");
    ui_print_box_title("  QUAN LY BENH VIEN  ", UI_WIDTH);
    ui_print_row("", UI_WIDTH);
    ui_print_row("  QUAN LY CO SO DU LIEU", UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    ui_print_row("   1.  Them khoa moi", UI_WIDTH);
    ui_print_row("   2.  Them bac si moi", UI_WIDTH);
    ui_print_row("   3.  Them ho so benh nhan moi", UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    ui_print_row("  HANG DOI & KHAM BENH", UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    ui_print_row("   4.  Dang ky kham - tu dong xep hang theo uu tien", UI_WIDTH);
    ui_print_row("   5.  Goi benh nhan tiep theo vao kham", UI_WIDTH);
    ui_print_row("   6.  Xem hang doi cua mot khoa", UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    ui_print_row("  DANH SACH & THONG KE", UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    ui_print_row("   7.  Xem danh sach tat ca benh nhan", UI_WIDTH);
    ui_print_row("   8.  Xem danh sach tat ca bac si", UI_WIDTH);
    ui_print_row("   9.  Xem danh sach tat ca khoa", UI_WIDTH);
    ui_print_row("  10.  Thong ke cuoi ngay", UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    ui_print_row("   0.  Luu va thoat chuong trinh", UI_WIDTH);
    ui_print_box_end(UI_WIDTH);
    printf("\n");
}

void ui_show_goodbye(void) {
    printf("\n");
    ui_draw_double_line(UI_WIDTH);
    ui_print_row("", UI_WIDTH);
    ui_print_row("   Du lieu da duoc luu vao he thong", UI_WIDTH);
    ui_print_row("   Cam on da su dung he thong!", UI_WIDTH);
    ui_print_row("", UI_WIDTH);
    ui_draw_double_line(UI_WIDTH);
    printf("\n");
}
 
static const char* gender_str(int gender) {
    return (gender == 1) ? "Nam" : "Nu";
}

static const char* priority_label(int p) {
    switch (p) {
        case 1: return "Cap cuu  (Muc 1)";
        case 2: return "Nguy cap (Muc 2)";
        case 3: return "Trung binh (Muc 3)";
        case 4: return "On dinh (Muc 4)";
        case 5: return "Binh thuong (Muc 5)";
        default: return "Khong ro";
    }
}

void ui_print_patient_table_header(void) {
    ui_draw_line(UI_WIDTH);
    printf("| %-5s | %-10s | %-20s | %-10s | %-10s | %-5s |\n",
           "STT", "Ma BN", "Ho va ten", "Ngay sinh", "Nhom mau", "Kham");
    ui_draw_line(UI_WIDTH);
}

void ui_print_patient_row(int stt, const struct Patient* p) {
    if (p == NULL) return;
    printf("| %-5d | %-10s | %-20s | %-10s | %-10s | %-5d |\n",
           stt,
           p->patient_id,
           p->full_name,
           p->birth_date,
           p->blood_type,
           p->visit_history.size);
}

void ui_print_patient_table_footer(int total) {
    char buf[UI_WIDTH];
    ui_draw_line(UI_WIDTH);
    sprintf(buf, "  Tong cong: %d benh nhan", total);
    ui_print_row(buf, UI_WIDTH);
    ui_print_box_end(UI_WIDTH);
}

void ui_print_patient_card(const struct Patient* p) {
    int i;
    char line_buf[512];

    if (p == NULL) return;

    ui_draw_double_line(UI_WIDTH);
    sprintf(line_buf, "  HO SO BENH NHAN: %s", p->patient_id);
    ui_print_row(line_buf, UI_WIDTH);
    ui_draw_line(UI_WIDTH);

    sprintf(line_buf, "  Ho va ten   : %s", p->full_name);
    ui_print_row(line_buf, UI_WIDTH);

    sprintf(line_buf, "  Ngay sinh   : %s   |  Gioi tinh: %s",
            p->birth_date, gender_str(p->gender));
    ui_print_row(line_buf, UI_WIDTH);

    sprintf(line_buf, "  CCCD        : %s", p->citizen_id);
    ui_print_row(line_buf, UI_WIDTH);

    sprintf(line_buf, "  Dien thoai  : %s   |  Email: %s",
            p->phone_number, p->email);
    ui_print_row(line_buf, UI_WIDTH);

    sprintf(line_buf, "  Dia chi     : %s", p->address);
    ui_print_row(line_buf, UI_WIDTH);

    sprintf(line_buf, "  Nhom mau    : %s", p->blood_type);
    ui_print_row(line_buf, UI_WIDTH);

    if (p->is_priority_object) {
        sprintf(line_buf, "  Doi tuong UU TIEN XA HOI: %s", p->priority_type);
        ui_print_row(line_buf, UI_WIDTH);
    }

    if (p->allergies.size > 0) {
        char allergy_buf[256] = "  Di ung      : ";
        for (i = 0; i < p->allergies.size; i++) {
            if (i > 0) strncat(allergy_buf, ", ", sizeof(allergy_buf) - strlen(allergy_buf) - 1);
            strncat(allergy_buf, (char*)p->allergies.elements[i],
                    sizeof(allergy_buf) - strlen(allergy_buf) - 1);
        }
        ui_print_row(allergy_buf, UI_WIDTH);
    }

    if (p->comorbidities.size > 0) {
        char com_buf[256] = "  Benh nen    : ";
        for (i = 0; i < p->comorbidities.size; i++) {
            if (i > 0) strncat(com_buf, ", ", sizeof(com_buf) - strlen(com_buf) - 1);
            strncat(com_buf, (char*)p->comorbidities.elements[i],
                    sizeof(com_buf) - strlen(com_buf) - 1);
        }
        ui_print_row(com_buf, UI_WIDTH);
    }

    ui_draw_line(UI_WIDTH);
    sprintf(line_buf, "  Tong so luot kham: %d", p->visit_history.size);
    ui_print_row(line_buf, UI_WIDTH);
    ui_print_box_end(UI_WIDTH);
}

void ui_print_visit_history(const struct Patient* p) {
    int i;
    struct MedicalVisit* v;
    char line_buf[1024];

    if (p == NULL || p->visit_history.size == 0) {
        ui_notify(MSG_INFO, "Benh nhan chua co luot kham nao.");
        return;
    }

    printf("\n  -- LICH SU KHAM BENH: %s - %s --\n", p->patient_id, p->full_name);
    for (i = 0; i < p->visit_history.size; i++) {
        v = (struct MedicalVisit*)p->visit_history.elements[i];
        ui_draw_line(UI_WIDTH);
        sprintf(line_buf, "  Luot %d | Ma: %s | Ngay: %s | Khoa: %s | Bac si: %s",
                i + 1, v->visit_id, v->visit_date, v->department_id, v->doctor_id);
        ui_print_row(line_buf, UI_WIDTH);
        sprintf(line_buf, "  Chan doan  : %s", v->diagnosis);
        ui_print_row(line_buf, UI_WIDTH);
        sprintf(line_buf, "  Don thuoc  : %s", v->prescription);
        ui_print_row(line_buf, UI_WIDTH);
    }
    ui_print_box_end(UI_WIDTH);
}

void ui_print_priority_legend(void) {
    printf("\n  -- GIAI THICH MUC DO KHAN CAP --\n");
    ui_draw_line(UI_WIDTH);
    printf("| %-6s | %-20s | %-50.50s |\n", "Muc", "Ten goi", "Dieu kien");
    ui_draw_line(UI_WIDTH);
    printf("| %-6s | %-20s | %-50.50s |\n", "Muc 1", "Cap cuu",
           "SpO2<90%, mach>=130, HA<90 hoac >=180, nhiet>=40");
    printf("| %-6s | %-20s | %-50.50s |\n", "Muc 2", "Nguy cap",
           "SpO2<94%, mach>=110, HA<100 hoac >=160, nhiet>=39");
    printf("| %-6s | %-20s | %-50.50s |\n", "Muc 3", "Trung binh",
           "SpO2<96%, mach>=100, HA>=140, nhiet>=38");
    printf("| %-6s | %-20s | %-50.50s |\n", "Muc 4", "On dinh",
           "SpO2<98%, mach bat thuong nhe, nhiet>=37.5");
    printf("| %-6s | %-20s | %-50.50s |\n", "Muc 5", "Binh thuong", "Cac truong hop con lai");
    ui_draw_line(UI_WIDTH);
    printf("  (*) Benh nhan thuoc doi tuong uu tien xa hoi duoc cong +1 diem.\n\n");
}

void ui_print_queue_header(const struct Department* dept) {
    char buf[512];
    if (dept == NULL) return;
    printf("\n");
    ui_draw_double_line(UI_WIDTH);
    sprintf(buf, "  HANG DOI KHAM BENH - KHOA: %s - %s",
            dept->dept_id, dept->dept_name);
    ui_print_row(buf, UI_WIDTH);
    sprintf(buf, "  Tong so dang cho: %d benh nhan", dept->wait_queue.size);
    ui_print_row(buf, UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    printf("| %-4s | %-10s | %-20s | %-20s | %-5s | %-5s |\n",
           "STT", "Ma BN", "Ho va ten", "Muc do khan cap", "Diem", "Vang");
    ui_draw_line(UI_WIDTH);
}

void ui_print_queue_entry(int rank, const struct PatientInQueue* entry) {
    char absent_str[8];
    if (entry == NULL || entry->root_record == NULL) return;
    sprintf(absent_str, "%d/3", entry->absent_count);
    printf("| %-4d | %-10s | %-20s | %-20s | %-5d | %-5s |\n",
           rank,
           entry->root_record->patient_id,
           entry->root_record->full_name,
           priority_label(entry->medical_priority),
           entry->queue_priority_score,
           absent_str);
}

void ui_print_queue_table(struct Department* dept) {
    int i;
    if (dept == NULL) {
        ui_notify(MSG_ERROR, "Khong tim thay khoa.");
        return;
    }
    update_priority_for_long_waiters(dept, (long)time(NULL));
    ui_print_queue_header(dept);
    if (dept->wait_queue.size == 0) {
        ui_print_row("  (Hang doi dang trong)", UI_WIDTH);
    } else {
        for (i = 0; i < dept->wait_queue.size; i++) {
            ui_print_queue_entry(i + 1, dept->wait_queue.heap_array[i]);
        }
    }
    ui_print_box_end(UI_WIDTH);
    ui_print_priority_legend();
}
    
void ui_print_doctor_table_header(void) {
    ui_draw_line(UI_WIDTH);
    printf("| %-5s | %-10s | %-20s | %-18s | %-8s |\n",
           "STT", "Ma BS", "Ho va ten", "Chuyen khoa", "Ma khoa");
    ui_draw_line(UI_WIDTH);
}

void ui_print_doctor_row(int stt, const struct Doctor* d) {
    if (d == NULL) return;
    printf("| %-5d | %-10s | %-20s | %-18s | %-8s |\n",
           stt,
           d->doctor_id,
           d->doctor_name,
           d->specialization,
           d->assigned_dept_id);
}

void ui_print_dept_table_header(void) {
    ui_draw_line(UI_WIDTH);
    printf("| %-5s | %-6s | %-30s | %-13s |\n",
           "STT", "Ma", "Ten khoa", "Dang cho kham");
    ui_draw_line(UI_WIDTH);
}

void ui_print_dept_row(int stt, const struct Department* d) {
    if (d == NULL) return;
    printf("| %-5d | %-6s | %-30s | %-13d |\n",
           stt,
           d->dept_id,
           d->dept_name,
           d->wait_queue.size);
}

void ui_print_system_summary(const struct MedicalSystemState* system) {
    int total_queue;
    int i;
    struct Department* dept;
    char buf[512];

    total_queue = 0;
    for (i = 0; i < system->dept_records_array.size; i++) {
        dept = (struct Department*)system->dept_records_array.elements[i];
        total_queue += dept->wait_queue.size;
    }

    printf("\n");
    ui_print_box_title("  TOM TAT HE THONG  ", UI_WIDTH);
    sprintf(buf, "  Tong so khoa phong      : %d", system->dept_records_array.size);
    ui_print_row(buf, UI_WIDTH);
    sprintf(buf, "  Tong so bac si          : %d", system->doctor_records_array.size);
    ui_print_row(buf, UI_WIDTH);
    sprintf(buf, "  Tong so ho so benh nhan : %d", system->patient_records_array.size);
    ui_print_row(buf, UI_WIDTH);
    sprintf(buf, "  Tong so dang trong hang doi: %d benh nhan", total_queue);
    ui_print_row(buf, UI_WIDTH);
    ui_print_box_end(UI_WIDTH);
}

void ui_print_daily_stats(struct MedicalSystemState* system) {
    void** copy_arr;
    int i;
    struct Patient* p;
    int total_visits;
    char buf[512];

    if (system->patient_records_array.size == 0) {
        ui_notify(MSG_INFO, "Chua co benh nhan nao trong he thong.");
        return;
    }

    copy_arr = (void**)malloc(sizeof(void*) * (size_t)system->patient_records_array.size);
    if (copy_arr == NULL) {
        ui_notify(MSG_ERROR, "Loi cap phat bo nho.");
        return;
    }
    for (i = 0; i < system->patient_records_array.size; i++) {
        copy_arr[i] = system->patient_records_array.elements[i];
    }
    quick_sort_patients(copy_arr, 0, system->patient_records_array.size - 1);

    total_visits = 0;
    for (i = 0; i < system->patient_records_array.size; i++) {
        p = (struct Patient*)copy_arr[i];
        total_visits += p->visit_history.size;
    }

    printf("\n");
    ui_print_box_title("  THONG KE CUOI NGAY  ", UI_WIDTH);
    sprintf(buf, "  Tong luot kham hom nay: %d   |   Tong benh nhan: %d",
            total_visits, system->patient_records_array.size);
    ui_print_row(buf, UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    printf("| %-4s | %-10s | %-25s | %-8s | %-28s |\n",
           "TT", "Ma BN", "Ho va ten", "So luot", "UU tien XH");
    ui_draw_line(UI_WIDTH);

    for (i = 0; i < system->patient_records_array.size; i++) {
        p = (struct Patient*)copy_arr[i];
        printf("| %-4d | %-10s | %-25s | %-8d | %-28s |\n",
               i + 1,
               p->patient_id,
               p->full_name,
               p->visit_history.size,
               p->is_priority_object ? p->priority_type : "Khong");
    }
    ui_print_box_end(UI_WIDTH);
    ui_print_system_summary(system);

    free(copy_arr);
}

int ui_validate_date(const char* date_str) {
    int y, m, d;
    int days_in_month[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (date_str == NULL || strlen(date_str) != 10) return 0;
    if (date_str[4] != '-' || date_str[7] != '-') return 0;
    if (sscanf(date_str, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    if (y < 1900 || y > 2100) return 0;
    if (m < 1 || m > 12) return 0;
    /* Nam nhuan */
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
        days_in_month[2] = 29;
    }
    if (d < 1 || d > days_in_month[m]) return 0;
    return 1;
}

int ui_validate_phone(const char* phone) {
    int i;
    int len;
    if (phone == NULL) return 0;
    len = (int)strlen(phone);
    if (len < 10 || len > 11) return 0;
    for (i = 0; i < len; i++) {
        if (phone[i] < '0' || phone[i] > '9') return 0;
    }
    return 1;
}


int ui_validate_citizen_id(const char* citizen_id) {
    return validate_citizen_id(citizen_id);
}

int ui_validate_email(const char* email) {
    const char* at;
    const char* dot;
    if (email == NULL || strlen(email) < 5) return 0;
    at = strchr(email, '@');
    if (at == NULL) return 0;
    dot = strchr(at, '.');
    if (dot == NULL || dot == at + 1) return 0;
    return 1;
}

int ui_validate_blood_type(const char* blood) {
    const char* valid[] = {"A+","A-","B+","B-","AB+","AB-","O+","O-",NULL};
    int i;
    if (blood == NULL) return 0;
    for (i = 0; valid[i] != NULL; i++) {
        if (strcmp(blood, valid[i]) == 0) return 1;
    }
    return 0;
}

void ui_read_validated_date(const char* prompt, char* buffer, int size) {
    while (1) {
        read_line(prompt, buffer, size);
        if (ui_validate_date(buffer)) return;
        ui_notify(MSG_WARNING, "Dinh dang ngay khong hop le. Vui long nhap lai theo dang YYYY-MM-DD (vi du: 1990-05-20).");
    }
}

void ui_read_validated_phone(const char* prompt, char* buffer, int size) {
    while (1) {
        read_line(prompt, buffer, size);
        if (ui_validate_phone(buffer)) return;
        ui_notify(MSG_WARNING, "So dien thoai khong hop le. Phai co 10-11 chu so (vi du: 0912345678).");
    }
}


void ui_read_validated_citizen_id(const char* prompt, char* buffer, int size) {
    while (1) {
        read_line(prompt, buffer, size);
        if (ui_validate_citizen_id(buffer)) return;
        ui_notify(MSG_WARNING, "CCCD khong hop le. CCCD phai gom dung 12 chu so, khong duoc nhap thua/thieu hoac ky tu khac.");
    }
}

void ui_read_validated_email(const char* prompt, char* buffer, int size) {
    while (1) {
        read_line(prompt, buffer, size);
        if (strlen(buffer) == 0 || ui_validate_email(buffer)) return;
        ui_notify(MSG_WARNING, "Email khong hop le. Phai co dang name@domain.com.");
    }
}

void ui_read_validated_blood(const char* prompt, char* buffer, int size) {
    while (1) {
        read_line(prompt, buffer, size);
        if (ui_validate_blood_type(buffer)) return;
        ui_notify(MSG_WARNING, "Nhom mau khong hop le. Cac gia tri hop le: A+, A-, B+, B-, AB+, AB-, O+, O-");
    }
}

int ui_read_int_range(const char* prompt, int min_val, int max_val) {
    int val;
    char buf[64];
    while (1) {
        printf("  %s [%d-%d]: ", prompt, min_val, max_val);
        if (fgets(buf, sizeof(buf), stdin) == NULL) continue;
        if (sscanf(buf, "%d", &val) != 1) {
            ui_notify(MSG_WARNING, "Vui long nhap mot so nguyen.");
            continue;
        }
        if (val < min_val || val > max_val) {
            char warn[128];
            sprintf(warn, "Gia tri phai trong khoang %d den %d.", min_val, max_val);
            ui_notify(MSG_WARNING, warn);
            continue;
        }
        return val;
    }
}

double ui_read_double_range(const char* prompt, double min_val, double max_val) {
    double val;
    char buf[64];
    while (1) {
        printf("  %s [%.1f-%.1f]: ", prompt, min_val, max_val);
        if (fgets(buf, sizeof(buf), stdin) == NULL) continue;
        if (sscanf(buf, "%lf", &val) != 1) {
            ui_notify(MSG_WARNING, "Vui long nhap mot so thuc.");
            continue;
        }
        if (val < min_val || val > max_val) {
            char warn[128];
            sprintf(warn, "Gia tri phai trong khoang %.1f den %.1f.", min_val, max_val);
            ui_notify(MSG_WARNING, warn);
            continue;
        }
        return val;
    }
}

void ui_menu_add_department(struct MedicalSystemState* system) {
    char id[TEXT_SMALL];
    char name[TEXT_MEDIUM];

    printf("\n");
    ui_print_box_title("  THEM KHOA MOI  ", UI_WIDTH);

    if (system->dept_records_array.size > 0) {
        int i;
        struct Department* d;
        ui_print_row("  Danh sach khoa hien co:", UI_WIDTH);
        for (i = 0; i < system->dept_records_array.size; i++) {
            char buf[UI_WIDTH];
            d = (struct Department*)system->dept_records_array.elements[i];
            sprintf(buf, "    %s - %s", d->dept_id, d->dept_name);
            ui_print_row(buf, UI_WIDTH);
        }
        ui_draw_line(UI_WIDTH);
    }

    printf("  Ma khoa (toi da 6 ky tu, vi du: K003): ");
    read_line("", id, sizeof(id));
    if (strlen(id) == 0 || strlen(id) > 6) {
        ui_notify(MSG_ERROR, "Ma khoa phai tu 1-6 ky tu.");
        return;
    }
    if (find_department(system, id) != NULL) {
        ui_notify(MSG_ERROR, "Ma khoa nay da ton tai. Vui long chon ma khac.");
        return;
    }
    printf("  Ten khoa (vi du: Cap cuu, Noi khoa): ");
    read_line("", name, sizeof(name));
    if (strlen(name) == 0) {
        ui_notify(MSG_ERROR, "Ten khoa khong duoc de trong.");
        return;
    }
    if (find_department_by_name(system, name) != NULL) {
        ui_notify(MSG_ERROR, "Ten khoa nay da ton tai. Khong the them khoa moi trung ten du khac ma khoa.");
        return;
    }

    dynamic_array_append(&system->dept_records_array, create_department(id, name));

    {
        char msg[512];
        sprintf(msg, "Them khoa thanh cong: [%s] %s", id, name);
        ui_notify(MSG_SUCCESS, msg);
    }
}

void ui_menu_add_doctor(struct MedicalSystemState* system) {
    char name[TEXT_MEDIUM];
    char spec[TEXT_MEDIUM];
    char dept_id[TEXT_SMALL];
    struct Doctor* d;
    int i;
    struct Department* dept;

    printf("\n");
    ui_print_box_title("  THEM BAC SI MOI  ", UI_WIDTH);

    if (system->dept_records_array.size == 0) {
        ui_notify(MSG_ERROR, "Chua co khoa nao. Vui long them khoa truoc (menu 1).");
        return;
    }
    ui_print_row("  Danh sach khoa hien co:", UI_WIDTH);
    ui_print_dept_table_header();
    for (i = 0; i < system->dept_records_array.size; i++) {
        ui_print_dept_row(i + 1, (struct Department*)system->dept_records_array.elements[i]);
    }
    ui_print_box_end(UI_WIDTH);

    printf("  Ho va ten bac si: ");
    read_line("", name, sizeof(name));
    if (strlen(name) == 0) {
        ui_notify(MSG_ERROR, "Ten khong duoc de trong.");
        return;
    }
    printf("  Chuyen khoa (vi du: Cap cuu, Noi tiet): ");
    read_line("", spec, sizeof(spec));
    printf("  Ma khoa phan cong: ");
    read_line("", dept_id, sizeof(dept_id));

    dept = find_department(system, dept_id);
    if (dept == NULL) {
        ui_notify(MSG_ERROR, "Ma khoa khong ton tai. Vui long kiem tra lai.");
        return;
    }

    d = create_doctor(system, name, spec, dept_id);
    if (d == NULL) {
        ui_notify(MSG_ERROR, "Khong the them bac si. Vui long kiem tra lai khoa phan cong.");
        return;
    }
    {
        char msg[512];
        sprintf(msg, "Them bac si thanh cong. Ma bac si: %s - Khoa: %s", d->doctor_id, dept->dept_name);
        ui_notify(MSG_SUCCESS, msg);
    }
}

void ui_menu_add_patient(struct MedicalSystemState* system) {
    char name[TEXT_MEDIUM];
    char birth[TEXT_SMALL];
    char citizen[TEXT_SMALL];
    char phone[TEXT_SMALL];
    char email[TEXT_MEDIUM];
    char address[TEXT_MEDIUM];
    char blood[TEXT_SMALL];
    char priority_type[TEXT_SMALL];
    char allergy[TEXT_MEDIUM];
    char disease[TEXT_MEDIUM];
    int gender;
    int priority;
    struct Patient* p;

    printf("\n");
    ui_print_box_title("  THEM HO SO BENH NHAN MOI  ", UI_WIDTH);
    ui_print_row("  Vui long dien day du thong tin. Cac truong co (*) bat buoc.", UI_WIDTH);
    ui_draw_line(UI_WIDTH);

    printf("  (*) Ho va ten: ");
    read_line("", name, sizeof(name));
    if (strlen(name) == 0) {
        ui_notify(MSG_ERROR, "Ten khong duoc de trong.");
        return;
    }

    ui_read_validated_date("  (*) Ngay sinh (YYYY-MM-DD): ", birth, sizeof(birth));

    printf("  (*) Gioi tinh (0 = Nu, 1 = Nam): ");
    gender = read_int("");
    if (gender != 0 && gender != 1) {
        ui_notify(MSG_WARNING, "Gioi tinh khong hop le. Mac dinh la Nu (0).");
        gender = 0;
    }

    ui_read_validated_citizen_id("  (*) Can cuoc cong dan (dung 12 chu so): ", citizen, sizeof(citizen));
    if (find_patient_by_citizen_id(system, citizen) != NULL) {
        ui_notify(MSG_ERROR, "CCCD nay da ton tai trong ho so benh nhan. Khong the them benh nhan moi trung CCCD, du ten co khac nhau.");
        return;
    }

    ui_read_validated_phone("  (*) So dien thoai: ", phone, sizeof(phone));
    ui_read_validated_email("      Email (co the bo trong): ", email, sizeof(email));

    printf("      Dia chi thuong tru: ");
    read_line("", address, sizeof(address));

    ui_read_validated_blood("  (*) Nhom mau (A+/A-/B+/B-/AB+/AB-/O+/O-): ", blood, sizeof(blood));

    printf("  Thuoc doi tuong uu tien xa hoi? (0=Khong, 1=Co): ");
    priority = read_int("");
    if (priority != 0 && priority != 1) priority = 0;
    if (priority) {
        printf("  Loai doi tuong uu tien (vi du: Nguoi cao tuoi, Thuong binh): ");
        read_line("", priority_type, sizeof(priority_type));
    } else {
        safe_copy(priority_type, sizeof(priority_type), "Khong");
    }

    p = create_patient(system, name, birth, gender, citizen, phone, email,
                       address, blood, priority, priority_type);
    if (p == NULL) {
        ui_notify(MSG_ERROR, "Khong the tao ho so benh nhan. Vui long kiem tra CCCD va thong tin bat buoc.");
        return;
    }
    printf("  Di ung (de trong neu khong co): ");
    read_line("", allergy, sizeof(allergy));
    if (strlen(allergy) > 0) add_text_to_array(&p->allergies, allergy);

    printf("  Benh nen (de trong neu khong co): ");
    read_line("", disease, sizeof(disease));
    if (strlen(disease) > 0) add_text_to_array(&p->comorbidities, disease);

    printf("\n");
    ui_print_patient_card(p);
    ui_notify(MSG_SUCCESS, "Ho so benh nhan da duoc tao thanh cong.");
}

void ui_menu_register_exam(struct MedicalSystemState* system) {
    char patient_id[TEXT_SMALL];
    char dept_id[TEXT_SMALL];
    double spo2, temp;
    int pulse, bp;
    int i;
    struct Patient* p;
    struct Department* dept;

    printf("\n");
    ui_print_box_title("  DANG KY KHAM BENH  ", UI_WIDTH);

    if (system->dept_records_array.size == 0) {
        ui_notify(MSG_ERROR, "Chua co khoa nao. Vui long them khoa truoc.");
        return;
    }
    ui_print_row("  Danh sach khoa hien co:", UI_WIDTH);
    ui_print_dept_table_header();
    for (i = 0; i < system->dept_records_array.size; i++) {
        ui_print_dept_row(i + 1, (struct Department*)system->dept_records_array.elements[i]);
    }
    ui_print_box_end(UI_WIDTH);

    printf("  Ma benh nhan (vi du: BN001): ");
    read_line("", patient_id, sizeof(patient_id));
    p = find_patient(system, patient_id);
    if (p == NULL) {
        ui_notify(MSG_ERROR, "Khong tim thay benh nhan voi ma nay.");
        return;
    }

    ui_print_patient_card(p);

    printf("  Ma khoa dang ky kham: ");
    read_line("", dept_id, sizeof(dept_id));
    dept = find_department(system, dept_id);
    if (dept == NULL) {
        ui_notify(MSG_ERROR, "Ma khoa khong ton tai.");
        return;
    }

    printf("\n");
    ui_print_row("  NHAP CHI SO SINH TON (de xac dinh muc do khan cap):", UI_WIDTH);
    ui_draw_line(UI_WIDTH);
    spo2  = ui_read_double_range("  Do bao hoa oxy mau SpO2 (%)", 50.0, 100.0);
    pulse = ui_read_int_range   ("  Mach (lan/phut)", 20, 300);
    bp    = ui_read_int_range   ("  Huyet ap tam thu (mmHg)", 50, 300);
    temp  = ui_read_double_range("  Nhiet do co the (do C)", 34.0, 45.0);

    if (register_for_examination(system, p, dept_id, spo2, pulse, bp, temp)) {
        ui_notify(MSG_SUCCESS, "Dang ky kham thanh cong. Benh nhan da duoc them vao hang doi.");
        ui_print_queue_table(dept);
    } else {
        ui_notify(MSG_ERROR, "Dang ky kham khong thanh cong. Vui long kiem tra lai thong tin.");
    }
}

void ui_menu_call_next(struct MedicalSystemState* system) {
    char dept_id[TEXT_SMALL];
    char answer[TEXT_SMALL];
    char diagnosis[200];
    char prescription[500];
    struct Department* dept;
    struct PatientInQueue* entry;
    struct Doctor* doctor;
    char doctor_id[15];
    int i;

    printf("\n");
    ui_print_box_title("  GOI BENH NHAN TIEP THEO  ", UI_WIDTH);

    ui_print_row("  Tong quan hang doi theo khoa:", UI_WIDTH);
    ui_print_dept_table_header();
    for (i = 0; i < system->dept_records_array.size; i++) {
        ui_print_dept_row(i + 1, (struct Department*)system->dept_records_array.elements[i]);
    }
    ui_print_box_end(UI_WIDTH);

    printf("  Nhap ma khoa muon goi benh nhan: ");
    read_line("", dept_id, sizeof(dept_id));
    dept = find_department(system, dept_id);
    if (dept == NULL) {
        ui_notify(MSG_ERROR, "Ma khoa khong ton tai.");
        return;
    }

    update_priority_for_long_waiters(dept, (long)time(NULL));

    if (dept->wait_queue.size == 0) {
        ui_notify(MSG_INFO, "Hang doi khoa nay dang trong. Khong co benh nhan nao cho kham.");
        return;
    }

    ui_print_queue_table(dept);

    entry = max_heap_extract_max(&dept->wait_queue);
    if (entry == NULL) {
        ui_notify(MSG_INFO, "Hang doi dang trong.");
        return;
    }

    printf("\n");
    ui_draw_double_line(UI_WIDTH);
    {
        char buf[512];
        sprintf(buf, "  >> GOI BENH NHAN: #%d | %s | %s",
                entry->checkin_number,
                entry->root_record->patient_id,
                entry->root_record->full_name);
        ui_print_row(buf, UI_WIDTH);
        sprintf(buf, "     Muc do khan cap: %s | Diem uu tien: %d",
                priority_label(entry->medical_priority),
                entry->queue_priority_score);
        ui_print_row(buf, UI_WIDTH);
    }
    ui_draw_double_line(UI_WIDTH);

    printf("\n  Benh nhan co mat khong? (C = Co mat, K = Khong co mat): ");
    read_line("", answer, sizeof(answer));

    if (answer[0] == 'k' || answer[0] == 'K' || answer[0] == 'n' || answer[0] == 'N') {
        handle_absent_patient(dept, entry);
        ui_notify(MSG_WARNING, "Benh nhan vang mat. Da xu ly theo quy dinh.");
        return;
    }

    doctor = find_doctor_in_dept(system, dept_id);
    if (doctor != NULL) {
        char buf[128];
        safe_copy(doctor_id, sizeof(doctor_id), doctor->doctor_id);
        sprintf(buf, "Bac si phu trach: %s - %s", doctor->doctor_id, doctor->doctor_name);
        ui_notify(MSG_INFO, buf);
    } else {
        safe_copy(doctor_id, sizeof(doctor_id), "KHONG_CO");
        ui_notify(MSG_WARNING, "Khoa nay chua co bac si duoc phan cong.");
    }

    ui_print_row("  NHAP KET QUA KHAM BENH:", UI_WIDTH);
    printf("  Chan doan: ");
    read_line("", diagnosis, sizeof(diagnosis));
    printf("  Don thuoc / Huong dieu tri: ");
    read_line("", prescription, sizeof(prescription));

    add_visit(entry->root_record, doctor_id, dept_id, diagnosis, prescription);
    ui_notify(MSG_SUCCESS, "Luot kham hoan tat. Ket qua da luu vao lich su benh nhan.");
    free(entry);
}

void ui_menu_show_queue(struct MedicalSystemState* system) {
    char dept_id[TEXT_SMALL];
    int i;
    struct Department* dept;

    printf("\n");
    ui_print_box_title("  XEM HANG DOI KHOA  ", UI_WIDTH);

    ui_print_row("  Danh sach khoa:", UI_WIDTH);
    ui_print_dept_table_header();
    for (i = 0; i < system->dept_records_array.size; i++) {
        ui_print_dept_row(i + 1, (struct Department*)system->dept_records_array.elements[i]);
    }
    ui_print_box_end(UI_WIDTH);

    printf("  Nhap ma khoa can xem hang doi: ");
    read_line("", dept_id, sizeof(dept_id));
    dept = find_department(system, dept_id);
    ui_print_queue_table(dept);
}

void ui_menu_list_patients(struct MedicalSystemState* system) {
    int i;
    struct Patient* p;
    char search[TEXT_MEDIUM];
    int show_all;

    printf("\n");
    ui_print_box_title("  DANH SACH BENH NHAN  ", UI_WIDTH);

    if (system->patient_records_array.size == 0) {
        ui_notify(MSG_INFO, "Chua co ho so benh nhan nao trong he thong.");
        return;
    }

    printf("  Tim kiem theo ten (Enter de xem tat ca): ");
    read_line("", search, sizeof(search));
    show_all = (strlen(search) == 0);

    ui_print_patient_table_header();
    for (i = 0; i < system->patient_records_array.size; i++) {
        p = (struct Patient*)system->patient_records_array.elements[i];
        if (show_all || strstr(p->full_name, search) != NULL) {
            ui_print_patient_row(i + 1, p);
        }
    }
    ui_print_patient_table_footer(system->patient_records_array.size);

    {
        char choice[TEXT_SMALL];
        printf("\n  Nhap ma benh nhan de xem chi tiet (Enter de bo qua): ");
        read_line("", choice, sizeof(choice));
        if (strlen(choice) > 0) {
            p = find_patient(system, choice);
            if (p != NULL) {
                ui_print_patient_card(p);
                ui_print_visit_history(p);
            } else {
                ui_notify(MSG_ERROR, "Khong tim thay benh nhan voi ma nay.");
            }
        }
    }
}

void ui_menu_list_doctors(struct MedicalSystemState* system) {
    int i;
    struct Doctor* d;
    int j;
    struct Department* dept;

    printf("\n");
    ui_print_box_title("  DANH SACH BAC SI  ", UI_WIDTH);

    if (system->doctor_records_array.size == 0) {
        ui_notify(MSG_INFO, "Chua co bac si nao trong he thong.");
        return;
    }

    for (i = 0; i < system->dept_records_array.size; i++) {
        dept = (struct Department*)system->dept_records_array.elements[i];
        {
            char buf[512];
            sprintf(buf, "  KHOA: %s - %s", dept->dept_id, dept->dept_name);
            ui_print_row(buf, UI_WIDTH);
        }
        ui_print_doctor_table_header();
        j = 0;
        for (j = 0; j < system->doctor_records_array.size; j++) {
            d = (struct Doctor*)system->doctor_records_array.elements[j];
            if (strcmp(d->assigned_dept_id, dept->dept_id) == 0) {
                ui_print_doctor_row(j + 1, d);
            }
        }
        ui_print_box_end(UI_WIDTH);
        printf("\n");
    }

    {
        int has_unassigned = 0;
        for (j = 0; j < system->doctor_records_array.size; j++) {
            d = (struct Doctor*)system->doctor_records_array.elements[j];
            if (find_department(system, d->assigned_dept_id) == NULL) {
                if (!has_unassigned) {
                    ui_print_row("  BAC SI CHUA DUOC PHAN CONG KHOA:", UI_WIDTH);
                    ui_print_doctor_table_header();
                    has_unassigned = 1;
                }
                ui_print_doctor_row(j + 1, d);
            }
        }
        if (has_unassigned) ui_print_box_end(UI_WIDTH);
    }
}

void ui_menu_list_departments(struct MedicalSystemState* system) {
    int i;

    printf("\n");
    ui_print_box_title("  DANH SACH KHOA PHONG  ", UI_WIDTH);

    if (system->dept_records_array.size == 0) {
        ui_notify(MSG_INFO, "Chua co khoa nao duoc them vao he thong.");
        return;
    }

    ui_print_dept_table_header();
    for (i = 0; i < system->dept_records_array.size; i++) {
        ui_print_dept_row(i + 1, (struct Department*)system->dept_records_array.elements[i]);
    }
    ui_print_box_end(UI_WIDTH);
    ui_print_system_summary(system);
}

void ui_menu_daily_statistics(struct MedicalSystemState* system) {
    ui_print_daily_stats(system);
}
