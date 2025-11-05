// ls_core.c

#include "ls.h" 
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <errno.h>  
#include <string.h> 
#include <pwd.h>    
#include <grp.h>    
#include <time.h>

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
            continue; 
        }

        // Cấp phát bộ nhớ
        *count = *count + 1;
        files = realloc(files, *count * sizeof(FileEntry));
        if (files == NULL) {
            perror("Lỗi cấp phát bộ nhớ");
            closedir(dir);
            return NULL;
        }

        // Sao chép tên và gọi stat
        FileEntry *current_file = &files[*count - 1];
        strncpy(current_file->name, entry->d_name, sizeof(current_file->name) - 1);
        current_file->name[sizeof(current_file->name) - 1] = '\0';
        
        // Tạo đường dẫn đầy đủ
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Sử dụng lstat để xử lý liên kết mềm và lấy dữ liệu thô
        if (lstat(full_path, &current_file->stat_info) < 0) {
            fprintf(stderr, "%s: cannot stat '%s': %s\n", app_name, full_path, strerror(errno));
        }
    }

    closedir(dir);
    return files;
}

// ----------------------------------------------------------------------
// CHỨC NĂNG T2: Chuyển đổi Mode Bits sang Chuỗi Quyền hạn
// ----------------------------------------------------------------------

void get_permissions_string(FileEntry *entry) {
    mode_t mode = entry->stat_info.st_mode;
    char *perm_string = entry->mode_string; 

    // 1. Loại Tệp (Ký tự đầu tiên)
    if (S_ISREG(mode)) { perm_string[0] = '-'; } 
    else if (S_ISDIR(mode)) { perm_string[0] = 'd'; } 
    else if (S_ISLNK(mode)) { perm_string[0] = 'l'; } 
    else { perm_string[0] = '?'; }

    // 2. Quyền Người Dùng (Owner: rwx)
    perm_string[1] = (mode & S_IRUSR) ? 'r' : '-'; 
    perm_string[2] = (mode & S_IWUSR) ? 'w' : '-'; 
    perm_string[3] = (mode & S_IXUSR) ? 'x' : '-'; 
    
    // 3. Quyền Nhóm (Group: rwx)
    perm_string[4] = (mode & S_IRGRP) ? 'r' : '-';
    perm_string[5] = (mode & S_IWGRP) ? 'w' : '-';
    perm_string[6] = (mode & S_IXGRP) ? 'x' : '-';

    // 4. Quyền Khác (Others: rwx)
    perm_string[7] = (mode & S_IROTH) ? 'r' : '-';
    perm_string[8] = (mode & S_IWOTH) ? 'w' : '-';
    perm_string[9] = (mode & S_IXOTH) ? 'x' : '-';

    perm_string[10] = '\0'; 
}

// ----------------------------------------------------------------------
// CHỨC NĂNG T3: Lấy Tên Chủ sở hữu và Nhóm từ UID/GID
// ----------------------------------------------------------------------

void get_owner_and_group_names(FileEntry *entry) {
    uid_t uid = entry->stat_info.st_uid;
    struct passwd *pwd = getpwuid(uid);
    
    if (pwd != NULL) {
        strncpy(entry->owner_name, pwd->pw_name, sizeof(entry->owner_name) - 1);
        entry->owner_name[sizeof(entry->owner_name) - 1] = '\0';
    } else {
        snprintf(entry->owner_name, sizeof(entry->owner_name), "%d", (int)uid);
    }

    gid_t gid = entry->stat_info.st_gid;
    struct group *grp = getgrgid(gid);

    if (grp != NULL) {
        strncpy(entry->group_name, grp->gr_name, sizeof(entry->group_name) - 1);
        entry->group_name[sizeof(entry->group_name) - 1] = '\0';
    } else {
        snprintf(entry->group_name, sizeof(entry->group_name), "%d", (int)gid);
    }
}

// ----------------------------------------------------------------------
// CHỨC NĂNG T4: Định dạng Thời gian
// ----------------------------------------------------------------------

void format_time_string(FileEntry *entry) {
    struct tm *tm_info;
    const time_t *timer = &entry->stat_info.st_mtime;

    tm_info = localtime(timer);
    // Định dạng: "Mon 1 Jan 10:00"
    strftime(entry->time_string, sizeof(entry->time_string), "%b %e %H:%M", tm_info);
}

// ----------------------------------------------------------------------
// CHỨC NĂNG T4: So sánh cho Sắp xếp (-t)
// ----------------------------------------------------------------------

// Hàm so sánh cho qsort (sắp xếp giảm dần theo thời gian: mới nhất lên đầu)
int compare_time(const void *a, const void *b) {
    const FileEntry *fileA = (const FileEntry *)a;
    const FileEntry *fileB = (const FileEntry *)b;
    
    // Sắp xếp giảm dần (mới hơn có giá trị thời gian lớn hơn)
    if (fileA->stat_info.st_mtime > fileB->stat_info.st_mtime) return -1;
    if (fileA->stat_info.st_mtime < fileB->stat_info.st_mtime) return 1;
    
    return 0; // Thời gian bằng nhau
}

void sort_files(FileEntry *files, int count, int sort_by_time) {
    if (sort_by_time) {
        qsort(files, count, sizeof(FileEntry), compare_time);
    }
    // (Có thể thêm sắp xếp theo tên mặc định tại đây nếu muốn)
}

// ----------------------------------------------------------------------
// CHỨC NĂNG T4: In ấn
// ----------------------------------------------------------------------

void print_file_list(FileEntry *files, int count, int long_format) {
    if (!long_format) {
        // Định dạng ngắn (Tương đương ls)
        for (int i = 0; i < count; i++) {
            printf("%s  ", files[i].name);
        }
        printf("\n");
    } else {
        // Định dạng dài (-l)
        long total_blocks = 0;
        for (int i = 0; i < count; i++) {
            total_blocks += files[i].stat_info.st_blocks;
        }
        // Chia 2 vì Linux thường tính block 1KB, nhưng ls in 0.5KB
        printf("total %ld\n", total_blocks / 2); 
        
        for (int i = 0; i < count; i++) {
            printf("%s %2ld %s %s %6lld %s %s\n",
                files[i].mode_string,         // Quyền hạn (T2)
                (long)files[i].stat_info.st_nlink,
                files[i].owner_name,          // Chủ sở hữu (T3)
                files[i].group_name,          // Nhóm (T3)
                (long long)files[i].stat_info.st_size,
                files[i].time_string,         // Thời gian đã định dạng (T4)
                files[i].name
            );
        }
    }
}

// ----------------------------------------------------------------------
// CHỨC NĂNG T4: Tích hợp và Xử lý
// ----------------------------------------------------------------------

void process_and_print(FileEntry *files, int count, int long_format, int sort_by_time) {
    // 1. Xử lý Dữ liệu (T2, T3, T4 - Phải chạy trước khi in/sắp xếp)
    for (int i = 0; i < count; i++) {
        get_permissions_string(&files[i]); // T2
        get_owner_and_group_names(&files[i]); // T3
        format_time_string(&files[i]); // T4
    }

    // 2. Sắp xếp
    sort_files(files, count, sort_by_time);

    // 3. In ấn
    print_file_list(files, count, long_format);
}