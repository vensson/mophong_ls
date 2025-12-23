// ls.h
#ifndef LS_H
#define LS_H

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
    char name[256];
    char path[1024];          // <<< THÊM: full path cho đệ quy
    struct stat stat_info;

    char mode_string[11];
    char owner_name[32];
    char group_name[32];
    char time_string[15];
    char size_string[10];
} FileEntry;

/* API */
FileEntry* read_directory(const char *path, int *count, int show_hidden, const char *app_name);

void get_permissions_string(FileEntry *entry);
void get_owner_and_group_names(FileEntry *entry);
void format_time_string(FileEntry *entry);
void get_human_readable_size(FileEntry *entry);

/* SORT */
void sort_files(FileEntry *files, int count, int sort_by_time, int sort_by_size);

/* PRINT */
void print_file_list(FileEntry *files, int count, int long_format, int human_readable);

/* CORE */
void process_and_print(FileEntry *files, int count,
                       int long_format,
                       int sort_by_time,
                       int sort_by_size,
                       int human_readable);

/* -R */
void list_recursive(const char *path,
                    int show_hidden,
                    int long_format,
                    int sort_by_time,
                    int sort_by_size,
                    int human_readable,
                    const char *app_name);

#endif
