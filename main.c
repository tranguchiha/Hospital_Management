#include "hospital.h"
#include "ui.h"

int main(int argc, char* argv[]) {
    struct MedicalSystemState system;
    int choice;
    char input_file[INPUT_SIZE];
    char output_file[INPUT_SIZE];

    system_state_init(&system);
    choose_input_file(input_file, sizeof(input_file), argc > 0 ? argv[0] : "");
    choose_output_file(output_file, sizeof(output_file), input_file);
    load_data_from_file(&system, input_file);
    save_all_data_files(&system, input_file, output_file);

    ui_show_welcome();
    ui_print_system_summary(&system);
    press_enter();

    while (1) {
        ui_show_main_menu();
        choice = read_int("  Chon chuc nang (nhap so): ");

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            ui_menu_add_department(&system);
        } else if (choice == 2) {
            ui_menu_add_doctor(&system);
        } else if (choice == 3) {
            ui_menu_add_patient(&system);
        } else if (choice == 4) {
            ui_menu_register_exam(&system);
        } else if (choice == 5) {
            ui_menu_call_next(&system);
        } else if (choice == 6) {
            ui_menu_show_queue(&system);
        } else if (choice == 7) {
            ui_menu_list_patients(&system);
        } else if (choice == 8) {
            ui_menu_list_doctors(&system);
        } else if (choice == 9) {
            ui_menu_list_departments(&system);
        } else if (choice == 10) {
            ui_menu_daily_statistics(&system);
        } else {
            ui_notify(MSG_WARNING, "Lua chon khong hop le. Vui long nhap so tu 0 den 10.");
        }

        if (choice != 0) {
            save_all_data_files(&system, input_file, output_file);
            press_enter();
        }
    }

    save_all_data_files(&system, input_file, output_file);
    system_state_free(&system);
    ui_show_goodbye();
    return 0;
}
