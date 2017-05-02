//
// Created by Shea on 2017-04-27.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
#include "libmyls.h"

#ifndef _DIRENT_HAVE_D_TYPE
#error "Need to be able to retrieve file type info from readdir() --- see notes of man readdir"
#endif

bool long_listing = false;
bool classify_listings = false;
bool follow_symlinks = false;
bool human_readable = false;
bool recursive = false;
long int disk_block_size = -1;

//TODO: Recursion
//TODO: Recursion with symlinks

void process_file(const char * filepath);
void process_directory(const char * dirpath, bool printName);

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

    if (optind < argc)
    {
        while(optind < argc)
        {
            process_file(argv[optind]);
            optind++;
        }
    }
    else //process current directory
    {
        process_directory(".", false);
    }

    return EXIT_SUCCESS;
}

void process_file(const char * filepath)
{
    finfo_t info;
    if (create_finfo(&info, filepath))
    {
        fprintf(stderr, "Can't stat %s: %s\n", filepath, strerror(errno));
    }
    else print_finfo(&info);
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
        process_file(dp->d_name);
    }
    closedir(dirp);
}
