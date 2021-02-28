
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int status;
struct addrinfo hints;
struct addrinfo *servinfo;

int main(int argc, char *argv[])
{

    memset(&hints, 0, sizeof hints);    //make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        //don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    //TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        //fill in my IP for me


    if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }




    while(1)
        ;


    return 0;
}

