//
// Created by Shea on 2017-04-27.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "libmyls.h"

static bool long_listing = false;
static bool classify_listings = false;
static bool follow_symlinks = false;
static bool human_readable = false;
static bool recursive = false;
static long int disk_block_size = -1; //<= 0 if not set

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

#define tf(val) ((val) ? "true" : "false")
    printf("%s %s %s %s %s\n", tf(classify_listings), tf(long_listing), tf(follow_symlinks), tf(human_readable), tf(recursive));
    printf("block size: %ld\n", disk_block_size);
    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    return EXIT_SUCCESS;
}
