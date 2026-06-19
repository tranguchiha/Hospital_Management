#include "hospital.h"

static struct PatientInQueue* find_active_queue_entry(struct Department* dept, const char* patient_id);

void rebuild_heap(struct MaxHeap* heap){
    int i;
    for (i = heap->size / 2 - 1; i >= 0; i--){
        max_heapify(heap, i);
        if (i == 0){
            break;
        }
    }
}


int calculate_queue_priority_score(struct PatientInQueue* entry, long current_time){
    int score;
    (void)current_time;
    if (entry == NULL){
        return 0;
    }

    /*
       Quy uoc muc do khan cap:
       1 = nguy cap nhat, 5 = nhe nhat.
       Hang doi van dung heap cuc dai, nen diem uu tien duoc dao lai:
       muc 1 duoc diem nen cao nhat, muc 5 duoc diem nen thap nhat.

       Da bo co che cong diem sau 60 phut cho.
       Diem uu tien hien chi phu thuoc vao muc do y te va doi tuong uu tien xa hoi.
    */
    score = (MEDICAL_PRIORITY_LOWEST + 1) - entry->medical_priority;

    if (entry->root_record != NULL && entry->root_record->is_priority_object){
        score += SOCIAL_PRIORITY_BONUS;
    }

    return score;
}


int assign_medical_priority(double spo2, int pulse, int systolic_bp, double temperature){
    /* 1 la nguy cap nhat, 5 la nhe nhat. */
    if (spo2 < 90.0 || pulse >= 130 || systolic_bp < 90 || systolic_bp >= 180 || temperature >= 40.0){
        return 1;
    }
    if (spo2 < 94.0 || pulse >= 110 || systolic_bp < 100 || systolic_bp >= 160 || temperature >= 39.0){
        return 2;
    }
    if (spo2 < 96.0 || pulse >= 100 || systolic_bp >= 140 || temperature >= 38.0){
        return 3;
    }
    if (spo2 < 98.0 || pulse < 60 || pulse >= 90 || systolic_bp < 110 || systolic_bp >= 130 || temperature >= 37.5){
        return 4;
    }
    return 5;
}


void register_for_examination(struct MedicalSystemState* system, struct Patient* patient_ptr, const char* dept_id, double spo2, int pulse, int bp, double temp){
    struct Department* dept;
    struct PatientInQueue* entry;
    int medical_priority;
    dept = find_department(system, dept_id);
    if (dept == NULL){
        printf("Khong tim thay khoa: %s\n", dept_id);
        return;
    }
    if (patient_ptr == NULL){
        printf("Khong tim thay benh nhan.\n");
        return;
    }
    if (find_active_queue_entry(dept, patient_ptr->patient_id) != NULL){
        printf("Benh nhan %s da co phien dang ky dang cho tai khoa %s. Khong the dang ky trung.\n", patient_ptr->patient_id, dept_id);
        return;
    }
    medical_priority = assign_medical_priority(spo2, pulse, bp, temp);
    entry = (struct PatientInQueue*)malloc(sizeof(struct PatientInQueue));
    if (entry == NULL){
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    entry->root_record = patient_ptr;
    entry->medical_priority = medical_priority;
    entry->registration_time = (long)time(NULL);
    entry->queue_priority_score = calculate_queue_priority_score(entry, entry->registration_time);
    entry->queue_status = 0;
    safe_copy(entry->current_dept_id, sizeof(entry->current_dept_id), dept_id);
    entry->checkin_number = system->next_checkin_number;
    system->next_checkin_number++;
    entry->absent_count = 0;
    max_heap_insert(&dept->wait_queue, entry);
    printf("Dang ky thanh cong. Benh nhan %s - So dang ky #%d - Muc do khan cap %d - Diem uu tien %d\n", patient_ptr->patient_id, entry->checkin_number, entry->medical_priority, entry->queue_priority_score);
}


void update_priority_for_long_waiters(struct Department* dept, long current_time){
    int i;
    if (dept == NULL){
        return;
    }
    for (i = 0; i < dept->wait_queue.size; i++){
        dept->wait_queue.heap_array[i]->queue_priority_score =
            calculate_queue_priority_score(dept->wait_queue.heap_array[i], current_time);
    }
    rebuild_heap(&dept->wait_queue);
}


void handle_absent_patient(struct Department* dept, struct PatientInQueue* entry){
    if (dept == NULL || entry == NULL){
        return;
    }
    entry->absent_count++;
    entry->queue_status = 2;
    if (entry->absent_count >= MAX_ABSENT_COUNT){
        printf("Benh nhan %s da vang mat %d lan, phien dang ky bi huy khoi hang doi.\n", entry->root_record->patient_id, MAX_ABSENT_COUNT);
        free(entry);
        return;
    }
    entry->queue_priority_score = calculate_queue_priority_score(entry, (long)time(NULL));
    printf("Benh nhan %s duoc danh dau vang mat. Da dua tro lai hang doi. So lan vang: %d/%d.\n", entry->root_record->patient_id, entry->absent_count, MAX_ABSENT_COUNT);
    entry->queue_status = 0;
    max_heap_insert(&dept->wait_queue, entry);
}


static int visit_count_of_void(void* p){
    struct Patient* patient;
    patient = (struct Patient*)p;
    return patient->visit_history.size;
}


static int compare_patient_for_stat(void* a, void* b){
    struct Patient* pa;
    struct Patient* pb;
    int va;
    int vb;
    pa = (struct Patient*)a;
    pb = (struct Patient*)b;
    va = visit_count_of_void(a);
    vb = visit_count_of_void(b);
    if (va != vb){
        return vb - va;
    }
    return strcmp(pa->patient_id, pb->patient_id);
}


int hoare_partition(void** arr, int low, int high){
    void* pivot;
    int i;
    int j;
    pivot = arr[(low + high) / 2];
    i = low - 1;
    j = high + 1;
    while (1){
        do{
            i++;
        } while (compare_patient_for_stat(arr[i], pivot) < 0);

        do{
            j--;
        } while (compare_patient_for_stat(arr[j], pivot) > 0);

        if (i >= j){
            return j;
        }
        swap_ptrs(&arr[i], &arr[j]);
    }
}


void quick_sort_patients(void** arr, int low, int high){
    int p;
    if (low < high){
        p = hoare_partition(arr, low, high);
        quick_sort_patients(arr, low, p);
        quick_sort_patients(arr, p + 1, high);
    }
}


void safe_copy(char* dest, size_t dest_size, const char* src){
    size_t i;
    if (dest_size == 0){
        return;
    }
    if (src == NULL){
        src = "";
    }
    i = 0;
    while (i + 1 < dest_size && src[i] != '\0'){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static void normalize_for_compare(const char* src, char* dest, size_t dest_size){
    size_t i;
    size_t j;
    int previous_space;

    if (dest == NULL || dest_size == 0){
        return;
    }
    if (src == NULL){
        src = "";
    }

    i = 0;
    j = 0;
    previous_space = TRUE;
    while (src[i] != '\0' && j + 1 < dest_size){
        unsigned char ch = (unsigned char)src[i];
        if (isspace(ch)){
            if (!previous_space){
                dest[j++] = ' ';
                previous_space = TRUE;
            }
        } else {
            dest[j++] = (char)tolower(ch);
            previous_space = FALSE;
        }
        i++;
    }
    if (j > 0 && dest[j - 1] == ' '){
        j--;
    }
    dest[j] = '\0';
}


static int text_equal_normalized(const char* a, const char* b){
    char na[TEXT_LARGE];
    char nb[TEXT_LARGE];
    normalize_for_compare(a, na, sizeof(na));
    normalize_for_compare(b, nb, sizeof(nb));
    return strcmp(na, nb) == 0;
}


static void format_patient_id_short(const char* input, char* output, size_t output_size){
    int number;
    char temp[TEXT_SMALL];

    if (output == NULL || output_size == 0){
        return;
    }

    if (input != NULL && strncmp(input, "BN", 2) == 0 && strlen(input) > 2){
        number = atoi(input + 2);
        if (number > 0){
            sprintf(temp, "BN%03d", number);
            safe_copy(output, output_size, temp);
            return;
        }
    }

    safe_copy(output, output_size, input);
}


char* duplicate_text(const char* s){
    size_t len;
    char* out;
    if (s == NULL){
        s = "";
    }
    len = strlen(s);
    out = (char*)malloc(len + 1);
    if (out == NULL){
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    strcpy(out, s);
    return out;
}


void read_line(const char* prompt, char* buffer, int size){
    size_t len;
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) == NULL){
        buffer[0] = '\0';
        return;
    }
    len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n'){
        buffer[len - 1] = '\0';
    }
}


int read_int(const char* prompt){
    char buffer[INPUT_SIZE];
    int value;
    while (1) {
        read_line(prompt, buffer, sizeof(buffer));
        if (sscanf(buffer, "%d", &value) == 1){
            return value;
        }
        printf("Gia tri khong hop le, vui long nhap lai.\n");
    }
}


double read_double(const char* prompt){
    char buffer[INPUT_SIZE];
    double value;
    while (1) {
        read_line(prompt, buffer, sizeof(buffer));
        if (sscanf(buffer, "%lf", &value) == 1){
            return value;
        }
        printf("Gia tri khong hop le, vui long nhap lai.\n");
    }
}


void press_enter(void){
    char buffer[INPUT_SIZE];
    read_line("Bam Enter de tiep tuc...", buffer, sizeof(buffer));
}


void add_text_to_array(struct DynamicArray* arr, const char* text){
    if (text != NULL && strlen(text) > 0) {
        dynamic_array_append(arr, duplicate_text(text));
    }
}


void system_state_init(struct MedicalSystemState* system){
    dynamic_array_init(&system->patient_records_array, 8);
    dynamic_array_init(&system->doctor_records_array, 8);
    dynamic_array_init(&system->dept_records_array, 8);
    system->next_patient_id_counter = 1;
    system->next_doctor_id_counter = 1;
    system->next_checkin_number = 1;
}


void system_state_free(struct MedicalSystemState* system){
    int i;
    int j;
    struct Patient* p;
    struct Department* d;
    for (i = 0; i < system->patient_records_array.size; i++){
        p = (struct Patient*)system->patient_records_array.elements[i];
        for (j = 0; j < p->allergies.size; j++){
            free(p->allergies.elements[j]);
        }
        dynamic_array_free(&p->allergies);

        for (j = 0; j < p->comorbidities.size; j++){
            free(p->comorbidities.elements[j]);
        }
        dynamic_array_free(&p->comorbidities);
        for (j = 0; j < p->visit_history.size; j++){
            free(p->visit_history.elements[j]);
        }
        dynamic_array_free(&p->visit_history);
        free(p);
    }
    dynamic_array_free(&system->patient_records_array);
    for (i = 0; i < system->doctor_records_array.size; i++){
        free(system->doctor_records_array.elements[i]);
    }
    dynamic_array_free(&system->doctor_records_array);
    for (i = 0; i < system->dept_records_array.size; i++){
        d = (struct Department*)system->dept_records_array.elements[i];
        max_heap_free(&d->wait_queue, TRUE);
        free(d);
    }
    dynamic_array_free(&system->dept_records_array);
}


struct Department* find_department(struct MedicalSystemState* system, const char* dept_id){
    int i;
    struct Department* d;
    for (i = 0; i < system->dept_records_array.size; i++){
        d = (struct Department*)system->dept_records_array.elements[i];
        if (strcmp(d->dept_id, dept_id) == 0) {
            return d;
        }
    }
    return NULL;
}


struct Department* find_department_by_name(struct MedicalSystemState* system, const char* dept_name){
    int i;
    struct Department* d;
    if (dept_name == NULL || strlen(dept_name) == 0){
        return NULL;
    }
    for (i = 0; i < system->dept_records_array.size; i++){
        d = (struct Department*)system->dept_records_array.elements[i];
        if (text_equal_normalized(d->dept_name, dept_name)){
            return d;
        }
    }
    return NULL;
}


struct Patient* find_patient(struct MedicalSystemState* system, const char* patient_id){
    int i;
    struct Patient* p;
    char normalized_id[TEXT_SMALL];

    format_patient_id_short(patient_id, normalized_id, sizeof(normalized_id));

    for (i = 0; i < system->patient_records_array.size; i++){
        p = (struct Patient*)system->patient_records_array.elements[i];
        if (strcmp(p->patient_id, patient_id) == 0 || strcmp(p->patient_id, normalized_id) == 0){
            return p;
        }
    }
    return NULL;
}


struct Patient* find_patient_by_citizen_id(struct MedicalSystemState* system, const char* citizen_id){
    int i;
    struct Patient* p;
    if (citizen_id == NULL || strlen(citizen_id) == 0){
        return NULL;
    }
    for (i = 0; i < system->patient_records_array.size; i++){
        p = (struct Patient*)system->patient_records_array.elements[i];
        if (strcmp(p->citizen_id, citizen_id) == 0){
            return p;
        }
    }
    return NULL;
}


static struct PatientInQueue* find_active_queue_entry(struct Department* dept, const char* patient_id){
    int i;
    struct PatientInQueue* e;
    if (dept == NULL || patient_id == NULL){
        return NULL;
    }
    for (i = 0; i < dept->wait_queue.size; i++){
        e = dept->wait_queue.heap_array[i];
        if (e != NULL && e->root_record != NULL && e->queue_status == 0 && strcmp(e->root_record->patient_id, patient_id) == 0){
            return e;
        }
    }
    return NULL;
}


struct Doctor* find_doctor_in_dept(struct MedicalSystemState* system, const char* dept_id){
    int i;
    struct Doctor* d;
    for (i = 0; i < system->doctor_records_array.size; i++){
        d = (struct Doctor*)system->doctor_records_array.elements[i];
        if (strcmp(d->assigned_dept_id, dept_id) == 0){
            return d;
        }
    }
    return NULL;
}


struct Department* create_department(const char* dept_id, const char* dept_name){
    struct Department* d;
    d = (struct Department*)malloc(sizeof(struct Department));
    if (d == NULL) {
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    safe_copy(d->dept_id, sizeof(d->dept_id), dept_id);
    safe_copy(d->dept_name, sizeof(d->dept_name), dept_name);
    max_heap_init(&d->wait_queue, 8);
    return d;
}


struct Doctor* create_doctor(struct MedicalSystemState* system, const char* name, const char* specialization, const char* dept_id){
    struct Doctor* d;
    d = (struct Doctor*)malloc(sizeof(struct Doctor));
    if (d == NULL){
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    sprintf(d->doctor_id, "BS%06d", system->next_doctor_id_counter);
    system->next_doctor_id_counter++;
    safe_copy(d->doctor_name, sizeof(d->doctor_name), name);
    safe_copy(d->specialization, sizeof(d->specialization), specialization);
    safe_copy(d->assigned_dept_id, sizeof(d->assigned_dept_id), dept_id);
    dynamic_array_append(&system->doctor_records_array, d);
    return d;
}


struct Patient* create_patient(struct MedicalSystemState* system, const char* name, const char* birth, int gender, const char* citizen_id, const char* phone, const char* email, const char* address, const char* blood, int priority_object, const char* priority_type) {
    struct Patient* p;
    if (find_patient_by_citizen_id(system, citizen_id) != NULL) {
        printf("CCCD nay da ton tai trong ho so benh nhan. Khong the them benh nhan moi trung CCCD.\n");
        return NULL;
    }
    p = (struct Patient*)malloc(sizeof(struct Patient));
    if (p == NULL) {
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    sprintf(p->patient_id, "BN%03d", system->next_patient_id_counter);
    system->next_patient_id_counter++;
    safe_copy(p->full_name, sizeof(p->full_name), name);
    safe_copy(p->birth_date, sizeof(p->birth_date), birth);
    p->gender = gender;
    safe_copy(p->citizen_id, sizeof(p->citizen_id), citizen_id);
    safe_copy(p->phone_number, sizeof(p->phone_number), phone);
    safe_copy(p->email, sizeof(p->email), email);
    safe_copy(p->address, sizeof(p->address), address);
    safe_copy(p->blood_type, sizeof(p->blood_type), blood);
    dynamic_array_init(&p->allergies, 2);
    dynamic_array_init(&p->comorbidities, 2);
    p->is_priority_object = priority_object ? TRUE : FALSE;
    safe_copy(p->priority_type, sizeof(p->priority_type), priority_type);
    dynamic_array_init(&p->visit_history, 4);
    dynamic_array_append(&system->patient_records_array, p);
    return p;
}


void add_visit(struct Patient* p, const char* doctor_id, const char* dept_id, const char* diagnosis, const char* prescription){
    struct MedicalVisit* visit;
    time_t now;
    struct tm* tm_info;
    visit = (struct MedicalVisit*)malloc(sizeof(struct MedicalVisit));
    if (visit == NULL){
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    sprintf(visit->visit_id, "LK%06d", p->visit_history.size + 1);
    now = time(NULL);
    tm_info = localtime(&now);
    if (tm_info != NULL) {
        strftime(visit->visit_date, sizeof(visit->visit_date), "%Y-%m-%d", tm_info);
    } else {
        safe_copy(visit->visit_date, sizeof(visit->visit_date), "Khong xac dinh.");
    }
    safe_copy(visit->diagnosis, sizeof(visit->diagnosis), diagnosis);
    safe_copy(visit->prescription, sizeof(visit->prescription), prescription);
    safe_copy(visit->doctor_id, sizeof(visit->doctor_id), doctor_id);
    safe_copy(visit->department_id, sizeof(visit->department_id), dept_id);
    dynamic_array_append(&p->visit_history, visit);
}


void print_patient(const struct Patient* p){
    int i;
    printf("Ma: %s | Ten: %s | Ngay sinh: %s | Gioi tinh: %s | So dien thoai: %s | Nhom mau: %s | So lan kham: %d\n",
           p->patient_id, p->full_name, p->birth_date,
           p->gender ? "Nam" : "Nu", p->phone_number, p->blood_type, p->visit_history.size);

    if (p->is_priority_object){
        printf("  Uu tien xa hoi: %s\n", p->priority_type);
    }
    if (p->allergies.size > 0) {
        printf("  Di ung: ");
        for (i = 0; i < p->allergies.size; i++) {
            printf("%s%s", (char*)p->allergies.elements[i], i + 1 < p->allergies.size ? ", " : "");
        }
        printf("\n");
    }
    if (p->comorbidities.size > 0) {
        printf("  Benh nen: ");
        for (i = 0; i < p->comorbidities.size; i++) {
            printf("%s%s", (char*)p->comorbidities.elements[i], i + 1 < p->comorbidities.size ? ", " : "");
        }
        printf("\n");
    }
}


void print_queue_of_department(struct Department* dept) {
    int i;
    struct PatientInQueue* e;
    if (dept == NULL) {
        printf("Khong tim thay khoa.\n");
        return;
    }
    update_priority_for_long_waiters(dept, (long)time(NULL));
    printf("Hang doi %s - %s: %d benh nhan\n", dept->dept_id, dept->dept_name, dept->wait_queue.size);
    for (i = 0; i < dept->wait_queue.size; i++) {
        e = dept->wait_queue.heap_array[i];
        printf("  #%d | %s | %s | muc do khan cap=%d | diem uu tien=%d | so lan vang=%d\n", e->checkin_number, e->root_record->patient_id, e->root_record->full_name, e->medical_priority, e->queue_priority_score, e->absent_count);
    }
}


void menu_add_department(struct MedicalSystemState* system) {
    char id[TEXT_SMALL];
    char name[TEXT_MEDIUM];
    read_line("Ma khoa, toi da 6 ky tu: ", id, sizeof(id));
    read_line("Ten khoa: ", name, sizeof(name));
    if (strlen(id) == 0 || strlen(name) == 0) {
        printf("Ma va ten khong duoc de trong.\n");
        return;
    }
    if (find_department(system, id) != NULL) {
        printf("Ma khoa nay da ton tai.\n");
        return;
    }
    if (find_department_by_name(system, name) != NULL) {
        printf("Ten khoa nay da ton tai. Vui long dung ten khac de tranh trung khoa.\n");
        return;
    }
    dynamic_array_append(&system->dept_records_array, create_department(id, name));
    printf("Them khoa thanh cong.\n");
}


void menu_add_doctor(struct MedicalSystemState* system) {
    char name[TEXT_MEDIUM];
    char spec[TEXT_MEDIUM];
    char dept_id[TEXT_SMALL];
    struct Doctor* d;
    read_line("Ten bac si: ", name, sizeof(name));
    read_line("Nhap chuyen khoa: ", spec, sizeof(spec));
    read_line("Ma khoa duoc phan cong: ", dept_id, sizeof(dept_id));
    if (find_department(system, dept_id) == NULL) {
        printf("Khoa khong ton tai. Vui long them khoa truoc.\n");
        return;
    }
    d = create_doctor(system, name, spec, dept_id);
    printf("Them bac si thanh cong. Ma bac si: %s\n", d->doctor_id);
}


void menu_add_patient(struct MedicalSystemState* system) {
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
    read_line("Ten benh nhan: ", name, sizeof(name));
    read_line("Ngay sinh: ", birth, sizeof(birth));
    gender = read_int("Gioi tinh 0=Nu, 1=Nam: ");
    read_line("Can cuoc cong dan: ", citizen, sizeof(citizen));
    read_line("So dien thoai: ", phone, sizeof(phone));
    read_line("Email: ", email, sizeof(email));
    read_line("Dia chi: ", address, sizeof(address));
    read_line("Nhom mau: ", blood, sizeof(blood));
    if (strlen(name) == 0 || strlen(citizen) == 0) {
        printf("Ten benh nhan va CCCD khong duoc de trong.\n");
        return;
    }
    if (find_patient_by_citizen_id(system, citizen) != NULL) {
        printf("CCCD nay da ton tai trong ho so benh nhan. Khong the them benh nhan moi trung CCCD.\n");
        return;
    }
    priority = read_int("Thuoc doi tuong uu tien xa hoi? 0=Khong, 1=Co: ");
    if (priority) {
        read_line("Loai doi tuong uu tien: ", priority_type, sizeof(priority_type));
    } else {
        safe_copy(priority_type, sizeof(priority_type), "Trong");
    }
    p = create_patient(system, name, birth, gender, citizen, phone, email, address, blood, priority, priority_type);
    if (p == NULL) {
        return;
    }
    read_line("Di ung, de trong neu khong co: ", allergy, sizeof(allergy));
    add_text_to_array(&p->allergies, allergy);
    read_line("Benh nen, de trong neu khong co: ", disease, sizeof(disease));
    add_text_to_array(&p->comorbidities, disease);
    printf("Them benh nhan thanh cong. Ma benh nhan: %s\n", p->patient_id);
}


void menu_register_exam(struct MedicalSystemState* system) {
    char patient_id[TEXT_SMALL];
    char dept_id[TEXT_SMALL];
    double spo2;
    int pulse;
    int bp;
    double temp;
    struct Patient* p;
    read_line("Ma benh nhan: ", patient_id, sizeof(patient_id));
    p = find_patient(system, patient_id);
    if (p == NULL) {
        printf("Benh nhan khong ton tai.\n");
        return;
    }
    read_line("Ma khoa: ", dept_id, sizeof(dept_id));
    if (find_department(system, dept_id) == NULL) {
        printf("Khoa khong ton tai.\n");
        return;
    }
    spo2 = read_double("Do bao hoa oxy mau SpO2 (%): ");
    pulse = read_int("Mach (lan/phut): ");
    bp = read_int("Huyet ap tam thu (mmHg): ");
    temp = read_double("Nhiet do (do C): ");
    register_for_examination(system, p, dept_id, spo2, pulse, bp, temp);
}


void menu_call_next(struct MedicalSystemState* system) {
    char dept_id[TEXT_SMALL];
    char answer[TEXT_SMALL];
    char diagnosis[200];
    char prescription[500];
    struct Department* dept;
    struct PatientInQueue* entry;
    struct Doctor* doctor;
    char doctor_id[15];
    read_line("Ma khoa: ", dept_id, sizeof(dept_id));
    dept = find_department(system, dept_id);
    if (dept == NULL) {
        printf("Ma khoa khong ton tai.\n");
        return;
    }
    update_priority_for_long_waiters(dept, (long)time(NULL));
    entry = max_heap_extract_max(&dept->wait_queue);
    if (entry == NULL) {
        printf("Hang doi dang trong.\n");
        return;
    }
    printf("Dang goi benh nhan: #%d | %s | %s | diem uu tien=%d\n", entry->checkin_number, entry->root_record->patient_id, entry->root_record->full_name, entry->queue_priority_score);
    read_line("Benh nhan co mat khong? (C: Co, K: Khong): ", answer, sizeof(answer));
    if (answer[0] == 'k' || answer[0] == 'K' || answer[0] == 'n' || answer[0] == 'N') {
        handle_absent_patient(dept, entry);
        return;
    }
    doctor = find_doctor_in_dept(system, dept_id);
    if (doctor != NULL) {
        safe_copy(doctor_id, sizeof(doctor_id), doctor->doctor_id);
        printf("Bac si phu trach: %s - %s\n", doctor->doctor_id, doctor->doctor_name);
    } else {
        safe_copy(doctor_id, sizeof(doctor_id), "KHONG_CO");
        printf("Khoa nay chua co bac si. Tam gan ma bac si la KHONG_CO.\n");
    }
    read_line("Chan doan: ", diagnosis, sizeof(diagnosis));
    read_line("Don thuoc: ", prescription, sizeof(prescription));
    add_visit(entry->root_record, doctor_id, dept_id, diagnosis, prescription);
    printf("Luot kham da hoan tat va duoc luu vao lich su benh nhan.\n");
    free(entry);
}


void menu_show_queue(struct MedicalSystemState* system) {
    char dept_id[TEXT_SMALL];
    read_line("Ma khoa: ", dept_id, sizeof(dept_id));
    print_queue_of_department(find_department(system, dept_id));
}


void menu_list_patients(struct MedicalSystemState* system) {
    int i;
    printf("Danh sach benh nhan: %d nguoi\n", system->patient_records_array.size);
    for (i = 0; i < system->patient_records_array.size; i++) {
        print_patient((struct Patient*)system->patient_records_array.elements[i]);
    }
}


void menu_list_doctors(struct MedicalSystemState* system) {
    int i;
    struct Doctor* d;
    printf("Danh sach bac si: %d bac si\n", system->doctor_records_array.size);
    for (i = 0; i < system->doctor_records_array.size; i++) {
        d = (struct Doctor*)system->doctor_records_array.elements[i];
        printf("Ma: %s | Ten: %s | Chuyen khoa: %s | Khoa: %s\n", d->doctor_id, d->doctor_name, d->specialization, d->assigned_dept_id);
    }
}


void menu_list_departments(struct MedicalSystemState* system) {
    int i;
    struct Department* d;
    printf("Danh sach khoa: %d khoa\n", system->dept_records_array.size);
    for (i = 0; i < system->dept_records_array.size; i++) {
        d = (struct Department*)system->dept_records_array.elements[i];
        printf("Ma: %s | Ten: %s | Cho kham: %d\n", d->dept_id, d->dept_name, d->wait_queue.size);
    }
}


void menu_daily_statistics(struct MedicalSystemState* system) {
    void** copy_arr;
    int i;
    struct Patient* p;
    if (system->patient_records_array.size == 0) {
        printf("Khong co benh nhan.\n");
        return;
    }
    copy_arr = (void**)malloc(sizeof(void*) * (size_t)system->patient_records_array.size);
    if (copy_arr == NULL) {
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    for (i = 0; i < system->patient_records_array.size; i++) {
        copy_arr[i] = system->patient_records_array.elements[i];
    }
    quick_sort_patients(copy_arr, 0, system->patient_records_array.size - 1);
    printf("Thong ke trong ngay theo so luot kham giam dan:\n");
    for (i = 0; i < system->patient_records_array.size; i++) {
        p = (struct Patient*)copy_arr[i];
        printf("%d. %s | %s | so luot kham=%d\n", i + 1, p->patient_id, p->full_name, p->visit_history.size);
    }

    free(copy_arr);
}


int file_exists(const char* filename) {
    FILE* f;
    if (filename == NULL || filename[0] == '\0') {
        return FALSE;
    }
    f = fopen(filename, "r");
    if (f == NULL) {
        return FALSE;
    }
    fclose(f);
    return TRUE;
}


void build_path_from_exe(char* dest, int dest_size, const char* argv0, const char* filename) {
    int i;
    int last_sep;
    int pos;
    if (dest == NULL || dest_size <= 0) {
        return;
    }
    dest[0] = '\0';
    if (argv0 == NULL || filename == NULL) {
        return;
    }
    last_sep = -1;
    for (i = 0; argv0[i] != '\0'; i++) {
        if (argv0[i] == '/' || argv0[i] == '\\') {
            last_sep = i;
        }
    }
    if (last_sep < 0) {
        safe_copy(dest, (size_t)dest_size, filename);
        return;
    }
    pos = 0;
    for (i = 0; i <= last_sep && pos < dest_size - 1; i++) {
        dest[pos] = argv0[i];
        pos++;
    }
    for (i = 0; filename[i] != '\0' && pos < dest_size - 1; i++) {
        dest[pos] = filename[i];
        pos++;
    }
    dest[pos] = '\0';
}


void build_sibling_path(char* dest, int dest_size, const char* source_path, const char* filename) {
    int i;
    int last_sep;
    int pos;
    if (dest == NULL || dest_size <= 0) {
        return;
    }
    dest[0] = '\0';
    if (source_path == NULL || filename == NULL) {
        return;
    }
    last_sep = -1;
    for (i = 0; source_path[i] != '\0'; i++) {
        if (source_path[i] == '/' || source_path[i] == '\\') {
            last_sep = i;
        }
    }
    if (last_sep < 0) {
        safe_copy(dest, (size_t)dest_size, filename);
        return;
    }
    pos = 0;
    for (i = 0; i <= last_sep && pos < dest_size - 1; i++) {
        dest[pos] = source_path[i];
        pos++;
    }
    for (i = 0; filename[i] != '\0' && pos < dest_size - 1; i++) {
        dest[pos] = filename[i];
        pos++;
    }
    dest[pos] = '\0';
}


void choose_input_file(char* dest, int dest_size, const char* argv0) {
    char exe_input[INPUT_SIZE];

    if (dest == NULL || dest_size <= 0) {
        return;
    }

    safe_copy(dest, (size_t)dest_size, "input.txt");
    if (file_exists(dest)) {
        return;
    }

    build_path_from_exe(exe_input, sizeof(exe_input), argv0, "input.txt");
    if (file_exists(exe_input)) {
        safe_copy(dest, (size_t)dest_size, exe_input);
        return;
    }

    printf("Khong tim thay tep input.txt. Hay dat input.txt cung thu muc dang chay hoac cung thu muc voi file exe.\n");
    exit(1);
}


void choose_output_file(char* dest, int dest_size, const char* input_path) {
    if (dest == NULL || dest_size <= 0) {
        return;
    }
    build_sibling_path(dest, dest_size, input_path, "output.txt");
}


void trim_newline(char* s) {
    size_t len;
    if (s == NULL) {
        return;
    }
    len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}


int split_fields(char* line, char fields[][TEXT_LARGE], int max_fields) {
    int count;
    int j;
    char* p;
    if (line == NULL || max_fields <= 0) {
        return 0;
    }
    count = 0;
    j = 0;
    fields[0][0] = '\0';
    p = line;
    while (*p != '\0') {
        if (*p == '|') {
            fields[count][j] = '\0';
            count++;
            if (count >= max_fields) {
                return count;
            }
            j = 0;
            fields[count][0] = '\0';
        } else {
            if (j < TEXT_LARGE - 1) {
                fields[count][j] = *p;
                j++;
            }
        }
        p++;
    }
    fields[count][j] = '\0';
    return count + 1;
}


static int id_number_after_prefix(const char* id, int prefix_len) {
    if (id == NULL || (int)strlen(id) <= prefix_len) {
        return 0;
    }
    return atoi(id + prefix_len);
}


static struct Patient* create_patient_from_file(struct MedicalSystemState* system, char fields[][TEXT_LARGE]) {
    struct Patient* p;
    int number;
    p = (struct Patient*)malloc(sizeof(struct Patient));
    if (p == NULL) {
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    format_patient_id_short(fields[0], p->patient_id, sizeof(p->patient_id));
    safe_copy(p->full_name, sizeof(p->full_name), fields[1]);
    safe_copy(p->birth_date, sizeof(p->birth_date), fields[2]);
    p->gender = atoi(fields[3]);
    safe_copy(p->citizen_id, sizeof(p->citizen_id), fields[4]);
    safe_copy(p->phone_number, sizeof(p->phone_number), fields[5]);
    safe_copy(p->email, sizeof(p->email), fields[6]);
    safe_copy(p->address, sizeof(p->address), fields[7]);
    safe_copy(p->blood_type, sizeof(p->blood_type), fields[8]);
    p->is_priority_object = atoi(fields[9]) ? TRUE : FALSE;
    safe_copy(p->priority_type, sizeof(p->priority_type), fields[10]);
    dynamic_array_init(&p->allergies, 2);
    dynamic_array_init(&p->comorbidities, 2);
    dynamic_array_init(&p->visit_history, 4);
    dynamic_array_append(&system->patient_records_array, p);
    number = id_number_after_prefix(p->patient_id, 2);
    if (number >= system->next_patient_id_counter) {
        system->next_patient_id_counter = number + 1;
    }
    return p;
}


static void add_visit_from_file(struct Patient* p, char fields[][TEXT_LARGE]) {
    struct MedicalVisit* visit;
    visit = (struct MedicalVisit*)malloc(sizeof(struct MedicalVisit));
    if (visit == NULL) {
        printf("Loi cap phat vung nho.\n");
        exit(1);
    }
    safe_copy(visit->visit_id, sizeof(visit->visit_id), fields[0]);
    safe_copy(visit->visit_date, sizeof(visit->visit_date), fields[1]);
    safe_copy(visit->diagnosis, sizeof(visit->diagnosis), fields[2]);
    safe_copy(visit->prescription, sizeof(visit->prescription), fields[3]);
    safe_copy(visit->doctor_id, sizeof(visit->doctor_id), fields[4]);
    safe_copy(visit->department_id, sizeof(visit->department_id), fields[5]);
    dynamic_array_append(&p->visit_history, visit);
}


void load_data_from_file(struct MedicalSystemState* system, const char* filename) {
    FILE* f;
    char line[INPUT_SIZE];
    char fields[20][TEXT_LARGE];
    int i;
    int j;
    int count;
    int n;
    struct Department* dept;
    struct Doctor* doctor;
    struct Patient* patient;
    struct PatientInQueue* entry;
    long reg_time;
    int number;
    char normalized_patient_id[TEXT_SMALL];

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Khong tim thay tep %s. Chuong trinh bat dau voi du lieu rong.\n", filename);
        return;
    }

    while (fgets(line, sizeof(line), f) != NULL) {
        trim_newline(line);
        if (strlen(line) == 0) {
            continue;
        }
        count = split_fields(line, fields, 20);

        if (strcmp(fields[0], "BO_DEM") == 0 && count >= 4) {
            system->next_patient_id_counter = atoi(fields[1]);
            system->next_doctor_id_counter = atoi(fields[2]);
            system->next_checkin_number = atoi(fields[3]);
            if (system->next_patient_id_counter <= 0) system->next_patient_id_counter = 1;
            if (system->next_doctor_id_counter <= 0) system->next_doctor_id_counter = 1;
            if (system->next_checkin_number <= 0) system->next_checkin_number = 1;
        } else if (strcmp(fields[0], "KHOA") == 0 && count >= 2) {
            n = atoi(fields[1]);
            for (i = 0; i < n; i++) {
                if (fgets(line, sizeof(line), f) == NULL) break;
                trim_newline(line);
                split_fields(line, fields, 20);
                if (strlen(fields[0]) > 0 && find_department(system, fields[0]) == NULL && find_department_by_name(system, fields[1]) == NULL) {
                    dynamic_array_append(&system->dept_records_array, create_department(fields[0], fields[1]));
                }
            }
        } else if (strcmp(fields[0], "BAC_SI") == 0 && count >= 2) {
            n = atoi(fields[1]);
            for (i = 0; i < n; i++) {
                if (fgets(line, sizeof(line), f) == NULL) break;
                trim_newline(line);
                count = split_fields(line, fields, 20);
                if (count < 4 || strlen(fields[0]) == 0) {
                    continue;
                }
                doctor = (struct Doctor*)malloc(sizeof(struct Doctor));
                if (doctor == NULL) {
                    printf("Loi cap phat vung nho.\n");
                    exit(1);
                }
                safe_copy(doctor->doctor_id, sizeof(doctor->doctor_id), fields[0]);
                safe_copy(doctor->doctor_name, sizeof(doctor->doctor_name), fields[1]);
                safe_copy(doctor->specialization, sizeof(doctor->specialization), fields[2]);
                safe_copy(doctor->assigned_dept_id, sizeof(doctor->assigned_dept_id), fields[3]);
                dynamic_array_append(&system->doctor_records_array, doctor);
                number = id_number_after_prefix(doctor->doctor_id, 2);
                if (number >= system->next_doctor_id_counter) {
                    system->next_doctor_id_counter = number + 1;
                }
            }
        } else if (strcmp(fields[0], "BENH_NHAN") == 0 && count >= 2) {
            n = atoi(fields[1]);
            for (i = 0; i < n; i++) {
                if (fgets(line, sizeof(line), f) == NULL) break;
                trim_newline(line);
                count = split_fields(line, fields, 20);
                if (count < 11 || strlen(fields[0]) == 0) {
                    continue;
                }
                if (find_patient(system, fields[0]) != NULL || find_patient_by_citizen_id(system, fields[4]) != NULL) {
                    patient = NULL;
                } else {
                    patient = create_patient_from_file(system, fields);
                }

                if (fgets(line, sizeof(line), f) == NULL) break;
                trim_newline(line);
                split_fields(line, fields, 20);
                if (strcmp(fields[0], "DI_UNG") == 0) {
                    count = atoi(fields[1]);
                    for (j = 0; j < count; j++) {
                        if (fgets(line, sizeof(line), f) == NULL) break;
                        trim_newline(line);
                        if (patient != NULL) add_text_to_array(&patient->allergies, line);
                    }
                }

                if (fgets(line, sizeof(line), f) == NULL) break;
                trim_newline(line);
                split_fields(line, fields, 20);
                if (strcmp(fields[0], "BENH_NEN") == 0) {
                    count = atoi(fields[1]);
                    for (j = 0; j < count; j++) {
                        if (fgets(line, sizeof(line), f) == NULL) break;
                        trim_newline(line);
                        if (patient != NULL) add_text_to_array(&patient->comorbidities, line);
                    }
                }

                if (fgets(line, sizeof(line), f) == NULL) break;
                trim_newline(line);
                split_fields(line, fields, 20);
                if (strcmp(fields[0], "LUOT_KHAM") == 0) {
                    count = atoi(fields[1]);
                    for (j = 0; j < count; j++) {
                        if (fgets(line, sizeof(line), f) == NULL) break;
                        trim_newline(line);
                        split_fields(line, fields, 20);
                        if (patient != NULL) add_visit_from_file(patient, fields);
                    }
                }
            }
        } else if (strcmp(fields[0], "HANG_DOI") == 0 && count >= 2) {
            n = atoi(fields[1]);
            for (i = 0; i < n; i++) {
                if (fgets(line, sizeof(line), f) == NULL) break;
                trim_newline(line);
                count = split_fields(line, fields, 20);
                if (count < 9) {
                    continue;
                }
                dept = find_department(system, fields[0]);
                format_patient_id_short(fields[1], normalized_patient_id, sizeof(normalized_patient_id));
                patient = find_patient(system, normalized_patient_id);
                if (dept == NULL || patient == NULL || find_active_queue_entry(dept, patient->patient_id) != NULL) {
                    continue;
                }
                entry = (struct PatientInQueue*)malloc(sizeof(struct PatientInQueue));
                if (entry == NULL) {
                    printf("Loi cap phat vung nho.\n");
                    exit(1);
                }
                entry->root_record = patient;
                entry->medical_priority = atoi(fields[2]);
                entry->queue_priority_score = atoi(fields[3]);
                reg_time = atol(fields[4]);
                if (reg_time <= 0) {
                    reg_time = (long)time(NULL);
                }
                entry->registration_time = reg_time;
                entry->queue_status = atoi(fields[5]);
                safe_copy(entry->current_dept_id, sizeof(entry->current_dept_id), fields[6]);
                entry->checkin_number = atoi(fields[7]);
                entry->absent_count = atoi(fields[8]);
                max_heap_insert(&dept->wait_queue, entry);
                if (entry->checkin_number >= system->next_checkin_number) {
                    system->next_checkin_number = entry->checkin_number + 1;
                }
            }
        }
    }

    fclose(f);
    printf("Da nap du lieu tu tep %s.\n", filename);
}


void save_data_to_file(struct MedicalSystemState* system, const char* filename) {
    FILE* f;
    int i;
    int j;
    int total_queue;
    struct Department* dept;
    struct Doctor* doctor;
    struct Patient* patient;
    struct MedicalVisit* visit;
    struct PatientInQueue* entry;

    for (i = 0; i < system->dept_records_array.size; i++) {
        dept = (struct Department*)system->dept_records_array.elements[i];
        update_priority_for_long_waiters(dept, (long)time(NULL));
    }

    f = fopen(filename, "w");
    if (f == NULL) {
        printf("Khong the ghi tep %s.\n", filename);
        return;
    }

    fprintf(f, "BO_DEM|%d|%d|%d\n", system->next_patient_id_counter, system->next_doctor_id_counter, system->next_checkin_number);

    fprintf(f, "KHOA|%d\n", system->dept_records_array.size);
    for (i = 0; i < system->dept_records_array.size; i++) {
        dept = (struct Department*)system->dept_records_array.elements[i];
        fprintf(f, "%s|%s\n", dept->dept_id, dept->dept_name);
    }

    fprintf(f, "BAC_SI|%d\n", system->doctor_records_array.size);
    for (i = 0; i < system->doctor_records_array.size; i++) {
        doctor = (struct Doctor*)system->doctor_records_array.elements[i];
        fprintf(f, "%s|%s|%s|%s\n", doctor->doctor_id, doctor->doctor_name, doctor->specialization, doctor->assigned_dept_id);
    }

    fprintf(f, "BENH_NHAN|%d\n", system->patient_records_array.size);
    for (i = 0; i < system->patient_records_array.size; i++) {
        patient = (struct Patient*)system->patient_records_array.elements[i];
        fprintf(f, "%s|%s|%s|%d|%s|%s|%s|%s|%s|%d|%s\n",
                patient->patient_id,
                patient->full_name,
                patient->birth_date,
                patient->gender,
                patient->citizen_id,
                patient->phone_number,
                patient->email,
                patient->address,
                patient->blood_type,
                patient->is_priority_object,
                patient->priority_type);

        fprintf(f, "DI_UNG|%d\n", patient->allergies.size);
        for (j = 0; j < patient->allergies.size; j++) {
            fprintf(f, "%s\n", (char*)patient->allergies.elements[j]);
        }

        fprintf(f, "BENH_NEN|%d\n", patient->comorbidities.size);
        for (j = 0; j < patient->comorbidities.size; j++) {
            fprintf(f, "%s\n", (char*)patient->comorbidities.elements[j]);
        }

        fprintf(f, "LUOT_KHAM|%d\n", patient->visit_history.size);
        for (j = 0; j < patient->visit_history.size; j++) {
            visit = (struct MedicalVisit*)patient->visit_history.elements[j];
            fprintf(f, "%s|%s|%s|%s|%s|%s\n",
                    visit->visit_id,
                    visit->visit_date,
                    visit->diagnosis,
                    visit->prescription,
                    visit->doctor_id,
                    visit->department_id);
        }
    }

    total_queue = 0;
    for (i = 0; i < system->dept_records_array.size; i++) {
        dept = (struct Department*)system->dept_records_array.elements[i];
        total_queue += dept->wait_queue.size;
    }
    fprintf(f, "HANG_DOI|%d\n", total_queue);
    for (i = 0; i < system->dept_records_array.size; i++) {
        dept = (struct Department*)system->dept_records_array.elements[i];
        for (j = 0; j < dept->wait_queue.size; j++) {
            entry = dept->wait_queue.heap_array[j];
            fprintf(f, "%s|%s|%d|%d|%ld|%d|%s|%d|%d\n",
                    dept->dept_id,
                    entry->root_record->patient_id,
                    entry->medical_priority,
                    entry->queue_priority_score,
                    entry->registration_time,
                    entry->queue_status,
                    entry->current_dept_id,
                    entry->checkin_number,
                    entry->absent_count);
        }
    }

    fclose(f);
}


void save_all_data_files(struct MedicalSystemState* system, const char* input_file, const char* output_file) {
    if (input_file != NULL && input_file[0] != '\0') {
        save_data_to_file(system, input_file);
    }
    if (output_file != NULL && output_file[0] != '\0') {
        save_data_to_file(system, output_file);
    }
}


void clear_screen_simple(void) {
#ifdef _WIN32
    system("cls");
#else
    int i;
    for (i = 0; i < 25; i++) {
        printf("\n");
    }
#endif
}


void print_box_line(const char* text, int width) {
    int len;
    int pad;
    int i;
    len = (int)strlen(text);
    if (len > width) {
        len = width;
    }
    printf("| ");
    for (i = 0; i < len; i++) {
        putchar(text[i]);
    }
    pad = width - len;
    for (i = 0; i < pad; i++) {
        putchar(' ');
    }
    printf(" |\n");
}


void show_menu(void) {
    const int width = 70;
    clear_screen_simple();
    printf("+------------------------------------------------------------------------+\n");
    print_box_line("                         QUAN LY BENH VIEN", width);
    print_box_line("", width);
    print_box_line("  > 1. Them khoa", width);
    print_box_line("    2. Them bac si", width);
    print_box_line("    3. Them ho so benh nhan", width);
    print_box_line("    4. Dang ky kham va tu dong sap xep thu tu uu tien", width);
    print_box_line("    5. Goi benh nhan tiep theo", width);
    print_box_line("    6. Xem hang doi cua khoa", width);
    print_box_line("    7. Danh sach benh nhan", width);
    print_box_line("    8. Danh sach bac si", width);
    print_box_line("    9. Danh sach khoa", width);
    print_box_line("   10. Thong ke cuoi ngay", width);
    print_box_line("    0. Thoat", width);
    print_box_line("", width);
    printf("+------------------------------------------------------------------------+\n");
}
