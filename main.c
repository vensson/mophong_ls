// main.c
#include "ls.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // 1. Khai báo và Xử lý các biến cờ (flags)
    int long_format = 0;   // -l
    int show_hidden = 0;   // -a
    int sort_by_time = 0;  // -t
    
    const char *target_path = "."; 

    // Lặp qua các đối số để tìm cờ và đường dẫn
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
                    default:
                        fprintf(stderr, "%s: Tùy chọn không hợp lệ -- '%c'\n", argv[0], argv[i][j]);
                        return 1;
                }
            }
        } else {
            // Đây là đường dẫn thư mục mục tiêu
            target_path = argv[i];
        }
    }

    // 2. Gọi hàm đọc thư mục (T1)
    int file_count = 0;
    FileEntry *files = read_directory(target_path, &file_count, show_hidden, argv[0]);

    if (files == NULL) {
        return 1; 
    }

    // 3. Gọi hàm xử lý, sắp xếp và in (Tích hợp T2, T3, T4)
    process_and_print(files, file_count, long_format, sort_by_time); 

    // 4. Dọn dẹp bộ nhớ
    if (files != NULL) {
        free(files);
    }
    
    return 0;
}