/* xhdrpacket.cpp,v 1.2 2001/06/20 00:42:20 root Exp */

#include "xhdrpacket.h"

XhdrPacket::XhdrPacket()
{ 
    data_size = 0;
    article_number = 0; 
    memset(data,0,sizeof(data));
}

XhdrPacket::XhdrPacket(unsigned long art_num,void *in_data,size_t data_size) 
{ 
    set(art_num,in_data,data_size); 
}

unsigned long XhdrPacket::getArticleNum()
{
    return article_number;
}

int   XhdrPacket::getDataAsInt() 
{ 
    if (data_size == 0) return 0; 
    else return (int) *((int *)data); 
}

char *XhdrPacket::getDataAsChar()
{ 
    if (data_size == 0) return ""; 
    return (char *)data; 
}

void XhdrPacket::set(unsigned long art_num,void *in_data,size_t dsize)
{
    data_size = dsize;
    setArticleNumber(art_num);
    setArticleData(in_data,dsize);
}

void XhdrPacket::setArticleNumber(unsigned long an)
{
    article_number = an;
}

void XhdrPacket::setArticleData(void *in_data,size_t dsize)
{
    data_size = dsize;

    if (data_size != 0 && in_data != NULL)
    {
        if (data_size > MAX_DATA_SIZE)
            data_size = MAX_DATA_SIZE;
        memcpy(data,in_data,data_size);
    }
    else
    {
        memset(data,0,sizeof(data));
    }
}



// qsort comparison Fn
int xhdrPacketSort(const void *One, const void *Two)
{

    unsigned long one = ((XhdrPacket *)One)->getArticleNum();
    unsigned long two = ((XhdrPacket *)Two)->getArticleNum();

    if (one < two) return -1;
    else if (two < one) return 1;
    else
        return 0;
}
