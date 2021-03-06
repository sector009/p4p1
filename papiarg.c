#include "papiarg.h"

/*
 * Verrify each arguments and send to the correct part of program
 */
void check_arg(struct server_info * inf, char * argv[], int argc)
{
	if(argc >= 2){

		if(!strcmp(argv[1], "-usb")){

			if(argc >= 3){

				if(strlen(argv[2]) > 0 && strlen(argv[2]) < 50){
					printf("usb\n");
				} else {
					usage(argv[0]);
				}

			} else {

				usage(argv[0]);
				error("Not enought args", 1);

			}

		} else if(!strcmp(argv[1], "-c")){
			// check for connection and send the notification to the screen and to a file :)

			if(argc >= 3){
				printf("Email notification at %s", argv[2]);
			} else {
				acandsendnotif(inf);
			}

		} else if(!strcmp(argv[1], "-gtk")){

			// Gtk invironement with gui interface
			inf->argo.graphic++;
			serverThread.grtk++;
			printf("IN PROGRESS\n");
			usage(argv[0]);

		} else if(!strcmp(argv[1], "-v")){

			// Verbose terminal mode no ncurses
			// nor gtk
			inf->argo.cli++;
			serverThread.cmd++;
			printf("IN PROGRESS\n");
			usage(argv[1]);

		} else if(!strcmp(argv[1], "-cli")){

			inf->argo.cli++;
			serverThread.cmd++;
			main_loop(inf);

		} else {

			usage(argv[0]);

		}

	} else {

		/*
		 * Main function
		 **/
		inf->argo.ncr++;
		serverThread.ncurses++;
		initscr();
		keypad(stdscr, TRUE);
		main_loop(inf);
		refresh();
		getch();
		endwin();

	}

}

void acandsendnotif(struct server_info * inf)
{
	bnlisten(inf);
	int sock;
	char * sessionid = "5";
	char * leave = "&&";
	while (1){
		ansetupcon(inf, sock);
		write(*(serverThread.saved_sockets + sock), sessionid, strlen(sessionid));
		write(*(serverThread.saved_sockets + sock), leave, strlen(leave));
		printf("[!!!!!!]Accepted a client!!!!\n");
		sock++;
	}
}
