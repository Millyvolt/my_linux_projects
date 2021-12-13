
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <fcntl.h>
// #include <sys/stat.h>

char* itoa(int value, char* result, int base);


#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE	200


#define UART_PACKET_LEN      	16
#define THP_DATA_PACKET_LEN     12
#define HEADER_PACKET_LEN       4
#define UART_PACKET_START       ':'
#define PACKET_ID_POS      		1
#define LEN_POS          		2
#define DATA_POS        		3
#define HUM_POS	        		7
#define PRES_POS        		11
#define CRC_POS          		15
#define THP_DATA        		1


void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	// sleep(20);
	
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	char buf[MAXDATASIZE];
	int numbytes;

	char* data_esp32 = "Message from ESP32 ";

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo("192.168.1.45"/*NULL*/, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	printf("server: bound to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) { perror("accept"); continue; }

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener

			//data exchange
			while(1) {
				if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, MSG_DONTWAIT)) == -1) { perror("recv"); }
				else {
					//raw data output
					// char buf_tmp[10] = {0}, res_buf_tmp[100] = {0};
					// itoa(*(int32_t*)&buf[DATA_POS], buf_tmp, 10);
					// printf("t:%s \n",buf_tmp);
					// memset(buf_tmp, 0, sizeof(buf_tmp));

					// for(uint8_t i = 0; i < numbytes; i++) {
					// 	itoa(buf[i], buf_tmp, 10);
					// 	strcat(buf_tmp, " ");
					// 	strcat(res_buf_tmp, buf_tmp);
					// 	memset(buf_tmp, 0, sizeof(buf_tmp));
					// }
					// printf("received: '%s'\n",res_buf_tmp);

					char *pbuf = memchr(buf, UART_PACKET_START, numbytes), tmpbuf2[200], tmpbuf[200] = "<?php echo date(\"H:i:s\"); ?>", *path;
					if(numbytes - (pbuf - buf) >= UART_PACKET_LEN) {
						int32_t tmpr = *(int32_t*)&pbuf[DATA_POS], hym = *(int32_t*)&pbuf[HUM_POS];
						uint32_t pres = *(uint32_t*)&pbuf[PRES_POS];
						sprintf(tmpbuf2, "\ttemperature: %d.%02dC, humidity: %d.%02d%%, pressure: %d.%06dmm rt st\n", tmpr/100, tmpr%100,	\
							hym/1024, hym%1024/10, pres*3/102400, pres*3%102400);

						// strcat(tmpbuf, "<?php echo date(\"H:i:s\"); ?>");
						strcat(tmpbuf, tmpbuf2);

						path = "/var/www/html/esp32data.php";
						mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;	/* The permissions for the new file. */
						int fd = open (path, O_RDWR | O_CREAT | O_TRUNC, mode);		/* Create the file. */
						if (fd == -1) { perror ("open file"); /* return 1; */}
						
						//need to error cheking here and repeat writing
						write(fd, tmpbuf, strlen(tmpbuf));

						close(fd);
					}
				}

				// printf("temperature: %d, humidity: %d, pressure: %d",*(int32_t*)&buf[DATA_POS], \
				// 		*(int32_t*)&buf[HUM_POS], *(uint32_t*)&buf[PRES_POS]);

				if (send(new_fd, "check alive ESP32", 17, 0) == -1) { perror("send"); break;  /*break while and reaccept connection*/}

				// char buf_tmp[10] = {0}, res_buf_tmp[100] = {0};

				// for(uint8_t i = 0; i < numbytes; i++) {
				// 	itoa(buf[i], buf_tmp, 10);
				// 	strcat(buf_tmp, " ");
				// 	strcat(res_buf_tmp, buf_tmp);
				// 	memset(buf_tmp, 0, sizeof(buf_tmp));
				// }
				// printf("received: '%s'\n",res_buf_tmp);

				// itoa(*(int32_t*)&buf[DATA_POS], buf_tmp, 10);
				// printf("t:%s \n",buf_tmp);
				// memset(buf_tmp, 0, sizeof(buf_tmp));

				// buf[numbytes] = '\0';
				// // parcing received data, do some actions
				// if(!strcmp(buf, data_esp32)) {
				// 	char* path = "/var/www/html/esp32data.txt";	//"/home/pi/Documents/esp32data.txt";
				// 	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;	/* The permissions for the new file. */
				// 	int fd = open (path, O_RDWR | O_CREAT | O_TRUNC, mode);		/* Create the file. */
				// 	if (fd == -1) {		/* An error occurred. Print an error message and bail.*/
				// 		perror ("open file");
				// 		// return 1;
				// 	}

				// 	uint8_t cnt = 2;
				// 	while(cnt--) {
				// 		write(fd, buf, strlen(buf));
				// 		//strcat(buf, "hi");
				// 		// sleep(1);
				// 	}

				// 	close(fd);
				// }

				sleep(1);
			}
			close(new_fd);
			// exit(0);		// ??
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}


/**
   * C++ version 0.4 char* style "itoa":
   * Written by Lukás Chmela
   * Released under GPLv3.

   */
  char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
    }
    return result;
  }




