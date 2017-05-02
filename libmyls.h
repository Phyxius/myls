//
// Created by Shea on 2017-04-23.
//

#ifndef LAB_5_MYLS_H
#define LAB_5_MYLS_H

#include <stdbool.h>
#include <sys/stat.h>

typedef int inode_t;
typedef struct finfo_t {
    char * name;
    ino_t inode;
    char * mode;
    char * owner;
    char * group;
    char * size;
    char * time;
    char * classification;
} finfo_t;

char* readable_fs(double size/*in bytes*/, char *buf);

int create_finfo(finfo_t * finfo, const char * path);
void free_finfo(finfo_t * finfo);
void print_finfo(const finfo_t * finfo);

static const mode_t MODE_BITS[] = {
        S_IRUSR,
        S_IWUSR,
        S_IXUSR,
        S_IRGRP,
        S_IWGRP,
        S_IXGRP,
        S_IROTH,
        S_IWOTH,
        S_IXOTH
};
static const int MODE_BITS_COUNT = sizeof(MODE_BITS) / sizeof(MODE_BITS[0]);

static const int TIME_STRING_SIZE = 3 + 1 + 2 + 1 + 2 + 1 + 2 + 1; //Mon dd hh:mm + a null
static char *const CLOSE_TIME_FORMAT = "%b %d %I:%M";
static char *const FAR_TIME_FORMAT = "%b %d %Y";

static const int SIZE_STRING_SIZE = 20;
extern bool long_listing;
extern bool classify_listings;
extern bool follow_symlinks;
extern bool human_readable;
extern bool recursive;
extern long int disk_block_size; //<= 0 if not set

#endif //LAB_5_MYLS_H
