#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define BUFLEN 512
#define NPACK 20
#define PORT 9930
#define SERVER_HOST "node1"
//#define SERVER_IP "10.1.1.3"

void fatal(char *s)
{
	perror(s);
	exit(1);
}

int main(void)
{
	struct sockaddr_in si_other;
	int s, i;
	char buf[BUFLEN];
	struct hostent *he;
	char ip[INET_ADDRSTRLEN];

	if ((he = gethostbyname(SERVER_HOST)) == NULL)
		fatal("Couldn't resolve server hostname");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	memcpy(&si_other.sin_addr, he->h_addr_list[0], sizeof(&si_other.sin_addr));

	inet_ntop(AF_INET, &si_other.sin_addr, ip, sizeof(ip));
	printf("server ip: %s\n", ip);

	//if (inet_aton(SERVER_IP, &si_other.sin_addr)==0) {
	//	fprintf(stderr, "inet_aton() failed\n");
	//	exit(1);
	//}

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		fatal("socket");

	for (i=0; i<NPACK; i++) {
		printf("Sending packet %d\n", i);
		sprintf(buf, "This is packet %d\n", i);
		if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, sizeof(si_other)) == -1)
			fatal("sendto()");
	}

	close(s);
	return 0;
}
