/*
 * Shea Polansky
 * CS481 Spring 2017
 * Dorian Arnold
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
#include <sys/queue.h>
#include "libmyls.h"

#ifndef _DIRENT_HAVE_D_TYPE
#error "Need to be able to retrieve file type info from readdir() --- see notes of man readdir"
#endif

typedef struct directory {
    char * path;
    SLIST_ENTRY(directory) nodes;
} directory_t;

bool long_listing = false;
bool classify_listings = false;
bool follow_symlinks = false;
bool human_readable = false;
bool recursive = false;
long int disk_block_size = -1;

SLIST_HEAD(dirlist, directory) head;

static void process_file(const char * filepath, bool force_basename);
static void process_directory(const char * dirpath, bool printName);
static void recurse();

int main(int argc, char ** argv)
{
    int c;
    char * endptr = NULL;
    while(true)
    {
        static struct option long_options[] =
                {
                        {"classify", no_argument, NULL, 'c'},
                        {"disk-usage", required_argument, NULL, 'd'},
                        {"long-listing", no_argument, NULL, 'l'},
                        {"follow-symlinks", no_argument, NULL, 'f'},
                        {"human-readable", no_argument, NULL, 'h'},
                        {"recursive", no_argument, NULL, 'r'},
                        {0, 0, 0, 0}
                };

        int option_index = 0;
        c = getopt_long(argc, argv, "cd:lfhr", long_options, &option_index);
        if (c == -1) break; //all options processed
        switch (c)
        {
            case 'c':
                classify_listings = true;
                break;
            case 'd':
                disk_block_size = strtol(optarg, &endptr, 10);
                if (endptr != optarg + strlen(optarg) || disk_block_size <= 0)
                {
                    fprintf(stderr, "Error: enter a valid positive integer for disk block size.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                follow_symlinks = true;
                break;
            case 'h':
                human_readable = true;
                break;
            case 'r':
                recursive = true;
                break;
            case 'l':
                long_listing = true;
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                abort();
        }
    }

    SLIST_INIT(&head);
    if (optind < argc)
    {
        int files_count = argc - optind;
        bool prev_was_dir = false;
        while(optind < argc)
        {
            if (prev_was_dir) printf("\n");
            if (is_directory(argv[optind]))
            {
                process_directory(argv[optind], files_count > 1 || recursive);
                prev_was_dir = true;
            }
            else
            {
                prev_was_dir = false;
                process_file(argv[optind], false);
            }
            optind++;
            recurse();
        }
    }
    else //process current directory
    {
        process_directory("./", recursive);
        recurse();
    }

    return EXIT_SUCCESS;
}

void recurse()
{
    while(recursive && !SLIST_EMPTY(&head))
    {
        printf("\n");
        directory_t * dir = SLIST_FIRST(&head);
        SLIST_REMOVE_HEAD(&head, nodes);
        process_directory(dir->path, true);
        free(dir->path);
        free(dir);
    }
}

void process_file(const char * filepath, bool force_basename)
{
    finfo_t info;
    if (create_finfo(&info, filepath, force_basename))
    {
        fprintf(stderr, "Can't stat %s: %s\n", filepath, strerror(errno));
    }
    else print_finfo(&info);
    if (recursive && info.is_directory && !should_skip_recursive_directory(filepath))
    {
        directory_t * node = malloc(sizeof(directory_t));
        node->path = strdup(filepath);
        SLIST_INSERT_HEAD(&head, node, nodes);
    }
    free_finfo(&info);
}

void process_directory(const char * dirpath, bool printName)
{
    DIR * dirp;
    struct dirent *dp;
    if ((dirp = opendir(dirpath)) == NULL)
    {
        fprintf(stderr, "Can't open directory %s:%s\n", dirpath, strerror(errno));
        return;
    }
    if (printName) printf("%s:\n", dirpath);

    while((dp = readdir(dirp)) != NULL)
    {
        char * fullpath = path_cat(dirpath, dp->d_name);
        process_file(fullpath, true);
        free(fullpath);
    }
    closedir(dirp);
}
