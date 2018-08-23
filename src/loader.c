#define _DEBUG_MODE

#include <ncurses.h>
#include <menu.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include "window.h"
#include "settings.h"
#include "sanggu.h"


#define ARRAY_SIZE(A)	(sizeof(A)/sizeof(A[0]))
#define _WAIT	sleep(0.3)
#define SLECT_HEIGHT  7
#define SLECT_WIDTH   50
#define SLECT_POS_X   5
#define SLECT_POS_Y   6


static int check_for_existing_process(void);
static void shutdown_services(int);
int main(int argc, char* argv[])
{

//Checking for already running process
_WAIT;
int pid = check_for_existing_process();

//Loading Raspi - Loader ! 
int settings[]={
		9190,//port
		52, //data width
		52, //data height
		1, //google push alarm
		0	//nothing....
		};
	initscr();
	cbreak();
	noecho();
	read_settings(settings);
	start_color();
	keypad(stdscr, TRUE);
	
WINDOW* SLECT_W = create_window(SLECT_HEIGHT, SLECT_WIDTH, SLECT_POS_Y, SLECT_POS_X);

	init_pair(15, COLOR_WHITE, COLOR_BLUE);
	bkgd(COLOR_PAIR(15));
	keypad(SLECT_W, TRUE);
	nodelay(SLECT_W, 1);
	

	char *menu_list[] = {
			"Server - On [ Background Run ( Silent Mode ) ] ",
#ifdef	_DEBUG_MODE
			"Server - Debugging Mode - On",
#endif
			"kill Background Process",
                        "Default Settings",
			"About",
			"Advanced Option",
			"Exit"
		};
	int n_choices = ARRAY_SIZE(menu_list);
	char str[30];

	mvprintw(1 , 0, "Raspberry Pi - Camera Server Loader !");	
	mvprintw(2 , 0, "Select and Enter");
        
        if(pid != 0){
	mvprintw(3 , 0 , "Raspberry Pi camera server is already on !");
        sprintf(str,"Current runing process pid %d",pid);
	mvprintw(4, 0 ,str);
        }
      
	sprintf(str,"Default port: %d",settings[0]);
	mvprintw(SLECT_POS_Y +n_choices +1, 0 ,str);

	sprintf(str,"Data width: %d",settings[1]);
	mvprintw(SLECT_POS_Y + n_choices +2, 0 ,str);
	
	sprintf(str,"Data height: %d",settings[2]);
	mvprintw(SLECT_POS_Y+ n_choices +3, 0 ,str);

	sprintf(str,"Google Push: %d",settings[3]);
	mvprintw(SLECT_POS_Y + n_choices +4, 0 ,str);	

	ITEM **my_items =(ITEM **)calloc(n_choices + 1, sizeof(ITEM *));			
		
	for(int i = 0; i < n_choices; ++i)
	        my_items[i] = new_item(menu_list[i],NULL);
	my_items[n_choices] = (ITEM *)NULL;


	MENU* my_menu = new_menu((ITEM **)my_items);
	set_menu_win(my_menu,SLECT_W);
	set_menu_mark(my_menu,"+");
	post_menu(my_menu);


	refresh();
	wrefresh(SLECT_W);


	int c;
	char temp[8];	sprintf(temp,"%d",settings[0]);

	while(true)
	{   switch(c=wgetch(SLECT_W))
	    {	case KEY_DOWN:
		        	menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
		case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
		case 	 10:	//Enter
				mvprintw(n_choices + 3,0,item_name(current_item(my_menu)));
				_WAIT;
				free_item(my_items[0]);
				free_item(my_items[1]);
				free_menu(my_menu);
				endwin();
				if(!strcmp("Server - On [ Background Run ( Silent Mode ) ] ",item_name(current_item(my_menu))))
				{	//create a deamon by makeing a zombie process
				printf("The server is running in Background......\n");

				int a = fork();
				if(a ==0)
					execl("bin/core","bin/core",temp,((char *)NULL));

				printf("▶ server port: %s Loader successfully closed!!\n",temp);
				}

#ifdef	_DEBUG_MODE
				else if(!strcmp("Server - Debugging Mode - On",item_name(current_item(my_menu))))
					execl("bin/admin","bin/admin","9190",((char *)NULL)    );
#endif
                                else if(!strcmp("kill Background Process",item_name(current_item(my_menu))))
					shutdown_services(settings[0]);
                                else if(!strcmp("Default Settings",item_name(current_item(my_menu)))){
					default_settings();
					printf("Cleared Data, please start again !!\n");

				}
				else if(!strcmp("Advanced Option",item_name(current_item(my_menu))))
					execl("/bin/ls"," ",((char *)NULL));
				else if(!strcmp("About",item_name(current_item(my_menu))))
					execl("/bin/cat","cat",".credit",((char *)NULL));

				return 0;
				break;

		case 	 27: 	//ESC
		case 	'q':
				free_item(my_items[0]);
				free_item(my_items[1]);
				free_menu(my_menu);
				endwin();
				return 0;
				break;

	
		}
	}	

	
}



static int check_for_existing_process(void){
    FILE* fp = fopen("./Temp/pid","rt");
    int pid;
    
    if(fp ==NULL)
        return 0;
    else
    {
        if( fscanf(fp,"%d",&pid) != 1 )
            return 0;
        
        fclose(fp);
        return pid;
        
    }
   
}

static void shutdown_services(int port){
       
int  serv_fd, serv_port = port;
struct sockaddr_in serv_addr;

        serv_fd = socket(AF_INET, SOCK_STREAM, 0);	        
	memset(&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
        connect(serv_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        
        write(serv_fd,"stopservice",strlen("stopservice"));
        close(serv_fd);


}
