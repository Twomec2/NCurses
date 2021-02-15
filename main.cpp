#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "sound.h"
#include "graphics.h"

/*Autor: Tomasz Kowalik */
static int player1S = 0;
static int player2S = 0;


static int mx = COLS, my = LINES; //Anzahl der Zeilen und Spalten Q: Tutorial
static char paddle[]="________________________";
void paddle_1(int highpad);       //erster paddle die position wird durch int geändert
void paddle_2(int lowpad);        //zweites paddle die postition wird durch int geändert
void draw();                      //Startseite mit hinweisen für spieler   Q: Tutorial
void gameloop();                  //Gameloop/Steuerung      Q: Tutorial
void animation(float t, float dt);//der Kollusions dedektor Q: Tutorial
void boing();                      //sound für den Ball wenn er kollidiert
void titelmusik();                 //titelmusik

int main()
{
  initscr();                         //screen initalisieren
  curs_set(FALSE);
  noecho();
  getmaxyx(stdscr, my, mx);
  sound_init();
  clear();
  titelmusik();
  draw();
  while (int ch = getch())
  {
    switch(ch) {
      case 'q': endwin();
      return(0);                    //wenn  "q" gedrückt wird dann beenden
      case ' ': clear();      //wenn spacetaste gedrückt wird spiel starten
      draw();
      gameloop();
    }
  }
  sound_exit();
  return(0);
}
//StartBildschirm
void draw()
{
  //farbkombinationen Auswahl
  start_color();
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_GREEN, COLOR_GREEN);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLUE);
  init_pair(5, COLOR_RED, COLOR_RED);
  init_pair(6, COLOR_WHITE, COLOR_RED);
  init_pair(7, COLOR_WHITE, COLOR_BLUE);
  init_pair(8, COLOR_WHITE, COLOR_BLACK);
  //colored frame
  //border für Startbildschirm
  color_set(3, NULL);
  border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
    ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

    //aus dem Tutorial
    const char sprit[]=
    (   "PPPPPPPPPPPPPPPP        OOOOOOOOO    NNNNNNNN        NNNNNNNN       GGGGGGGGGGGGG\n"
    "P::::::::::::::::P    OO:::::::::OO  N:::::::N       N::::::N    GGG::::::::::::G\n"
    "P::::::PPPPPP:::::P OO:::::::::::::OON::::::::N      N::::::N  GG:::::::::::::::G\n"
    "PP:::::P     P:::::O:::::::OOO:::::::N:::::::::N     N::::::N G:::::GGGGGGGG::::G\n"
    "  P::::P     P:::::O::::::O   O::::::N::::::::::N    N::::::NG:::::G       GGGGGG  \n"
    "  P::::P     P:::::O:::::O     O:::::N:::::::::::N   N::::::G:::::G                \n"
    "  P::::PPPPPP:::::PO:::::O     O:::::N:::::::N::::N  N::::::G:::::G                \n"
    "  P:::::::::::::PP O:::::O     O:::::N::::::N N::::N N::::::G:::::G    GGGGGGGGGG  \n"
    "  P::::PPPPPPPPP   O:::::O     O:::::N::::::N  N::::N:::::::G:::::G    G::::::::G  \n"
    "  P::::P           O:::::O     O:::::N::::::N   N:::::::::::G:::::G    GGGGG::::G  \n"
    "  P::::P           O:::::O     O:::::N::::::N    N::::::::::G:::::G        G::::G  \n"
    "  P::::P           O::::::O   O::::::N::::::N     N:::::::::NG:::::G       G::::G  \n"
    " PP::::::PP        O:::::::OOO:::::::N::::::N      N::::::::N G:::::GGGGGGGG::::G\n"
    " P::::::::P         OO:::::::::::::OON::::::N       N:::::::N  GG:::::::::::::::G\n"
    " P::::::::P           OO:::::::::OO  N::::::N        N::::::N    GGG::::::GGG:::G\n"
    " PPPPPPPPPP             OOOOOOOOO    NNNNNNNN         NNNNNNN       GGGGGG   GGG \n");

    draw_sprite(10, 15, sprit);
    //text zum Starten vom Spiel mittig gesetzt
    attrset(A_BLINK); //blinkend
    color_set(3,NULL);
    const char start[] = "Press space to Start";
    move(my-4, mx/2-(strlen(start)/2));
    printw(start);
    //anweisung player 1
    attrset(A_BOLD);
    color_set(7,NULL);
    const char player_one[] = "Player 1 controlls a = left & d = right";
    move(1, 2);
    printw(player_one);
    //anweisung player 2
    attrset(A_BOLD);
    color_set(6,NULL);
    const char player_two[] = "Player 2 controlls j= left & l = right";
    move(1, mx-(strlen(player_two))-2);
    printw(player_two);


  }
  void gameloop()
  {

    //aus dem Tutorial
    int fps = 20;
    float dt = 1.0/fps;
    int us = 1000 * 1000 * dt;
    float t = 0;
    static int paddle1_x = mx/2-8;
    static int paddle2_x = mx/2-8;
    timeout(0);


    while(1)
    {

      clear();
      getmaxyx(stdscr, my, mx);
      attrset(A_BOLD);
      color_set(3,NULL);
      border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
        ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

        //spiel "menü"
        attrset(A_BLINK);
        color_set(3,NULL);
        const char stop[] = "Press p to Stop";
        move(my/2-3, mx-(strlen(stop))-2);
        printw(stop);
        attrset(A_BLINK);
        color_set(3,NULL);
        const char quit[] = "Press q to Quit";
        move(my/2-2, mx-(strlen(quit))-2);
        printw(quit);

        // Paddle Zeichnen
        attrset(A_BOLD);
        color_set(5,NULL);
        mvprintw(1,paddle2_x,paddle);
        color_set(4,NULL);
        mvprintw(my-2,paddle1_x,paddle);
        color_set(8,NULL);

        // Spielstand

        mvprintw(my-6,2,"Punkte: %i",player1S);
        mvprintw(4,mx-12,"Punkte: %i",player2S);

        animation(t, dt);

        refresh();
        usleep(us);

        //steuerung der Paddels

        t += dt;
        int ch = getch();
        int len = strlen(paddle);
        if(ch != EOF)
        {
        }
        if(tolower(ch)=='p')
        {
          while(1)
          {
            int c = getch();
            if(tolower(c)=='p')
            {
              break;
            }
          }
        }
        if(tolower(ch)=='q')
        {
          break;
        }
        if(tolower(ch)== 'a')
        {
          if(paddle1_x>2)
          {
            paddle1_x -=5;
          }
        }
        if(tolower(ch)== 'd')
        {
          if(paddle1_x+len < mx-2)
          {
            paddle1_x +=5;
          }
        }
        if(tolower(ch)== 'j')
        {
          if(paddle2_x>2)
          {
            paddle2_x -=5;
          }
        }
        if(tolower(ch)== 'l')
        {
          if(paddle2_x+len< mx-2)
          {
            paddle2_x +=5;
          }
        }
      }
    }
    //Quelle: Tutorial
    void animation(float t, float dt)
    {
      static float bx = mx/2, by = my/2;
      static float dbx = 20, dby = 20;  //geschwindigkeit
      bx += dbx * dt;
      by += dby * dt;
      //Ball wird erzeugt
      color_set(2, NULL);
      mvaddch(by, bx, 'O');
      //collision detection aus dem Tutorial
      int mask = A_CHARTEXT | A_ALTCHARSET;
      if(dbx < 0)
      {
        if (bx-1<1)
        {
          dbx = -dbx;
          boing();
        }
      }
      if(dbx > 0)
      {
        if (bx+1>mx-2)
        {
          dbx = -dbx;
          boing();
        }
      }
      if(dby < 0)
      {
        int ch = mvinch(by-1, bx) & mask;
        if (ch == '_')
        {
          dby = -dby;
          boing();
        }
        else if(by < 0)
        {
          sound_play("hit.wav");
          player1S++;
          by = my/2;
          bx = mx/2;
        }
      }
      if(dby > 0)
      {
        int ch = mvinch(by+1, bx) & mask;
        if (ch == '_')
        { dby = -dby;
          boing();
        }
        else if(by > my)
        {
          sound_play("hit.wav");
          player2S++;
          by = my/2;
          bx = mx/2;
        }
      }
    }
    //Quelle: Tutorial
    void boing()
    {
      sound_play("boing.wav");
    }
    void titelmusik()
    {
      sound_play("starter.wav");
    }
