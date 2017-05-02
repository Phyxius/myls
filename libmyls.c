#include <stdio.h>
#include "libmyls.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>
#include <libgen.h>

char *get_mode_string(mode_t mode);

char *get_friendly_time(timestruc_t timespec);

char* readable_fs(double size/*in bytes*/, char *buf) {
    int i = 0;
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f%s", i, size, units[i]);
    return buf;
}

int create_finfo(finfo_t * finfo, const char * path, bool long_listing, bool follow_links)
{
    struct stat stat_buf;
    if ((follow_links ? stat : lstat)(path, &stat_buf)) return -1;
    if (!long_listing) finfo->name = strdup(path);
    else {
        char * temp = strdup(path);
        finfo->name = strdup(basename(temp));
        free(temp);
    }
    if (!long_listing) return 0;
    finfo->inode = stat_buf.st_ino;
    finfo->mode = get_mode_string(stat_buf.st_mode);
    finfo->size = stat_buf.st_size;
    struct group *const group = getgrgid(stat_buf.st_gid);
    if (group == NULL) return -1;
    finfo->group = strdup(group->gr_name);
    struct passwd *const user = getpwuid(stat_buf.st_uid);
    if (user == NULL) return -1;
    finfo->name = user->pw_name;
    finfo->time = get_friendly_time(stat_buf.st_mtim);
    return 0;
}

char *get_friendly_time(timestruc_t timespec)
{
    time_t raw_current_time;
    struct tm current_time;
    struct tm file_time;
    localtime_r(&timespec.tv_sec, &file_time);
    time(&raw_current_time);
    localtime_r(&raw_current_time, &current_time);
    char * ret = calloc(TIME_STRING_SIZE, sizeof(char));
    char * fmt = (current_time.tm_year == file_time.tm_year) ?
                 CLOSE_TIME_FORMAT :
                 FAR_TIME_FORMAT;
    strftime(ret, TIME_STRING_SIZE, fmt, &file_time);
    return ret;
}

char *get_mode_string(mode_t mode)
{
    char * ret = strdup("-rwxrwxrwx");
    switch(mode & S_IFMT)
    {
        case S_IFBLK:
            ret[0] = 'b';
            break;
        case S_IFCHR:
            ret[0] = 'c';
            break;
        case S_IFIFO:
            ret[0] = 'f';
            break;
        case S_IFLNK:
            ret[0] = 'l';
            break;
        case S_IFDIR:
            ret[0] = 'd';
            break;
        case S_IFREG:
        default:
            ret[0] = '-';
            break;
    }
    for (int i = 0; i < MODE_BITS_COUNT; i++)
    {
        if (mode & MODE_BITS[i]) ret[i+1] = '-';
    }
    return ret;
}
