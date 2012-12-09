#include "obj_string.h"

#include <stdlib.h>
#include <string.h>
#include <new.h>

char *itoa (int value, char *string, int radix)
{
  char *dst;
  char digits[32];
  unsigned x;
  int i, n;

  dst = string;
  if (radix < 2 || radix > 36)
    {
      *dst = 0;
      return (string);
    }
  if (radix == 10 && value < 0)
    {
      *dst++ = '-';
      x = -value;
    }
  else
    x = value;
  i = 0;
  do
    {
      n = x % radix;
      digits[i++] = (n < 10 ? (char)n+'0' : (char)n-10+'a');
      x /= radix;
    } while (x != 0);
  while (i > 0)
    *dst++ = digits[--i];
  *dst = 0;
  return (string);
}

// ------------------- Constructors -------------------------------

String::String(char *s){

   if (s == NULL){
      _string = NULL;
      _length = 0;
   }
   else {
      _length = strlen(s);
      _string = new char[_length+1];
      strcpy(_string,s);
   }
}

String::String(char ch){

   _string = new char[2];
   *(_string)   = ch;
   *(_string+1) = '\0';
   _length = 1;
}

String::String(int x){
   char buffer[255];

   itoa(x,buffer,10);          // not portable...
   _length = strlen(buffer);
   _string = new char[_length+1];
   strcpy(_string,buffer);
}


// ---------------------------- Destructor -------------------------------

String::~String(void){

   if (_string) delete [] _string;
}


// --------------------- Miscellaneous Services -----------------------

unsigned int String::is_empty(void){

   return _length == 0;
}

unsigned int String::length(void){

   return _length;
}

void String::trim(void){

// poor way of implementing this.
// should do code specific to this service and save at
// least 1 allocate and 2 function calls.

   trim_left();
   trim_right();
}

void String::trim_right(void){
   char *ptr;
   int   old_length;

   if (_length != 0){
      old_length = _length;
      trim_right_quick();

      if (old_length > (int)_length){   // if space was saved
         ptr = _string;
         _string = new char[_length+1];
         strcpy(_string,ptr);
         delete [] ptr;
      }
   }
}

void String::trim_left(void){
   char *ptr,*old_string;
   int new_length=_length;

   if (_length != 0) {
      ptr = _string;
      while((new_length > 0)&&(isspace(*ptr))){
         ptr++;
         new_length--;
      }
      if (new_length < (int)_length){    // if space was saved
         old_string = _string;
         _length = new_length;
         _string = new char[_length+1];
         strcpy(_string,ptr);
         delete [] old_string;
      }
   }
}


void String::trim_right_quick(void){
   char *ptr;

   if (_length != 0){
      ptr = _string + _length-1;
      while ((_length != 0)&&(isspace(*ptr))){
         *ptr = '\0';
         ptr--;
         _length--;
      }
   }
}

void String::raise(void){
   char *ptr;

   if (_string){
      ptr = _string;
      while (*(ptr) != '\0'){
         *(ptr) = toupper(*(ptr));
         ptr++;
      }
   }
}

void String::lower(void){
   char *ptr;

   if (_string){
      ptr = _string;
      while (*(ptr) != '\0'){
         *(ptr) = tolower(*(ptr));
         ptr++;
      }
   }
}

int String::as_integer(void){
   if (_string){                   // to core-dump or not to core-dump
      return (int)atoi(_string);   // that is the question!
   }
   return 0;
}

float String::as_float(void){
   if (_string){
      return (float)atof(_string);
   }
   return (float)0;
}

// -------------------------- Operators ----------------------------------

char *String::operator () (void){

   if (_string) return _string;
   return "<empty>";
}

char String::operator [] (int index){

   return (char) *(_string+index);
}



String &String::operator = (const String &s){

   if (this != &s){
      if (_string) delete [] _string;
      _length = s._length;
      _string = new char[_length+1];
      strcpy(_string,s._string);
   }
   return *this;
}

String &String::operator = (char *s){

   if (_string) delete [] _string;
   if (s) {
      _length = strlen(s);
      _string = new char[_length+1];
      strcpy(_string,s);
   }
   else {
      _string = NULL;
      _length = 0;
   }
   return *this;
}


String &String::operator = (char ch){

   if (_string) delete [] _string;
   _string = new char[2];
   *(_string)   = ch;
   *(_string+1) = '\0';
   _length = 1;
   return *this;
}

String &String::operator = (int x){
   char buffer[255];

   if (_string) delete [] _string;
   itoa(x,buffer,10);          // not portable...
   _length = strlen(buffer);
   _string = new char[_length+1];
   strcpy(_string,buffer);
   return *this;
}



String &String::operator += (const String &s){
   char *ptr;

   if (this != &s){
      _length += s._length;
      ptr = new char[_length+1];
      if (_string) strcpy(ptr,_string);
      if (s._string) strcat(ptr,s._string);
      if (_string) delete [] _string;
      _string = ptr;
   }
   else {
      int i;

      ptr = new char[_length*2];
      if (_string) strcpy(ptr,_string);
      for (i=0; i<(int)_length; i++){
         *(ptr+i+_length) = *(ptr+i);
      }
      _length *= 2;
      *(ptr+_length) = '\0';
      if (_string) delete [] _string;
      _string = ptr;

   }
   return *this;
}

String &String::operator += (char *s){
   char *ptr;

   if (s){
      _length += strlen(s);
      ptr = new char[_length+1];
      if (_string) strcpy(ptr,_string);
      strcat(ptr,s);
      if (_string) delete [] _string;
      _string = ptr;
   }
   return *this;
}

String &String::operator += (char ch){
   char *ptr;

   _length += 1;
   ptr = new char[_length+1];
   if (_string) strcpy(ptr,_string);
   *(ptr+_length-1) = ch;
   *(ptr+_length) = '\0';
   if (_string) delete [] _string;
   _string = ptr;
   return *this;
}


String &String::operator += (int x){
   char buffer[255];

   itoa(x,buffer,10);          // not portable...
   operator += (buffer);
   return *this;
}

String operator + (String &s1,String &s2){

   String s(NULL);
   s = s1;
   s += s2;
   return s;
}

int String::operator == (const String &s){

   if (_length != s._length) return 0;                      // elvis fast comparison eh?!
   if ((_string == NULL) && (s._string == NULL)) return 1;
   if ((_string != NULL) && (s._string == NULL)) return 0;
   if ((_string == NULL) && (s._string != NULL)) return 0;
   return (strcmp(_string,s._string)==0);
}

int String::operator == (char *s){

   if ((_string == NULL) && (s == NULL)) return 1;
   if ((_string != NULL) && (s == NULL)) return 0;
   if ((_string == NULL) && (s != NULL)) return 0;
   return (strcmp(_string,s)==0);
}

int String::operator < (const String &s){

   if ((_string == NULL) && (s._string == NULL)) return 0;
   if ((_string != NULL) && (s._string == NULL)) return 1;
   if ((_string == NULL) && (s._string != NULL)) return 0;
   return (strcmp(_string,s._string) < 0);
}

int String::operator < (char *s){

   if ((_string == NULL) && (s == NULL)) return 0;
   if ((_string != NULL) && (s == NULL)) return 1;
   if ((_string == NULL) && (s != NULL)) return 0;
   return (strcmp(_string,s) < 0);
}

int String::operator <= (const String &s){

   if ((_string == NULL) && (s._string == NULL)) return 1;
   if ((_string != NULL) && (s._string == NULL)) return 1;
   if ((_string == NULL) && (s._string != NULL)) return 0;
   return (strcmp(_string,s._string) <= 0);
}

int String::operator <= (char *s){

   if ((_string == NULL) && (s == NULL)) return 1;
   if ((_string != NULL) && (s == NULL)) return 1;
   if ((_string == NULL) && (s != NULL)) return 0;
   return (strcmp(_string,s) <= 0);
}

int String::operator != (const String &s){

   return (!(operator == (s)));
}

int String::operator != (char *s){

   return (!(operator == (s)));
}

int String::operator > (const String &s){

   return (!(operator <= (s)));
}

int String::operator > (char *s){

   return (!(operator <= (s)));
}

int String::operator >= (const String &s){

   return (!(operator < (s)));
}

int String::operator >= (char *s){

   return (!(operator < (s)));
}


