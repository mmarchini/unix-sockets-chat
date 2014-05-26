#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "client.h"
#include "connection.h"
#include "messages.h"
#include "util.h"
#include "constants.h"

#define MAX_RECIEVED_MESSAGES 256

int user_id;
int room_id;

char *recieved_buffer[MAX_RECIEVED_MESSAGES];

WINDOW *recieved_window = NULL;
WINDOW *typing_window = NULL;

void initncurses()
{
	initscr();
	start_color();
	keypad(stdscr, stdin);
	//noecho();
	//cbreak();
	curs_set(0);
	init_pair(1, COLOR_BLACK, COLOR_GREEN);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_color(COLOR_CYAN, 85, 85, 85);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);
}

WINDOW *destroy_win(WINDOW *local_win)
{
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners
	 * and so an ugly remnant of window.
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window
	 * 3. rs: character to be used for the right side of the window
	 * 4. ts: character to be used for the top side of the window
	 * 5. bs: character to be used for the bottom side of the window
	 * 6. tl: character to be used for the top left corner of the window
	 * 7. tr: character to be used for the top right corner of the window
	 * 8. bl: character to be used for the bottom left corner of the window
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);

	return NULL;
}

WINDOW *cria_janela(int altura, int largura, int y, int x, int boolean)
{
	WINDOW *janela_local;

	//Cria janela
	janela_local=newwin(altura, largura, y, x);

	//Colore a janela
	wbkgd(janela_local, COLOR_PAIR(2));

	//Adiciona borda a janela
	if(boolean)
		box(janela_local, 0, 0);

	return janela_local;
}

void refresh_typing() {
	int x, y;
	getmaxyx(stdscr, y, x);

	if(typing_window != NULL)
		typing_window = destroy_win(typing_window);
	typing_window = cria_janela(3, x, y-3, 0, 1);
	wmove(typing_window, 1, 1);

	wrefresh(typing_window);
}

void refresh_recieved(int y, int x) {

	if(recieved_window != NULL)
		recieved_window = destroy_win(recieved_window);
	recieved_window = cria_janela(y-3, x, 0, 0, 1);
	wmove(recieved_window, 1, 1);

	wrefresh(recieved_window);
}

void write_recieved(Message *message){
	char *buffer;
	char time[MESSAGE_SIZE];
	int i, j;
	int x,y;
	getmaxyx(stdscr, y, x);

	bzero(time, MESSAGE_SIZE);
	buffer = calloc(MESSAGE_SIZE, sizeof(char));
	time_to_string(time, MESSAGE_SIZE, message->timestamp);

	sprintf(buffer, "%s[%s]: %s", message->sender_name, time, message->message);

	for(i=0; i<MAX_RECIEVED_MESSAGES && recieved_buffer[i]!=NULL;i++);

	if(i>=MAX_RECIEVED_MESSAGES){
		free(recieved_buffer[0]);
		for(i=0; i<MAX_RECIEVED_MESSAGES-1;i++){
			recieved_buffer[i]=recieved_buffer[i+1];
		}
	}
	recieved_buffer[i] = buffer;

	j = i-(y-5);

	refresh_recieved(y, x);
	for(j = j<0 ? 0 : j;j<=i;j++){
		wmove(recieved_window, j+1, 1);
		wprintw(recieved_window, recieved_buffer[j]);
	}
	wrefresh(recieved_window);

}

void initialize_interface() {
	int x, y;
	initncurses();
	bzero(recieved_buffer, MAX_RECIEVED_MESSAGES);

	getmaxyx(stdscr, y, x);

	refresh_typing();
	refresh_recieved(y, x);

	wrefresh(recieved_window);			/* Print it on to the real screen */

}

void terminate_interface() {
	endwin();
}

void *handleSentMessages(void *args){
    Connection *connection = (Connection *)args;
    Message *message;
    char buffer[MESSAGE_SIZE];
    char time[256];
    bool running=True;
    if(connection == NULL){
    	printf("o.O");
    }

    while(running){
    	wgetstr(typing_window, buffer);
    	refresh_typing();

        message = MessageCreate(buffer, "", user_id, timestamp());

        if(writeTo(connection, message) != 0 || !connection->is_active)
        	running=False;
    }

    return NULL;
}

void *handleRecieveMessages(void *args){
    Connection *connection = (Connection *)args;
    Message *message;
    char time[256];
    bool running = True;

    while(running){
    	message = readFrom(connection);
    	if((message != NULL) && (connection->is_active)){
    		write_recieved(message);
			MessageDestroy(message);
    	} else {
    		running=False;
    	}
    }

    return NULL;
}

int startClient(char *ip, int port, char *user_name) {
    pthread_t sender, reciever;
    void *reciever_return, *sender_return;
    Connection *connection= connectToServer(ip, port);
    char zerobuffer[MESSAGE_SIZE];
    if(connection == NULL)
    	return 1;
    bzero(zerobuffer, MESSAGE_SIZE);
    Message *message = MessageCreate(zerobuffer, user_name, -1, timestamp());

    writeTo(connection, message);

    MessageDestroy(message);

    message = readFrom(connection);
    user_id = message->sender_id;

    initialize_interface();

    pthread_create(&reciever, NULL, &handleRecieveMessages, (void *) connection);
    pthread_create(&sender, NULL, &handleSentMessages, (void *) connection);

    pthread_join(reciever, &reciever_return);
    pthread_join(sender, &sender_return);
    
    terminate_interface();

    closeConnection(connection);

    return 0;
}
