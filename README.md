# Hệ thống quản lý bệnh viện
Sử dụng hàng đợi ưu tiên để phân loại bệnh nhân theo tình trạng sức khỏe
# Hướng dẫn cài đặt & Chạy chương trình
1. Yêu cầu hệ thống
Trình biên dịch đã cài đặt GCC 
2. Cách thức chạy chương trình trên VsCode
- Mở terminal và chạy dòng lệnh sau: `gcc --version`
- Xác định đã cài đặt GCC, chạy tiếp lần lượt các dòng lệnh:
`. cd "C:\Users\Admin\Downloads\Hospital_management-main (2)\Hospital_management-main"` và
`dir *.c`
- Sau khi chạy `dir *.c` phải hiện ra các file mã nguồn .c (`dynamic_array.c` , `hospital.c` , `main.c` , `max_heap.c` , `ui.c`)
- Tiếp tục chạy dòng lệch sau: `gcc main.c hospital.c ui.c dynamic_array.c max_heap.c -o hospital.exe`
- Sau khi chạy nếu không báo lỗi sẽ tạo file: `hospital.exe`
- Sau đó chạy: `.\hospital.exe`
