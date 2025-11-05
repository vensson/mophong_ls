// ls_core.c

#include "ls.h" 
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> // Cần cho opendir/readdir
#include <errno.h>  // Cần cho xử lý lỗi
#include <string.h> // Cần cho strcpy/snprintf

// ----------------------------------------------------------------------
// CHỨC NĂNG T1: Đọc thư mục và thu thập dữ liệu thô
// ----------------------------------------------------------------------

FileEntry* read_directory(const char *path, int *count, int show_hidden, const char *app_name) {
    DIR *dir;
    struct dirent *entry;
    FileEntry *files = NULL;
    *count = 0;
    
    dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "%s: cannot access '%s': %s\n", app_name, path, strerror(errno));
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Lọc cho tùy chọn -a (show_hidden)
        if (!show_hidden && entry->d_name[0] == '.') {
            continue; // Bỏ qua tệp ẩn nếu -a không được đặt
        }

        // Tăng kích thước mảng (Cấp phát bộ nhớ động)
        *count = *count + 1;
        files = realloc(files, *count * sizeof(FileEntry));
        
        if (files == NULL) {
            perror("Lỗi cấp phát bộ nhớ");
            closedir(dir);
            return NULL;
        }

        // Sao chép tên và chuẩn bị gọi stat
        FileEntry *current_file = &files[*count - 1];
        strncpy(current_file->name, entry->d_name, sizeof(current_file->name) - 1);
        current_file->name[sizeof(current_file->name) - 1] = '\0';
        
        // Tạo đường dẫn đầy đủ để gọi stat (ví dụ: ./home/son/.bashrc)
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Sử dụng lstat để xử lý liên kết mềm và lấy dữ liệu thô
        if (lstat(full_path, &current_file->stat_info) < 0) {
            fprintf(stderr, "%s: cannot stat '%s': %s\n", app_name, full_path, strerror(errno));
            // Không thể làm gì nhiều nếu stat lỗi, chúng ta vẫn trả về file list.
        }
    }

    closedir(dir);
    return files;
}

// KHÔNG CÓ CÁC HÀM T2, T3, T4 Ở ĐÂY LÚC NÀY