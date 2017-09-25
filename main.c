#include <stdlib.h>
#include <ncurses.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define CONNECTED    0x01
#define READING      0x02 
#define RECORDING    0x04

#define RED 	     "\x1b[31m"
#define PAD_X        12
#define MAX_RH	     100.00
#define MIN_RH	     0.00 

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

void curses_init(void){
	initscr();
	start_color();
	cbreak();
	noecho();
	curs_set(0);
	keyad(stdscr,TRUE);
}

WINDOW* rhbar init_rhbar(void){
	WINDOW* rhbar = newwin(3,COLS-PAD_X,5,5);
	wborder(rhbar,0,0,0,0,0,0,0,0);	
	return rhbar;		
}

void update_rhbar(WINODW* rhbar,double rh){
	double scale_factor = (LINES-PAD_X)/MAX_RH;
	int dRH = floor(rh*scale_factor); //rescaled and discretized RH
	wmove(rhbar,1,1);
	whline(rhbar,ACS_BLOCK,dRH);
	refresh();
	wrefresh(rhbar);	 
}
	
int main(void){

	struct SENSOR snsr;
	snsr.temp_now = 0.00;
	snsr.rh_now   = 0.00;
	snsr.time_now = 0.00;
	snsr.state &= ~(1 << 0); 
	char ans;
	double T_RH[2];

	char in[1];
	int bytesread;

	char buffer[128] = "temp text";
	int chk;

	printf( RED "\nEstablish Connection? (y/n) ");
	scanf("%s",&ans);
	if( ans == 'n'){exit(0);}

	struct termios options;
	char* PORTNAME = "/dev/ttyACM1";
	printf("\nEstablishing connection...");
	fflush(stdout);
	int fd = open_port(PORTNAME, O_RDWR | O_NOCTTY );
	usleep(3500000);
	printf("Done.");
	printf("\nConfiguring port settings...");
	port_conf_8N1(fd, B9600, B9600, options); 
	printf("Done.");
	snsr.state |= CONNECTED;
	printf("\nBeginning data collection...\n\n");
	fflush(stdout);
	usleep(3500000);

	curses_init();
	WINDOW* tbar = init_tbar();

	snsr.state |= READING;
	while(1){
		write(fd, "0", 1);
		bytesread = read(fd,buffer,128);	
		buffer[bytesread] = 0;
		printf("\r%s",buffer);
		fflush(stdout);
		sleep(2);
	}
}	
