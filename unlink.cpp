#include "unlink.h"

int deleteFile(char *full_path)
{
    int rc = unlink(full_path);
    
    if (rc == 0)
        return DELETE_OK;
    else
    {
        rc = DELETE_FAIL;
    }
    
    return rc;
    
}














