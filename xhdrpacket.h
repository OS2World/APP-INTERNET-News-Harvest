/* xhdrpacket.h,v 1.2 2001/06/20 00:42:20 root Exp */

#ifndef __XHDRPACHET_H__
#define __XHDRPACHET_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#ifndef UNIX
#include <new.h>
#endif

#define MAX_DATA_SIZE 1024

class XhdrPacket
{
public:
    XhdrPacket();
    XhdrPacket(unsigned long art_num,void *in_data,size_t data_size) ;
    
    unsigned long getArticleNum();
    
    int   getDataAsInt() ;
    
    char *getDataAsChar();
    
    void  set(unsigned long art_num,void *in_data,size_t dsize);
    
    void setArticleNumber(unsigned long an);
    
    XhdrPacket& operator = (XhdrPacket &xp)
    {
        setArticleData(xp.data,xp.data_size);
        setArticleNumber(xp.article_number);
        
        return *this;
    }
    
    void setArticleData(void *in_data,size_t dsize);
    
private:
    unsigned long article_number;
    char data[MAX_DATA_SIZE];
    int  data_size;
    
};

#endif //#ifndef __XHDRPACHET_H__
