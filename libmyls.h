//
// Created by Shea on 2017-04-23.
//

#ifndef LAB_5_MYLS_H
#define LAB_5_MYLS_H
typedef int inode_t;
typedef struct finfo_t {
    char * name;
    inode_t inode;
    struct finfo_t * next;

} finfo_t;

char* readable_fs(double size/*in bytes*/, char *buf);

#endif //LAB_5_MYLS_H
