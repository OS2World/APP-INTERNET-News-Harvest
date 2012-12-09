#ifndef __UNLINK_H__
#define __UNLINK_H__

#include <stdio.h>
#include <errno.h>

#ifdef UNIX
#include <unistd.h>
#endif

#define DELETE_OK   1
#define DELETE_FAIL 2

#define DELETE_FAIL_RO   3
#define DELETE_FAIL_OPEN 4
#define DELETE_FAIL_NONE 5

int deleteFile(char *path);

#endif //#ifndef __UNLINK_H__
