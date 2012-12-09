/* obj_string.h,v 1.2 2001/06/20 00:42:20 root Exp */

#ifndef __OBJ_STRING_H__
#define __OBJ_STRING_H__

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

class String {
private:
 unsigned int _length;
 char *       _string;

public:
    
    String() { _length=0; _string=NULL; }
 String(char *);
 String(char);
 String(int);

 ~String(void);

 unsigned int is_empty(void);
 unsigned int length(void);
 void raise(void);
 void lower(void);
 int as_integer(void);
    int asInteger(void) { return as_integer(); }
    float as_float(void);
    float asFloat(void) { return as_float(); }
    char *as_charstar() { return _string; }
    char *asCharStar() { return _string; }
 void trim(void);
 void trim_left(void);
 void trim_right(void);
 void trim_right_quick(void);

 char *operator () (void);
 char  operator [] (int);

 String &operator = (const String&);
 String &operator = (char *);
 String &operator = (char);
 String &operator = (int);

 String &operator += (const String&);
 String &operator += (char*);
 String &operator += (char);
 String &operator += (int);

 friend String operator + (String &s1,String &s2);

        int operator == (const String &);
        int operator == (char *);
 int operator < (const String &);
 int operator < (char *);
 int operator <= (const String &);
 int operator <= (char *);
 
        int operator != (const String &);
        int operator != (char *);
 int operator > (const String &);
 int operator > (char *);
 int operator >= (const String &);
 int operator >= (char *);
};

#endif

