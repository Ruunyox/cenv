#include <stdlib.h>
#include <ncurses.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#define CONNECTED    0x01
#define READING      0x02 
#define RECORDING    0x04

#define RED 	     "\x1b[31m"
#define PAD_X        12
#define MAX_RH	     100.00
#define MIN_RH	     0.00 
#define MAX_T	     50

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
	//start_color();
	cbreak();
	noecho();
	curs_set(0);
	nodelay(stdscr,TRUE);
	keypad(stdscr,TRUE);
}

WINDOW* init_rhbar(void){
	WINDOW* rhbar = newwin(3,COLS-PAD_X,5,5);
	wborder(rhbar,0,0,0,0,0,0,0,0);	
	return rhbar;		
}
/*
WINDOW* init_tbar(void){
	WINDOW* tbar = newwin(7,COLS-PAD_X,5,5);
	wborder(tbar,0,0,0,0,0,0,0,0);	
	return tbar;		
}
*/
void update_rhbar(WINDOW* rhbar,double rh){
	double scale_factor = 2*(LINES-PAD_X)/MAX_RH;
	char str[24];
	sprintf(str,"%lf",rh);
	int dRH = ceil(rh*scale_factor); //rescaled and discretized RH
	wclear(rhbar);
	wborder(rhbar,0,0,0,0,0,0,0,0);	
	wmove(rhbar,1,1);
	whline(rhbar,' '|A_REVERSE,dRH);
	mvaddstr(15,5,str);
	refresh();
	wrefresh(rhbar);	 
}
/*
void update_tbar(WINDOW* tbar,double t){
	double scale_factor = 2*(LINES-PAD_X)/MAX_T;
	char str[24];
	sprintf(str,"%lf",t);
	int dt = ceil(t*scale_factor); //rescaled and discretized RH
	wclear(tbar);
	wborder(tbar,0,0,0,0,0,0,0,0);	
	wmove(tbar,1,1);
	whline(tbar,' '|A_REVERSE,dt);
	mvaddstr(16,5,str);
	refresh();
	wrefresh(tbar);	 
}
*/	
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
	char* PORTNAME = "/dev/ttyACM0";
	printf("\nEstablishing connection.....");
	fflush(stdout);
	int fd = open_port(PORTNAME, O_RDWR | O_NOCTTY );
	usleep(2500000);
	printf("Done.");
	printf("\nConfiguring port settings...");
	port_conf_8N1(fd, B9600, B9600, options); 
	printf("Done.");
	snsr.state |= CONNECTED;
	printf("\nBeginning data collection...\n\n");
	fflush(stdout);
	usleep(1000000);

	curses_init();
	WINDOW* rhbar = init_rhbar();
//	WINDOW* tbar  = init_tbar();
	char rh_str[5];
	char ch;
	snsr.state |= READING;
	while(1){
		ch = getch();
		if(ch == 'q'){endwin();exit(0);}
		write(fd, "0", 1);
		bytesread = read(fd,buffer,128);	
		buffer[bytesread] = 0;
		strncpy(rh_str,buffer + 5,5);
		snsr.rh_now = atof(rh_str);
		update_rhbar(rhbar, snsr.rh_now);
//		update_tbar(rhbar, snsr.rh_now);
		//printf("\r%s",buffer);
		//printf("\r%lf",rh);
		//fflush(stdout);
		usleep(2000000);
	}
	close_port(PORTNAME);
}	
