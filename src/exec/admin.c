#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
//-----------------Ncurses Header---------------------------------
#include <ncurses.h>
#include <form.h>
#include <menu.h>
//---------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/select.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/msg.h>

// -----------------NETWORK HEADDER-------------------------------
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ipc.h>


pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
#define SHARED_KEY_VAL 9988
#define SHARED_DATA_NUM 9
//-----------------------------------------------------------------
#define MAX_ARR_SIZE 2000
#define MAX_ARR_SIZE_S  200
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
//----------------------색갈표------------------------------------
//RAGNE 0~15까지만!!!!
//9빨강 10 녹색 8 회색 7
#define BRIGHT_WHITE 15
#define BRIGHT_BLUE 12
#define BRIGHT_YELLOW 7
#define SKY_BLUE        14
//----------------------------------------------------------------
#define PAIR_BLACK_WHITE        1
#define PAIR_WHITE_BLACK        2
#define PAIR_RED_BLUE           3
#define PAIR_WHITE_BLUE         4
#define PAIR_RED_YELLOW         5



//----------------------------------------------------------------------------
//DATA_TYPE=1 NETWORK 2=RASPPI/CAM
typedef struct {
    long SHARED_DATA_TYPE;
    char DATA1[MAX_ARR_SIZE_S];
    char DATA2[MAX_ARR_SIZE_S];
    char DATA3[MAX_ARR_SIZE_S];
    char DATA4[MAX_ARR_SIZE_S];
    char DATA5[MAX_ARR_SIZE_S];
    char DATA6[MAX_ARR_SIZE_S];
    char DATA7[MAX_ARR_SIZE_S];
    char DATA8[MAX_ARR_SIZE_S];
    char DATA9[MAX_ARR_SIZE_S];
}SHARED_DATA;

//필요한 함수선언-------------------------------------------------------------
WINDOW *create_newwin(int TITLE_HEIGHT, int TITLE_WIDTH, int starty, int startx);
void destroy_win(WINDOW *local_win);
int System_Command(char* Command_in, char Data_out[]);
void Color_Setting(void);
void* picam_process(void*argv);
void Init_Program(void);
void*network_process(void* argv);
void* picampic_process(void*argv);

int linux_kbhit(void);
char* locate_shared_data(SHARED_DATA *shared_data, int i);
MENU* create_newslectwin(WINDOW* SLECT_W, char** choices, int SLECT_WIDTH, int SLECT_HEIGHT, int x, int y, char SLECT_DATA[]);

//-------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int saveimagesuffix = 0;
char saveimage[MAX_ARR_SIZE_S];
char status_ok[] = "status ok";
char status_not_ok[] = "status not ok";
int Detected = 0;
int Donotpic = 0;
int Key_IN;
FILE* DEBUG;
static struct termios initial_settings, new_settings;
static int peek_character = -1;
struct timespec ts;
int time_before = 0;
//-------------------------------------------------------------------------------
//void*pipe_process(void* a)
//{




//}



int main(int argc, char* argv[])
{



    int pid;
    int pipe_fd[2];


    if (pipe(pipe_fd) == -1)
    {
        printf("pipe_create failed\n");
        exit(1);
    }
    pid = fork();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (pid == 0)
    {
        dup2(pipe_fd[1], 2);
        dup2(pipe_fd[1], 1);
        if (execl("/usr/sbin/tcpdump", "/usr/sbin/tcpdump", "-t", "-nn", "port", argv[1], NULL) == -1)
        {
            printf("NO tcpdump found please INSTALL: TCPDUMP \n apt-get install tcpdump\n");
            //exit(1);
        }



    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else
    {


        int flag = fcntl(pipe_fd[0], F_GETFL, 0);
        fcntl(pipe_fd[0], F_SETFL, flag | O_NONBLOCK);
        key_t SHARED_KEY = msgget((key_t)SHARED_KEY_VAL, IPC_CREAT | 0666);
        if (SHARED_KEY == -1)
        {
            printf("failed to create SHARED_KEY\n");
            exit(1);
        }

        SHARED_DATA network_data;

        if (argc != 2)
        {
            printf("SERVER PORT: REQUIRED\n");
            exit(1);
        }

        //---------------------------------------------------------------------------------------------------------------------


        //----------------------------------------------------------------------------------------------------------------------
        memset(&network_data, 0x00, sizeof(network_data));
        pthread_t network_fd, picam_fd, picampic_fd;
        char* network_arg = argv[1];
        pthread_create(&network_fd, NULL, network_process, (void*)network_arg);
        pthread_create(&picam_fd, NULL, picam_process, (void*)network_arg);
        pthread_create(&picampic_fd, NULL, picampic_process, (void*)network_arg);


        //pipe_process
        //-----------------------------------------------------------------------------------------------------------------------
        Init_Program();
        //-------------------------------------------------------------------------------------------
        int BACKGROUND_HEIGHT = LINES;
        int BACKGROUND_WIDTH = COLS;
        int BACKGROUND_POS_X = 0;
        int BACKGROUND_POS_Y = 1;
        int TITLE_HEIGHT = 2;
        int TITLE_WIDTH = COLS;
        int TITLE_POS_X = 0;
        int TITLE_POS_Y = 1;
        int CLOCK_HEIGHT = 1;
        int CLOCK_WIDTH = (COLS * 1) / 4;
        int CLOCK_POS_X = 1;
        int CLOCK_POS_Y = (TITLE_POS_Y)+TITLE_HEIGHT + 1;
        int STATUSBAR_HEIGHT = 2;
        int STATUSBAR_WIDTH = (COLS * 3) / 4 - 3;
        int STATUSBAR_POS_X = CLOCK_POS_X + CLOCK_WIDTH + 1;
        int STATUSBAR_POS_Y = (TITLE_POS_Y)+TITLE_HEIGHT + 1;
        int AREA_1_HEIGHT = LINES / 4;
        int AREA_1_WIDTH = (COLS * 1) / 4;
        int AREA_1_POS_X = 1;
        int AREA_1_POS_Y = 7 * (LINES / 30);
        int AREA_2_HEIGHT = LINES / 4;
        int AREA_2_WIDTH = (COLS * 1) / 4;
        int AREA_2_POS_X = (COLS - AREA_2_WIDTH - 1);
        int AREA_2_POS_Y = 7 * (LINES / 30);
        int AREA_3_HEIGHT = LINES / 4;
        int AREA_3_WIDTH = (COLS - AREA_2_WIDTH - AREA_1_WIDTH) - 4;
        int AREA_3_POS_X = (COLS - AREA_3_WIDTH) / 2;
        int AREA_3_POS_Y = 7 * (LINES / 30);
        int AREA_4_HEIGHT = LINES / 5;
        int AREA_4_WIDTH = (COLS * 3) / 4 - 3;
        int AREA_4_POS_X = (COLS - AREA_3_WIDTH) / 2;
        int AREA_4_POS_Y = LINES - AREA_4_HEIGHT - 1;
        int SLECT_HEIGHT = LINES / 5;
        int SLECT_WIDTH = (COLS) / 4;
        int SLECT_POS_X = 1;
        int SLECT_POS_Y = LINES - SLECT_HEIGHT - 1;
        int AREA_5_HEIGHT = LINES - LINES / 4 - LINES / 5 - 10;
        int AREA_5_WIDTH = (COLS * 1) / 4;
        int AREA_5_POS_X = 1;
        int AREA_5_POS_Y = 7 * (LINES / 30) + AREA_1_HEIGHT + 1;
        int AREA_6_HEIGHT = LINES - LINES / 4 - LINES / 5 - 10;
        int AREA_6_WIDTH = (COLS * 1) / 4;
        int AREA_6_POS_X = (COLS - AREA_2_WIDTH - 1);
        int AREA_6_POS_Y = 7 * (LINES / 30) + AREA_1_HEIGHT + 1;
        int AREA_7_HEIGHT = LINES - LINES / 4 - LINES / 5 - 10;
        int AREA_7_WIDTH = (COLS - AREA_2_WIDTH - AREA_1_WIDTH) - 4;
        int AREA_7_POS_X = (COLS - AREA_3_WIDTH) / 2;
        int AREA_7_POS_Y = 7 * (LINES / 30) + AREA_1_HEIGHT + 1;
        //-----------------------------------------------------------------------------------------------
        char AREA_UPTOP_DATA[MAX_ARR_SIZE] = "Press Q to Quit";
        char AREA_CLOCK_DATA[MAX_ARR_SIZE];
        char AREA_STATUSBAR_DATA[MAX_ARR_SIZE];
        char AREA_TITLE_DATA[MAX_ARR_SIZE] = "->>>  Raspberry Pi Surveillance Camera ->>>>  Administrator Page";
        char AREA_1_DATA[MAX_ARR_SIZE] = "example";
        char AREA_2_DATA[MAX_ARR_SIZE];
        char AREA_3_DATA[MAX_ARR_SIZE];
        char AREA_4_DATA[MAX_ARR_SIZE];
        char AREA_5_DATA[MAX_ARR_SIZE];
        char AREA_6_DATA[MAX_ARR_SIZE];
        char AREA_7_DATA[MAX_ARR_SIZE];
        char SLECT_DATA[MAX_ARR_SIZE] = "SELECT OPTION";
        //--------------------------------Slection Menu----------선택메뉴만--------------------------------------
        char *MENU1[] = {
            "Sytem Status           ",
            "Choise 2               ",
            "Choice 3               ",
            "Choice 4               ",
            "Choise 5               ",
            "Choise 6               ",
            "Choise 7               "
        };

        //-----------------------------------------------------------------------------------------------------
        WINDOW* BACKGROUND;
        WINDOW* TITLE;
        WINDOW* AREA_1;
        WINDOW* AREA_2;
        WINDOW* AREA_3;
        WINDOW* AREA_4;
        WINDOW* AREA_5;
        WINDOW* AREA_6;
        WINDOW* AREA_7;
        WINDOW* SLECT_W;
        WINDOW* CLOCK;
        WINDOW* STATUSBAR;

        //-----------------------------------------------------------------------------------------------------
        BACKGROUND = create_newwin(BACKGROUND_HEIGHT, BACKGROUND_WIDTH, BACKGROUND_POS_Y, BACKGROUND_POS_X);
        TITLE = create_newwin(TITLE_HEIGHT, TITLE_WIDTH, TITLE_POS_Y, TITLE_POS_X);
        CLOCK = create_newwin(CLOCK_HEIGHT, CLOCK_WIDTH, CLOCK_POS_Y, CLOCK_POS_X);
        STATUSBAR = create_newwin(STATUSBAR_HEIGHT, STATUSBAR_WIDTH, STATUSBAR_POS_Y, STATUSBAR_POS_X);
        AREA_1 = create_newwin(AREA_1_HEIGHT, AREA_1_WIDTH, AREA_1_POS_Y, AREA_1_POS_X);
        AREA_2 = create_newwin(AREA_2_HEIGHT, AREA_2_WIDTH, AREA_2_POS_Y, AREA_2_POS_X);
        AREA_3 = create_newwin(AREA_3_HEIGHT, AREA_3_WIDTH, AREA_3_POS_Y, AREA_3_POS_X);
        AREA_4 = create_newwin(AREA_4_HEIGHT, AREA_4_WIDTH, AREA_4_POS_Y, AREA_4_POS_X);
        AREA_5 = create_newwin(AREA_5_HEIGHT, AREA_5_WIDTH, AREA_5_POS_Y, AREA_5_POS_X);
        AREA_6 = create_newwin(AREA_6_HEIGHT, AREA_6_WIDTH, AREA_6_POS_Y, AREA_6_POS_X);
        AREA_7 = create_newwin(AREA_7_HEIGHT, AREA_7_WIDTH, AREA_7_POS_Y, AREA_7_POS_X);
        SLECT_W = create_newwin(SLECT_HEIGHT, SLECT_WIDTH, SLECT_POS_Y, SLECT_POS_X);

        //-------------------------------------배경--------------색-----------------------------------------------
        bkgd(COLOR_PAIR(PAIR_RED_YELLOW));
        wbkgd(BACKGROUND, COLOR_PAIR(PAIR_WHITE_BLUE));  //배경색갈 지정 PAIR_(**)를 위에 #define 보면서 변경가능
        wbkgd(TITLE, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(CLOCK, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(STATUSBAR, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(AREA_1, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(AREA_2, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(AREA_3, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(AREA_4, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(AREA_5, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(AREA_6, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(AREA_7, COLOR_PAIR(PAIR_WHITE_BLACK));
        wbkgd(SLECT_W, COLOR_PAIR(PAIR_BLACK_WHITE));


        //--------------------------------------------------------------------------
        keypad(SLECT_W, TRUE);
        keypad(TITLE, TRUE);
        keypad(CLOCK, TRUE);
        keypad(STATUSBAR, TRUE);
        keypad(AREA_1, TRUE);
        keypad(AREA_2, TRUE);
        keypad(AREA_3, TRUE);
        keypad(AREA_4, TRUE);
        keypad(AREA_5, TRUE);
        keypad(AREA_6, TRUE);
        keypad(AREA_7, TRUE);

        //------------------------------------------------------------------------------------------------
        nodelay(SLECT_W, 1);
        nodelay(stdscr, 1);
        nodelay(AREA_1, 1);
        nodelay(AREA_2, 1);
        nodelay(AREA_3, 1);
        nodelay(AREA_4, 1);
        nodelay(AREA_5, 1);
        nodelay(AREA_6, 1);
        nodelay(AREA_7, 1);
        nodelay(BACKGROUND, 1);
        nodelay(STATUSBAR, 1);
        nodelay(TITLE, 1);
        nodelay(CLOCK, 1);

        //----------------------------------선택 메뉴 -영역4---------------------------------------------------------
        MENU* Menu = create_newslectwin(SLECT_W, MENU1, SLECT_WIDTH, SLECT_HEIGHT, 2, 0, SLECT_DATA);
        printw(AREA_UPTOP_DATA);
        //---------------------------------------------------------------------------------------------------




        //-----------------------------------------------process starts here--------------------------------------------------
        // while((Key_IN = getch()) != KEY_F(1))
        //while (Key_IN != 'q')

        while (1)
        {

            //timespec_get(&ts, TIME_UTC);
            //usleep(1000); //100000us

            //if(time_before != (int)ts.tv_sec)
            {//     time_before=(int)ts.tv_sec;
             //     fputc('t', DEBUG);
             //---------------------------------------------------------------------------------------------------------------


             //System_Command("ps -ef", AREA_2_DATA);
             //System_Command("date -R",AREA_1_DATA);


             //---------------------delete screen--------------------------------------------------------------------

                werase(TITLE);
                werase(CLOCK);
                werase(STATUSBAR);
                werase(AREA_1);
                werase(AREA_2);
                werase(AREA_3);
                werase(AREA_4);
                werase(AREA_5);
                werase(AREA_6);
                werase(AREA_7);

                //--------------------print data-----------------------------------------------------------------------------------------

                mvwprintw(TITLE, 1, (TITLE_WIDTH - strlen(AREA_TITLE_DATA)) / 2, "%s", AREA_TITLE_DATA);
                mvwprintw(AREA_1, 0, 0, AREA_1_DATA);

                //-------------------------------------------------------------------------------------------------------------------

                sprintf(AREA_STATUSBAR_DATA, "Network status: %d, CAM STATUS: %d, Data_Trasmission: %d Detection System %d", 1, 1, 1, Detected);
                mvwprintw(STATUSBAR, 0, 0, AREA_STATUSBAR_DATA);
                //------------------------------------------------------------------------------------------------
                System_Command("date -R", AREA_CLOCK_DATA);
                mvwprintw(CLOCK, 0, 0, AREA_CLOCK_DATA);
                //-------------------------------------------------------------------------------------------------
                //mvwprintw(AREA_2, 0, 0, AREA_2_DATA);
                //memset(&network_data,0,sizeof(network_data));

                msgrcv(SHARED_KEY, &network_data, sizeof(network_data) - sizeof(long), 1, IPC_NOWAIT);
                mvwprintw(AREA_2, 0, 0, network_data.DATA1);
                mvwprintw(AREA_2, 1, 0, network_data.DATA2);
                mvwprintw(AREA_2, 2, 0, network_data.DATA3);
                mvwprintw(AREA_2, 3, 0, network_data.DATA4);
                mvwprintw(AREA_2, 4, 0, network_data.DATA5);
                mvwprintw(AREA_2, 5, 0, network_data.DATA6);
                mvwprintw(AREA_2, 6, 0, network_data.DATA7);
                mvwprintw(AREA_2, 7, 0, network_data.DATA8);
                mvwprintw(AREA_2, 8, 0, network_data.DATA9);
                //--------------------------------------------------------------------------------------------------
                System_Command("netstat -an", AREA_3_DATA);
                mvwprintw(AREA_3, 0, 0, AREA_3_DATA);
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //System_Command("df -h",AREA_4_DATA);
                read(pipe_fd[0], AREA_4_DATA, sizeof(AREA_4_DATA));
                mvwprintw(AREA_4, 0, 0, AREA_4_DATA);



                //---------------screen update----------------------------------------------------------------------------------
                refresh();
                wrefresh(BACKGROUND);
                wrefresh(TITLE);
                wrefresh(CLOCK);
                wrefresh(STATUSBAR);
                wrefresh(AREA_1);
                wrefresh(AREA_2);
                wrefresh(AREA_3);
                wrefresh(AREA_4);
                wrefresh(AREA_5);
                wrefresh(AREA_6);
                wrefresh(AREA_7);
                wrefresh(SLECT_W);

            }

            //--------------------input ---------------------------------------------------------------------------

            //if( kbhit() )
            //{
            Key_IN = wgetch(SLECT_W);
            //Key_IN = getch();
            //fputc(KEY_DOWN, DEBUG);
            //fputc(',',DEBUG);
            //fputc(Key_IN, DEBUG);

            switch (Key_IN)
            {
            case KEY_DOWN:
                menu_driver(Menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(Menu, REQ_UP_ITEM);
                break;
            case 'q':
                endwin();
                fclose(DEBUG);
                system("ps -ef|grep tcpdump|grep -v grep |awk '{print $2}'|xargs kill -9");
                return 0;
                break;
            }
            //}





        }
    }
}


WINDOW *create_newwin(int TITLE_HEIGHT, int TITLE_WIDTH, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(TITLE_HEIGHT, TITLE_WIDTH, starty, startx);
    //      box(local_win, 0 , 0);
    wrefresh(local_win);

    return local_win;
}


void destroy_win(WINDOW *local_win)
{
    /* box(local_win, ' ', ' '); : This won't produce the desired
    * result of erasing the window. It will leave it's four corners
    * and so an ugly remnant of window.
    */
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
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
}


int System_Command(char* Command_in, char Data_out[])
{
    FILE *fp;
    char path[MAX_ARR_SIZE];
    memset(path, 0, sizeof(path));
    int c = -1;
    int i = 0;


    /* Open the command for reading. */
    fp = popen(Command_in, "r");
    if (fp == NULL)
    {
        return -1;
    }

    while (c != 0)
    {
        c = fgetc(fp);
        if (c == EOF || i >= (MAX_ARR_SIZE - 1))
        {
            c = 0;
        }
        path[i++] = c;
    }


    strcpy(Data_out, path);
    /* close */
    pclose(fp);
    return 0;

}



void Color_Setting(void)
{



    //-----------------흰색 글자 검정 파탕---------------------------------------------

    if (can_change_color() && COLORS >= 16)
    {
        init_color(BRIGHT_WHITE, 1000, 1000, 1000);
    }
    if (COLORS >= 16)
    {
        init_pair(PAIR_BLACK_WHITE, COLOR_BLACK, BRIGHT_WHITE);
    }
    else
    {
        init_pair(PAIR_BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
    }
    //-----------------글자흰색  배경파랑색  생성-------------------------------------------
    if (can_change_color() && COLORS >= 16)
    {
        init_color(BRIGHT_BLUE, 100, 100, 1000);
    }
    if (COLORS >= 16)
    {
        init_pair(PAIR_WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);
    }
    else
    {
        init_pair(PAIR_WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);
    }
    //-------------------하늘색 생성------------------------------------------


    if (can_change_color() && COLORS >= 16)
    {
        init_color(SKY_BLUE, 100, 100, 1000);
    }
    if (COLORS >= 16)
    {
        init_pair(PAIR_RED_BLUE, COLOR_RED, SKY_BLUE);
    }
    else
    {
        init_pair(PAIR_RED_BLUE, COLOR_RED, COLOR_BLUE);
    }

    //--------------------배경검은색글자흰색--------------------------------------------------


    if (can_change_color() && COLORS >= 16)
    {
        init_color(SKY_BLUE, 100, 100, 1000);
    }
    if (COLORS >= 16)
    {
        init_pair(PAIR_WHITE_BLACK, BRIGHT_WHITE, COLOR_BLACK);
    }
    else
    {
        init_pair(PAIR_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
    }

    //--------------------노랑빨강--------------------------------------------------


    if (can_change_color() && COLORS >= 16)
    {
        init_color(BRIGHT_YELLOW, 1000, 1000, 1000);
    }
    if (COLORS >= 16)
    {
        init_pair(PAIR_RED_YELLOW, COLOR_RED, BRIGHT_YELLOW);
    }
    else
    {
        init_pair(PAIR_RED_YELLOW, COLOR_RED, COLOR_YELLOW);
    }


}


void Init_Program(void)
{
    // INTITIALIZATION
    initscr(); //이걸 활성화시켜야지 그래픽 모드 작동댐
    raw();  //사용자가 엔터키를 안눌러도 글자 전달댐
    keypad(stdscr, TRUE); //특수키를 입력 받을지의 여부, ex F1,F2~F12같은 거
    noecho(); //사용자가 입력해도 화면에, 지저분하게 뜨지 않음
              //cbreak();
    refresh(); //반드시 해야함 LINES, COLS을 업데이트 함
    start_color();// 색갈을 사용함
    Color_Setting();
    //halfdelay(1);
    //cbreak();
    //nobreak();
    memset(&ts, 0, sizeof(ts));
    DEBUG = fopen("Debug_Result.txt", "w");
}

MENU* create_newslectwin(WINDOW* SLECT_W, char** choices, int SLECT_WIDTH, int SLECT_HEIGHT, int y, int x, char SLECT_DATA[])
{

    ITEM **TABLE;
    MENU *Menu;
    MENU *Menu_o;
    int n_choices, i;
    ITEM *Current_Slected;
    n_choices = 6;
    //n_choices = sizeof(*choices) / sizeof(*choices[0]);
    TABLE = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (i = 0; i < n_choices; ++i)
        TABLE[i] = new_item(choices[i], choices[i]);
    TABLE[n_choices] = (ITEM *)NULL;
    Menu = new_menu((ITEM **)TABLE);
    set_menu_win(Menu, SLECT_W);
    set_menu_sub(Menu, derwin(SLECT_W, n_choices, SLECT_WIDTH, y, x)); //(상자크기(y,x 위치 y,x)
    set_menu_mark(Menu, ">");
    //wattron(SLECT_W, COLOR_PAIR(PAIR_RED_BLUE));
    //box(SLECT_W, 0, 0);
    mvwprintw(SLECT_W, 1, (SLECT_WIDTH - strlen(SLECT_DATA)) / 2, SLECT_DATA);
    //mvwaddch(SLECT_W, 2, 0, ACS_LTEE);
    //mvwhline(SLECT_W, 2, 1, ACS_HLINE, 38);
    //mvwaddch(SLECT_W, 2, 39, ACS_RTEE);
    //refresh();
    //wattroff(SLECT_W, COLOR_PAIR(PAIR_RED_BLUE));
    post_menu(Menu);
    //refresh();
    //wrefresh(SLECT_W);
    return Menu;

}


char* locate_shared_data(SHARED_DATA *shared_data, int i)
{
    if (i == 1)
    {
        return (char*)((*shared_data).DATA1);
    }
    else if (i == 2)
    {
        return (char*)((*shared_data).DATA2);
    }
    else if (i == 3)
    {
        return (char*)((*shared_data).DATA3);
    }
    else if (i == 4)
    {
        return (char*)((*shared_data).DATA4);
    }
    else if (i == 5)
    {
        return (char*)((*shared_data).DATA5);
    }
    else if (i == 6)
    {
        return (char*)((*shared_data).DATA6);
    }
    else if (i == 7)
    {
        return (char*)((*shared_data).DATA7);
    }
    else if (i == 8)
    {
        return (char*)((*shared_data).DATA8);
    }
    else if (i == 9)
    {
        return (char*)((*shared_data).DATA9);
    }

}




void* picampic_process(void*argv)
{
    while (1)
    {
        //if(Donotpic != 1)

        {


        }
    }

}


void* picam_process(void*argv)
{


    int wfd;
    int fd;
    int ffd;
    int readn = 1;
    int writen = 0;
    char buf[3];
    char buff[3];
    char gar[51];
    float odd = 0.15;
    int i = 0;
    int j = 0;
    int dif = 0;
    float final = 0;
    float result[3];
    while (1)
    {
        i = 0;
        dif = 0;
        readn = 1;
        memset(gar, 0x00, sizeof(gar));


        system("raspistill -o image1.bmp -e bmp -h 102 -w 102 -n -t 1");
        system("raspistill -o image2.bmp -e bmp -h 102 -w 102 -n -t 1");

        if (((fd = open("image1.bmp", O_RDONLY)) != -1) && ((ffd = open("image2.bmp", O_RDONLY)) != -1))
        {
            read(fd, gar, sizeof(gar));
            read(ffd, gar, sizeof(gar));
            while (readn != 0)
            {
                memset(buf, 0x00, 3);
                readn = read(fd, buf, sizeof(buf));
                memset(buff, 0x00, 3);
                readn = read(ffd, buff, sizeof(buff));
                result[0] = (float)(fabs(((float)buff[0] - (float)buf[0]) / 256));
                result[1] = (float)(fabs(((float)buff[1] - (float)buf[1]) / 256));
                result[2] = (float)(fabs(((float)buff[2] - (float)buf[2]) / 256));

                if (result[0] > odd)
                {
                    i++;
                    dif++;
                }
                else if (result[1] > odd)
                {
                    i++;
                    dif++;
                }
                else if (result[2] > odd)
                {
                    i++;
                    dif++;
                }

                else
                {
                    i++;
                }

            }

            final = (float)(dif) / (float)(i);
            if (final < odd)
            {
                Detected = 0;
            }
            else
            {
                saveimagesuffix++;
                memset(saveimage, 0, sizeof(saveimage));
                sprintf(saveimage, "cp image2.bmp detection_%d", saveimagesuffix);
                system(saveimage);
                Detected = 1;
                system("php /var/www/html/php/push_notification.php");
                Donotpic = 1;
            }
            close(ffd);
            close(fd);
        } //if end

    }       //while end

} // thread _end


void*network_process(void* argv)
{
    key_t SHARED_KEY;
    int data_fd;
    int data_pt = 0;
    char data_temp[MAX_ARR_SIZE];
    char data[MAX_ARR_SIZE];
    char received_data[MAX_ARR_SIZE];
    SHARED_DATA shared_data;
    memset(data, 0, sizeof(data));
    memset(&shared_data, 0, sizeof(shared_data));

    //shared msgget()------------------------------------------------------------
    SHARED_KEY = msgget((key_t)SHARED_KEY_VAL, IPC_CREAT | 0666);
    if (SHARED_KEY == -1)
    {
        printf("failed to create SHARED_KEY\n");
        exit(1);
    }
    //printf("RASPBERRY SERVER STARTED\n\n ");

    pthread_mutex_lock(&mutex);

    shared_data.SHARED_DATA_TYPE = 1;
    strcpy(locate_shared_data(&shared_data, ((data_pt++) % SHARED_DATA_NUM) + 1), "SERVER: RASPBERRY SERVER STARTED\n\n");
    msgsnd(SHARED_KEY, &shared_data, sizeof(shared_data) - sizeof(long), 0);
    pthread_mutex_unlock(&mutex);

    //----------------------------------------------------------------------------------------------------------------
    int serv_fd;
    int clnt_fd;
    int serv_port = atoi((char*)argv);

    pthread_mutex_lock(&mutex);
    //printf("SERVER: 서버 포트: %d\n",serv_port);
    sprintf(data_temp, "SERVER: SERVER PORT: %d\n", serv_port);
    strcpy(locate_shared_data(&shared_data, ((data_pt++) % SHARED_DATA_NUM) + 1), data_temp);
    msgsnd(SHARED_KEY, &shared_data, sizeof(shared_data) - sizeof(long), 0);
    pthread_mutex_unlock(&mutex);

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (serv_fd == -1)
    {
        printf("SOCKET ERROR");
        exit(1);
    }

    struct sockaddr_in serv_addr, clnt_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&clnt_addr, 0, sizeof(clnt_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(serv_port);


    pthread_mutex_lock(&mutex);
    //printf("RASPBERRY IS CURRENTLY BINDING ...\n");
    strcpy(locate_shared_data(&shared_data, ((data_pt++) % SHARED_DATA_NUM) + 1), "RASPBERRY IS CURRENTLY BINDING ...\n");
    msgsnd(SHARED_KEY, &shared_data, sizeof(shared_data) - sizeof(long), 0);
    pthread_mutex_unlock(&mutex);


    if (bind(serv_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("RASPBERRY BIND() FAILED!!!!!!!!!!!!!!!!!!!!! ...\n");
        exit(1);
    }

    if (listen(serv_fd, 5)<0)
    {
        printf("RASPBERRY LISTEN() FAILED!!!!!!!!!!!!!!!!!!!\n");
        exit(1);
    }


    pthread_mutex_lock(&mutex);
    //printf("RASPBERRY IS CURRENTLY LISTENING.\n");
    strcpy(locate_shared_data(&shared_data, ((data_pt++) % SHARED_DATA_NUM) + 1), "RASPBERRY IS CURRENTLY LISTENING.\n");
    msgsnd(SHARED_KEY, &shared_data, sizeof(shared_data) - sizeof(long), 0);
    pthread_mutex_unlock(&mutex);

    //char  sendbuff[MAX_ARR_SIZE]="SERVER: CONNECTION SUCESSFUL!!!\n";
    char  recevbuff[MAX_ARR_SIZE];

    int len = sizeof(clnt_addr);

    while (1)
    {
        data_fd = open("image2.bmp", O_RDONLY);

        if (data_fd == -1)
        {
            //printf("Please ADD serv_in file in our directory\n");
            //exit(1);
            system("touch serv_in");
        }
        clnt_fd = accept(serv_fd, (struct sockaddr*)&clnt_addr, &len);
        int readn = 0;
        if (clnt_fd <0)
        {
            printf("RASPBERRY ACCEPT() FAILED \n");
            //exit(1);
        }

        char clnt_ip_addr[100];

        inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, clnt_ip_addr, sizeof(clnt_ip_addr));
        pthread_mutex_lock(&mutex);
        if ((data_pt % SHARED_DATA_NUM) == 0)
        {
            long temp = shared_data.SHARED_DATA_TYPE;
            memset(&shared_data, 0, sizeof(shared_data));
            shared_data.SHARED_DATA_TYPE = temp;
        }
        sprintf(data_temp, "SERVER: %s client connected \n", clnt_ip_addr);
        //printf("SERVER: %s client connected \n",clnt_ip_addr);
        strcpy(locate_shared_data(&shared_data, ((data_pt++) % SHARED_DATA_NUM) + 1), data_temp);
        msgsnd(SHARED_KEY, &shared_data, sizeof(shared_data) - sizeof(long), 0);
        pthread_mutex_unlock(&mutex);


        /*
        {
        }
        */
        memset(data, 0, sizeof(data));
        memset(received_data, 0, sizeof(received_data));
        read(clnt_fd, received_data, MAX_ARR_SIZE - 1);
        if (!strcmp(received_data, "status"))
        {
            if (Detected == 0)
            {
                write(clnt_fd, status_ok, strlen(status_ok));
            }
            else
            {
                write(clnt_fd, status_not_ok, strlen(status_not_ok));
            }

        }
        else if (!strcmp(received_data, "imagereq"))
        {
            while (readn = read(data_fd, data, MAX_ARR_SIZE - 1))
            {
                write(clnt_fd, data, readn);
                memset(data, 0, sizeof(data));

            }
            Donotpic = 0;
        }



        close(clnt_fd);
	close(data_fd);
        pthread_mutex_lock(&mutex);
        if ((data_pt % SHARED_DATA_NUM) == 0)
        {
            long temp = shared_data.SHARED_DATA_TYPE;
            memset(&shared_data, 0, sizeof(shared_data));
            shared_data.SHARED_DATA_TYPE = temp;
        }



        //printf("SERVER: Connection Sucessfully Closed\n");
        strcpy(locate_shared_data(&shared_data, ((data_pt++) % SHARED_DATA_NUM) + 1), "SERVER: Connection Sucessfully Closed\n");
        msgsnd(SHARED_KEY, &shared_data, sizeof(shared_data) - sizeof(long), 0);
        pthread_mutex_unlock(&mutex);

    }

}






