// structs.h
#ifndef STRUCTS_H
#define STRUCTS_H

#include <sys/stat.h> // Chứa struct stat và các macro kiểm tra quyền hạn
#include <sys/types.h> // Chứa các kiểu dữ liệu như uid_t, gid_t
#include <time.h>      // Chứa các hàm liên quan đến thời gian
#include <unistd.h>    // Chứa các hằng số liên quan đến liên kết

// Định nghĩa một hằng số cho kích thước tối đa của một dòng hiển thị
#define MAX_LINE_LENGTH 512

// Cấu trúc dùng để lưu trữ tất cả thông tin của một tệp/thư mục
// Đây là "hợp đồng" chung giữa tất cả các thành viên trong nhóm
typedef struct {
    char name[256];        // T1: Tên tệp/thư mục (từ readdir)

    // T1: Dữ liệu thô thu thập từ lời gọi stat()
    struct stat stat_info; 

    // T2: Dữ liệu đã xử lý từ T2
    char mode_string[11];  // Ví dụ: -rwxr-xr-x

    // T3: Dữ liệu đã xử lý từ T3 (Nhiệm vụ của bạn!)
    char owner_name[32];   // Tên chủ sở hữu (từ getpwuid)
    char group_name[32];   // Tên nhóm (từ getgrgid)
    
    // T4: Dữ liệu đã xử lý từ T4
    char time_string[15];  // Chuỗi thời gian đã định dạng (ví dụ: Jan 1 10:00)

    // Các trường khác (có thể dùng trong quá trình sắp xếp hoặc in)
    long long size_in_bytes; // Kích thước tệp (đã được chuyển đổi, nếu cần)
    int is_directory;        // 1 nếu là thư mục, 0 nếu không

} FileEntry;

// Khai báo nguyên mẫu hàm (API) cho T1 để T4/main.c có thể gọi
FileEntry* read_directory(const char *path, int *count, int show_hidden, const char *app_name);

// Khai báo nguyên mẫu hàm (API) cho T2
void get_permissions_string(FileEntry *entry);

// Khai báo nguyên mẫu hàm (API) cho T3 (Đây là nhiệm vụ của bạn)
void get_owner_and_group_names(FileEntry *entry);

// Khai báo nguyên mẫu hàm (API) cho T4
void sort_files(FileEntry *files, int count, int sort_by_time);
void format_time_and_size(FileEntry *entry);
void print_file_list(FileEntry *files, int count, int long_format);

#endif 