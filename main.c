// main.c
#include "ls.h" // Sử dụng file header chung
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // 1. Khai báo các biến cờ (flags)
    int long_format = 0;   // Cờ cho -l
    int show_hidden = 0;   // Cờ cho -a
    int sort_by_time = 0;  // Cờ cho -t
    int reverse_order = 0; // Cờ cho -r (sắp xếp ngược)
    
    const char *target_path = "."; // Mặc định là thư mục hiện tại (.)

    // 2. Lặp qua các đối số để tìm cờ và đường dẫn
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Đây là một tùy chọn (flag)
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'l':
                        long_format = 1;
                        break;
                    case 'a':
                        show_hidden = 1;
                        break;
                    case 't':
                        sort_by_time = 1;
                        break;
                    case 'r':
                        reverse_order = 1;
                        break;
                    default:
                        // Báo lỗi tùy chọn không hợp lệ
                        fprintf(stderr, "%s: Tùy chọn không hợp lệ -- '%c'\n", argv[0], argv[i][j]);
                        return 1;
                }
            }
        } else {
            // Đây là đường dẫn thư mục mục tiêu
            target_path = argv[i];
        }
    }

    // 3. Gọi hàm đọc thư mục (sẽ viết trong ls_core.c)
    int file_count = 0;
    FileEntry *files = read_directory(target_path, &file_count, show_hidden, argv[0]);

    if (files == NULL) {
        return 1; // Lỗi đã được xử lý trong read_directory
    }

    // 4. Gọi hàm xử lý, sắp xếp và in
    // Trước tiên, xử lý các dữ liệu phụ thuộc (T2, T3, T4 - thời gian/kích thước)
    // Sau đó mới sắp xếp và in.

    // 5. Dọn dẹp bộ nhớ
    if (files != NULL) {
        free(files);
    }
    
    return 0;
}