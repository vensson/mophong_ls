// ls_core.c
#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

/* ======================= READ DIR ======================= */

FileEntry* read_directory(const char *path, int *count, int show_hidden, const char *app_name) {
    DIR *dir = opendir(path);
    struct dirent *entry;
    FileEntry *files = NULL;
    *count = 0;

    if (!dir) {
        fprintf(stderr, "%s: cannot access '%s'\n", app_name, path);
        return NULL;
    }

    while ((entry = readdir(dir))) {
        if (!show_hidden && entry->d_name[0] == '.') continue;

        FileEntry *tmp = realloc(files, (*count + 1) * sizeof(FileEntry));
        if (!tmp) exit(1);
        files = tmp;

        FileEntry *f = &files[*count];
        (*count)++;

        snprintf(f->name, sizeof(f->name), "%s", entry->d_name);
        snprintf(f->path, sizeof(f->path), "%s/%s", path, entry->d_name);

        if (lstat(f->path, &f->stat_info) < 0) continue;
    }

    closedir(dir);
    return files;
}

/* ======================= INFO ======================= */

void get_permissions_string(FileEntry *e) {
    mode_t m = e->stat_info.st_mode;
    char *p = e->mode_string;

    p[0] = S_ISDIR(m) ? 'd' : S_ISLNK(m) ? 'l' : '-';
    p[1] = (m & S_IRUSR) ? 'r' : '-';
    p[2] = (m & S_IWUSR) ? 'w' : '-';
    p[3] = (m & S_IXUSR) ? 'x' : '-';
    p[4] = (m & S_IRGRP) ? 'r' : '-';
    p[5] = (m & S_IWGRP) ? 'w' : '-';
    p[6] = (m & S_IXGRP) ? 'x' : '-';
    p[7] = (m & S_IROTH) ? 'r' : '-';
    p[8] = (m & S_IWOTH) ? 'w' : '-';
    p[9] = (m & S_IXOTH) ? 'x' : '-';
    p[10] = 0;
}

void get_owner_and_group_names(FileEntry *e) {
    struct passwd *pw = getpwuid(e->stat_info.st_uid);
    struct group *gr = getgrgid(e->stat_info.st_gid);

    snprintf(e->owner_name, sizeof(e->owner_name),
             "%s", pw ? pw->pw_name : "unknown");
    snprintf(e->group_name, sizeof(e->group_name),
             "%s", gr ? gr->gr_name : "unknown");
}

void format_time_string(FileEntry *e) {
    struct tm *tm = localtime(&e->stat_info.st_mtime);
    strftime(e->time_string, sizeof(e->time_string),
             "%b %e %H:%M", tm);
}

void get_human_readable_size(FileEntry *e) {
    double size = e->stat_info.st_size;
    const char *u[] = {"B","K","M","G","T"};
    int i = 0;
    while (size >= 1024 && i < 4) {
        size /= 1024;
        i++;
    }
    snprintf(e->size_string, sizeof(e->size_string),
             "%.1f%s", size, u[i]);
}

/* ======================= SORT ======================= */

static int cmp_time(const void *a, const void *b) {
    const FileEntry *x = a, *y = b;
    return y->stat_info.st_mtime - x->stat_info.st_mtime;
}

static int cmp_size(const void *a, const void *b) {
    const FileEntry *x = a, *y = b;
    return y->stat_info.st_size - x->stat_info.st_size;
}

static int cmp_name(const void *a, const void *b) {
    return strcmp(((FileEntry*)a)->name, ((FileEntry*)b)->name);
}

void sort_files(FileEntry *files, int count, int sort_by_time, int sort_by_size) {
    if (sort_by_size)
        qsort(files, count, sizeof(FileEntry), cmp_size);
    else if (sort_by_time)
        qsort(files, count, sizeof(FileEntry), cmp_time);
    else
        qsort(files, count, sizeof(FileEntry), cmp_name);
}

/* ======================= PRINT ======================= */

void print_file_list(FileEntry *files, int count, int long_format, int human_readable) {
    if (!long_format) {
        for (int i = 0; i < count; i++)
            printf("%s  ", files[i].name);
        printf("\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        if (!human_readable)
            snprintf(files[i].size_string, sizeof(files[i].size_string),
                     "%lld", (long long)files[i].stat_info.st_size);

        printf("%s %2ld %-8s %-8s %6s %s %s\n",
               files[i].mode_string,
               (long)files[i].stat_info.st_nlink,
               files[i].owner_name,
               files[i].group_name,
               files[i].size_string,
               files[i].time_string,
               files[i].name);
    }
}

/* ======================= CORE ======================= */

void process_and_print(FileEntry *files, int count,
                       int long_format,
                       int sort_by_time,
                       int sort_by_size,
                       int human_readable) {
    for (int i = 0; i < count; i++) {
        get_permissions_string(&files[i]);
        get_owner_and_group_names(&files[i]);
        format_time_string(&files[i]);
        if (human_readable)
            get_human_readable_size(&files[i]);
    }

    sort_files(files, count, sort_by_time, sort_by_size);
    print_file_list(files, count, long_format, human_readable);
}

/* ======================= RECURSIVE ======================= */

void list_recursive(const char *path,
                    int show_hidden,
                    int long_format,
                    int sort_by_time,
                    int sort_by_size,
                    int human_readable,
                    const char *app_name) {
    int count;
    FileEntry *files = read_directory(path, &count, show_hidden, app_name);
    if (!files) return;

    printf("\n%s:\n", path);
    process_and_print(files, count,
                      long_format,
                      sort_by_time,
                      sort_by_size,
                      human_readable);

    for (int i = 0; i < count; i++) {
        if (S_ISDIR(files[i].stat_info.st_mode) &&
            strcmp(files[i].name, ".") != 0 &&
            strcmp(files[i].name, "..") != 0) {
            list_recursive(files[i].path,
                           show_hidden,
                           long_format,
                           sort_by_time,
                           sort_by_size,
                           human_readable,
                           app_name);
        }
    }

    free(files);
}
