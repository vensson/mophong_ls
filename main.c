// main.c
#include "ls.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int long_format = 0;     // -l
    int show_hidden = 0;     // -a
    int sort_by_time = 0;    // -t
    int sort_by_size = 0;    // -S
    int human_readable = 0;  // -h
    int recursive = 0;       // -R

    const char *target_path = ".";

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j]; j++) {
                switch (argv[i][j]) {
                    case 'l': long_format = 1; break;
                    case 'a': show_hidden = 1; break;
                    case 't': sort_by_time = 1; break;
                    case 'S': sort_by_size = 1; break;
                    case 'h': human_readable = 1; break;
                    case 'R': recursive = 1; break;
                    default:
                        fprintf(stderr, "%s: invalid option -- '%c'\n",
                                argv[0], argv[i][j]);
                        return 1;
                }
            }
        } else {
            target_path = argv[i];
        }
    }

    if (recursive) {
        list_recursive(target_path,
                       show_hidden,
                       long_format,
                       sort_by_time,
                       sort_by_size,
                       human_readable,
                       argv[0]);
        return 0;
    }

    int count = 0;
    FileEntry *files = read_directory(target_path, &count, show_hidden, argv[0]);
    if (!files) return 1;

    process_and_print(files, count,
                      long_format,
                      sort_by_time,
                      sort_by_size,
                      human_readable);

    free(files);
    return 0;
}
