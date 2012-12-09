#include "bincat.h"

void doCat(FILE *out,char *,int);
void binCat(char *buffer);
void binCat(FILE *out,char *buffer);
void binCat(char *buffer,int num_chars);
void binCat(FILE *out,char *buffer,int num_chars);


int use_ansi = 0;

void setAnsi(int on_or_off)
{
    use_ansi = on_or_off;
}

void binCat(char *buffer)
{
    binCat(stdout,buffer,strlen(buffer));
}

void binCat(FILE *out,char *buffer)
{
    binCat(out,buffer,strlen(buffer));
}

void binCat(char *buffer,int num_chars)
{
    binCat(stdout,buffer,num_chars);
}

void binCat(FILE *out,char *buffer,int num_chars)
{
    int num_lines = num_chars / PER_LINE;
    int part_line = num_chars % PER_LINE;
    int i;
    char *ptr = buffer;
    
    for (i=0; i<num_lines; i++)
    {
        doCat(out,ptr,PER_LINE);
        ptr += PER_LINE;
    }
    
    if (part_line)
    {
        ptr = buffer + (PER_LINE * num_lines);
        doCat(out,ptr,part_line);
    }
}


// private
void doCat(FILE *out,char *buffer,int num_chars)
{
   int i;
   int count=0;
   int bold_on=0;

   for (i=0; i<PER_LINE; i++)
   {
       if (i < num_chars)
   fprintf(out,"%02x ",buffer[i]);
       else
   fprintf(out,"   ");

       count++;

       if (count % 4 == 0 && i != 0)
   fprintf(out," ");
   }

   fprintf(out,"  ");

   count=0;
   for (i=0; i<PER_LINE; i++)
   {
       if (i < num_chars)
    {
           if (isprint(buffer[i]))
           fprintf(out,"%c",buffer[i]);
           else
        fprintf(out,".");
       }
       else fprintf(out," ");
       
    count++;
 
    if (count == 4 && i != PER_LINE-1) {
       count = 0;
       if (bold_on){
          if (use_ansi && out==stdout) fprintf(out,NORMAL);
          bold_on = 0;
       }
          else {
          if (use_ansi && out==stdout) fprintf(out,BOLD);
          bold_on = 1;
          }
       }
   }                       
   if (use_ansi && out==stdout) fprintf(out,NORMAL);
   fprintf(out,"\n");
   
}

