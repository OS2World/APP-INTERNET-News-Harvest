#ifndef __BINCAT_H__
#define __BINCAT_H__

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


#define PER_LINE 16
#define BOLD "\x1b[1m"
#define NORMAL "\x1b[0m"

#define ANSI_ON  1
#define ANSI_OFF 0


void setAnsi(int on_or_off);

void binCat(char *buffer);
void binCat(char *buffer,int num_chars);
void binCat(FILE *out,char *buffer);
void binCat(FILE *out,char *buffer,int num_chars);




#endif

