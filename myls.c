//
// Created by Shea on 2017-04-27.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include<libgen.h>
#include "libmyls.h"

bool long_listing = false;
bool classify_listings = false;
bool follow_symlinks = false;
bool human_readable = false;
bool recursive = false;
long int disk_block_size = -1;

//TODO: Recursion
//TODO: handle directories
//TODO: Handle no-arg case

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
            finfo_t info;
            if (create_finfo(&info, argv[optind]))
            {
                fprintf(stderr, "Can't stat %s: %s\n", argv[optind], strerror(errno));
                exit(EXIT_FAILURE);
            }
            print_finfo(&info);
            free_finfo(&info);
            optind++;
        }
    }

    return EXIT_SUCCESS;
}
