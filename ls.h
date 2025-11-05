// ls.h
#ifndef LS_H
#define LS_H

#include <sys/stat.h>
#include <sys/types.h> 
#include <time.h>      
#include <unistd.h>    
#include <errno.h>

// Cấu trúc FileEntry: Lưu trữ toàn bộ thông tin của một mục
typedef struct {
    char name[256];        
    struct stat stat_info; // Dữ liệu thô từ stat()
    
    // Dữ liệu đã xử lý
    char mode_string[11];  // -rwxr-xr-x (T2)
    char owner_name[32];   // Tên người dùng (T3)
    char group_name[32];   // Tên nhóm (T3)
    char time_string[15];  // Chuỗi thời gian đã định dạng (T4)
    
} FileEntry;

// Khai báo Nguyên mẫu Hàm (API)
FileEntry* read_directory(const char *path, int *count, int show_hidden, const char *app_name); // T1
void get_permissions_string(FileEntry *entry); // T2
void get_owner_and_group_names(FileEntry *entry); // T3
void format_time_string(FileEntry *entry); // T4 (Định dạng thời gian)
void sort_files(FileEntry *files, int count, int sort_by_time); // T4 (Sắp xếp)
void process_and_print(FileEntry *files, int count, int long_format, int sort_by_time); // T4 (Tích hợp)
void print_file_list(FileEntry *files, int count, int long_format); // T4 (In ấn)

#endif