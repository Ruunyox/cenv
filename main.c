#include <stdlib.h>
#include <ncurses.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define CONNECTED    0x01
#define READING      0x02 
#define RECORDING    0x04

struct SENSOR {
	double temp_now;
	double rh_now;
	double time_now;
	unsigned int state;
};

int open_port(char* PORTNAME, tcflag_t OOPTS){
	int fd;
	fd = open(PORTNAME, OOPTS);
	if(fd == -1){
		perror("\nUnable to establish port connection.\n");
		exit(0);
	}
	else{
		fcntl(fd, F_SETFL, 0);
	}
	return fd;
}

void port_conf_8N1(int fd, int IBAUD, int OBAUD, struct termios options){
	 tcgetattr(fd,&options);
         cfsetispeed(&options, B9600);
         cfsetospeed(&options, B9600);
         options.c_cflag |= (CLOCAL | CREAD);
         options.c_cflag &= ~PARENB;   
         options.c_cflag &= ~CSTOPB;
         options.c_cflag &= ~CSIZE;
         options.c_cflag |= CS8;
	 options.c_lflag |= ICANON;
         tcsetattr(fd,TCSANOW, &options);   
}

int main(void){

	struct SENSOR snsr;
	snsr.temp_now = 0.00;
	snsr.rh_now = 0.00;
	snsr.time_now = 0.00;
	snsr.state &= ~(1 << 0); 
	char ans;
	double T_RH[2];

	char in[1];
	int bytesread;

	int chk;

	printf("\n\nEstablish Connection? (y/n) ");
	scanf("%s",&ans);
	if( ans == 'n'){exit(0);}

	struct termios options;
	char* PORTNAME = "/dev/ttyACM0";
	int fd = open_port(PORTNAME, O_RDWR | O_NOCTTY | O_NDELAY);
	
	sleep(5);
	if(fd != -1){
		port_conf_8N1(fd, B9600, B9600, options); 
		snsr.state |= CONNECTED;
	}
	printf("\nReseting Arduino...");
	
	snsr.state |= READING;
	write(fd, snsr.state, sizeof(snsr.state));

	char buffer[128] = "temp text";

	while(1){
		printf("\nPRESS 0 for MSG: ");
		scanf("%d",&chk);
		if(chk == 0){
			write(fd, "0", 1);
			bytesread = read(fd,buffer,128);	
			buffer[bytesread] = 0;
			printf("\n%s",buffer);
			sleep(1);
		}
	}
}	
