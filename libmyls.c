#include <stdio.h>
#include "libmyls.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>

char *get_mode_string(mode_t mode);

char *get_friendly_time(struct timespec timespec);

char *classify_file(struct stat info, const char *path);

char *get_size(struct stat stat);

char* readable_fs(double size/*in bytes*/) {
    int i = 0;
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    char * ret;
    char * temp;
    int tempsize = asprintf(&temp, "%.1f", size);
    if (temp[tempsize - 1] == '0') temp[tempsize - 2] = '\0';
    asprintf(&ret, "%s%s", temp, units[i]);
    free(temp);
    return ret;
}

void free_finfo(finfo_t * finfo)
{
#define conditional_free(x) if(x) free(x)
    conditional_free(finfo->name);
    conditional_free(finfo->group);
    conditional_free(finfo->mode);
    conditional_free(finfo->owner);
    conditional_free(finfo->time);
    conditional_free(finfo->classification);
    conditional_free(finfo->size);
#undef conditional_free
}

int create_finfo(finfo_t * finfo, const char * path)
{
    memset(finfo, 0, sizeof(typeof(*finfo)));
    struct stat stat_buf;
    if ((follow_symlinks ? stat : lstat)(path, &stat_buf)) return -1;
    if (!long_listing) finfo->name = strdup(path);
    else {
        char * temp = strdup(path);
        if (temp[strlen(temp)-1] == '/') temp[strlen(path)-1] = '\0';
        finfo->name = strdup(basename(temp));
        free(temp);
    }
    if (!long_listing && !classify_listings)
    {
        finfo->classification = strdup("");
        return 0;
    }
    finfo->inode = stat_buf.st_ino;
    finfo->mode = get_mode_string(stat_buf.st_mode);
    finfo->size = get_size(stat_buf);
    struct group *const group = getgrgid(stat_buf.st_gid);
    if (group == NULL) return -1;
    finfo->group = strdup(group->gr_name);
    struct passwd *const user = getpwuid(stat_buf.st_uid);
    if (user == NULL) return -1;
    finfo->owner = strdup(user->pw_name);
    finfo->time = get_friendly_time(stat_buf.st_mtim);
    if (classify_listings) finfo->classification = classify_file(stat_buf, path);
    else finfo->classification = strdup("");
    return 0;
}

char *get_size(struct stat stat)
{
    long size;
    if (disk_block_size <= 0) size = stat.st_size;
    else size = stat.st_blocks * disk_block_size;
    char * ret = NULL;
    if (human_readable) ret = readable_fs(size);
    else asprintf(&ret, "%ld", size);
    return ret;
}

char *classify_file(struct stat info, const char *path)
{
    if (info.st_mode & S_IFLNK) return strdup("@");
    if (info.st_mode & S_IFDIR) return strdup("/");
    if (!access(path, X_OK)) return "*";
    return strdup("");
}

char *get_friendly_time(struct timespec timespec)
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
        if (!(mode & MODE_BITS[i])) ret[i+1] = '-';
    }
    return ret;
}

void print_finfo(const finfo_t * info)
{
    if (!long_listing)
    {
        printf("%s%s\n", info->name, info->classification);
        return;
    }

    printf("%ld %s %s %s %s %s %s%s\n",
           info->inode, info->mode, info->owner, info->group,
           info->size, info->time, info->name, info->classification);
}