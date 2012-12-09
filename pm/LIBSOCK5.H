#ifndef _LIBSOCK5_H_
#define _LIBSOCK5_H_

#ifndef __SOCKET_32H
#include <stdlib.h>
#include <sys/socket.h>
#endif

#ifdef BSD_SELECT
#ifndef __SELECT_32H
#include <sys/select.h>
#endif
#endif

#define accept            SOCKSaccept
#define bind              SOCKSbind
#define connect           SOCKSconnect
#define getpeername       SOCKSgetpeername
#define getsockname       SOCKSgetsockname
#define listen            SOCKSlisten
#define recv              SOCKSrecv
#define recvfrom          SOCKSrecvfrom
#ifndef BSD_SELECT
#define select            SOCKSselect
#else
#define bsdselect         SOCKSbsdselect
#endif
#define send              SOCKSsend
#define sendto            SOCKSsendto
#define soclose           SOCKSsoclose
#define shutdown          SOCKSshutdown

int _System SOCKSaccept( int, struct sockaddr *, int * );
int _System SOCKSbind( int, struct sockaddr *, int );
int _System SOCKSconnect( int, struct sockaddr *, int );
int _System SOCKSgetpeername( int, struct sockaddr *, int * );
int _System SOCKSgetsockname( int, struct sockaddr *, int * );
int _System SOCKSlisten( int, int);
int _System SOCKSrecv( int, char *, int, int );
int _System SOCKSrecvfrom(int, char *, int, int, struct sockaddr *, int * );
#ifndef BSD_SELECT
int _System SOCKSselect( int *, int, int, int, long );
#else
int _System SOCKSbsdselect(int,
                               struct fd_set *,
                               struct fd_set *,
                               struct fd_set *,
                               struct timeval *);
#endif
int _System SOCKSsend( int, char *, int, int );
int _System SOCKSsendto( int, char *, int, int, struct sockaddr *, int);
int _System SOCKSsoclose( int );
int _System SOCKSshutdown(int, int);

#endif /*_LIBSOCK5_H_ */
