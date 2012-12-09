#include "ServerDefinition.h"

void ServerDefinition::setName(char *n)
{
    char *login = NULL;
    char *pass = NULL;
    
    // server name is either <name> or <name>,<login-id>,<password>
    login = strchr(n,',');
    if (login != NULL)
    {
        *login = '\0';
        login++;  // skip ','
        pass = strchr(login,',');
        if (pass != NULL)
        {
            *pass = '\0';
             pass++;  // skip ','
        }
    }
    
    strncpy(name,(n?n:""),255); 
    strncpy(username,(login?login:""),32);
    strncpy(password,(pass?pass:""),32);

}
