#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE B115200

static struct termios tcfg_orig;
static int fd;

void restore_tcfg(void) {
	if(fd>0)
		tcsetattr(fd, 0, &tcfg_orig);
	exit(0);
}

int main(int argc, char **argv) {
	if(argc<1) { printf("Usage: %s tty\n", argv[0]); }
	fd=open(argv[1],O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd<0) { perror("open"); exit(1); }
	signal(SIGINT, restore_tcfg);
	atexit(restore_tcfg);
	tcgetattr(fd, &tcfg_orig);
	struct termios tcfg;
	cfmakeraw(&tcfg);
	cfsetspeed(&tcfg,BAUDRATE);
	if(tcsetattr(fd, 0, &tcfg)<0) { perror("tcsetattr");  }
	char buf[512];
	int ntry=0;
	printf("Press D8 and reset the board... \n");
	while(ntry++<1000000) {
		write(fd,"\x55\x55",2);
		usleep(5000);
		if(read(fd,buf,2)>0) {
			printf("%s\n", buf);
			break;
		}
	}
	if(ntry==100000) {
		printf("Timeout\n");
		exit(1);
	}
	if(buf[0]!='O' || buf[1]!='K') 
		exit(1);
	usleep(15000);
	ntry=0;
	while(ntry++<100) {
		write(fd,"\x10\0\0\0",4);
		usleep(5000);
		if(read(fd,buf,24)>0) {
			printf("Bootinfo: \n");
			for(int i=0;i<24;i++) { printf("%02hhx ",buf[i]); if(i==15 || i==23) printf("\n"); }
			break;
		}
	}
}
