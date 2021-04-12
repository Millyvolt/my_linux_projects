
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>



#define NAME_LEN    100

char host_name[NAME_LEN];



int main(int argc, char *argv[])
{


    gethostname(host_name, NAME_LEN);
    printf("Hello, %s\n", host_name);




//    while(1)
//        ;

    return 0;
}

