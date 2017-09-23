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
         tcsetattr(fd,TCSANOW, &options);   
         options.c_cflag &= ~CSIZE;      
         options.c_cflag |= CS8;        
         options.c_cflag &= ~PARENB;   
         options.c_cflag &= ~CSTOPB;
         options.c_cflag &= ~CSIZE;
         options.c_cflag |= CS8;
}

int main(void){

	struct SENSOR snsr;
	snsr.temp_now = 0.00;
	snsr.rh_now = 0.00;
	snsr.time_now = 0.00;
	snsr.state &= ~(1 << 0); 
	char ans;

	printf("\n\nEstablish Connection? (y/n) ");
	scanf("%s",&ans);
	if( ans == 'n'){exit(0);}

	struct termios options;
	char* PORTNAME = "/dev/tty.usbserial-AI0550D2";
	int fd = open_port(PORTNAME, O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd != -1){
		port_conf_8N1(fd, B9600, B9600, options); 
		snsr.state |= CONNECTED;
	}
	printf("\n\nBegin Reading? (y/n) ");
	scanf("%s",&ans);
	if( ans == 'n'){exit(0);}
	sleep(1);

	for(int i=0; i<10; i++){
		sleep(1);		
	}
}	
