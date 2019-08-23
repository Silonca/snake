#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>


enum direction { UP, DOWN, LEFT, RIGHT};
enum mapsymbol { PLACE,HEAD, TAIL, BODY, WALL, FOOD};
enum foodstate { FOOD_EATEN, FOOD_NOT_EATEN};


typedef struct Node
{
    struct Node *next;
    struct Node *last;
    int x,y;
} Node;

typedef struct List
{
    Node *head;
    Node *tail;
    int longth;
} List;

void Init( List *list);
_Bool AddFirst( List *list, int x, int y);
void DeleteLast( List *list);
void DeleteAll( List *list);
bool EatItself( List *list, int dir);
bool isWall( int x, int y, int w, int h);
void CreateFood( List *list, int *x, int *y, int w, int h);
int Move( List *list, int x, int y, int dir);

void gotoxy(int x,int y);
void HideCursor();

int dirmove[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};


void gotoxy(int x, int y)
{
COORD coord = {x, y};
SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HideCursor()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hOut, &cci);
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cci);
}

void Init( List *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->longth = 0;
}

_Bool AddFirst( List *list, int x, int y)
{
    Node *newnode = ( Node*) malloc( sizeof( Node));
    if( newnode == NULL) return false;
    if( list->head != NULL)
        list->head->last = newnode;
    if( list->tail == NULL) list->tail = newnode;
    newnode->next = list->head;
    newnode->last = NULL;
    newnode->x = x;
    newnode->y = y;
    list->head = newnode;
    ++list->longth;
    return true;
}

void DeleteLast( List *list)
{
    if(list->tail == NULL)
        return ;

    Node *temp = list->tail->last;
    free( list->tail);
    --list->longth;
    if( temp == NULL)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        temp->next = NULL;
        list->tail = temp;
    }
}

void DeleteAll( List *list)
{
    while( list->tail != NULL)
        DeleteLast(list);
    //free(list->head);
    //list->head = NULL;
    //list->longth = 0;
}


_Bool EatItself( List *list, int dir)
{
    Node *temp = list->head->next;

    if( temp == NULL)
        return false;
    while( true)
    {
        if( list->head->x + dirmove[dir][0] == temp->x && list->head->y + dirmove[dir][1] == temp->y)
            return true;

        if( temp->next != NULL)
            temp = temp->next;
        else break;
    }
    return false;
}



_Bool isWall( int x, int y, int w, int h)
{
    if( x<0 || y<0 || x>=w || y >=h)
        return true;
    else return false;
}


void CreateFood( List *list, int *x, int *y, int w, int h)
{
    while( true)
    {
        *x = rand() % w;
        *y = rand() % h;

        Node *temp = list->head;
        while( temp != NULL)
        {
            if( temp->x != *x && temp->y != *y)
                return ;
            temp = temp->next;
        }
    }
}


int Move( List *list, int x, int y, int dir)
{
    if( list->head->x + dirmove[dir][0] == x && list->head->y + dirmove[dir][1] == y)
    {
        AddFirst( list, x, y);
        return FOOD_EATEN;

    }
    else
    {
        AddFirst( list, list->head->x + dirmove[dir][0], list->head->y + dirmove[dir][1]);
        DeleteLast( list);
    }
    return FOOD_NOT_EATEN;
}




int main()
{
    List list;
    int w,h;
    char input;
    int dir;


    Init( &list);

    srand( ( unsigned) time( NULL));

    printf("please input the width and height.\n");
    scanf("%d %d", &w, &h);
    getchar();
    printf("please input w,s,a or d to start the game\n");
    input = getchar();
    HideCursor();

    char map[w+2][h+2];

    char map_buffer[(w+2)*(h+2)];

    setvbuf( stdout, map_buffer, _IOFBF, (w+2)*(h+2));



    switch(input)
    {
    case 'w':
        dir = UP;
        break;
    case 's':
        dir = DOWN;
        break;
    case 'a':
        dir = LEFT;
        break;
    case 'd':
        dir = RIGHT;
        break;
    }

    system("cls");

    AddFirst( &list, w/2, h/2);
    AddFirst( &list, w/2 + dirmove[dir][0], h/2 + dirmove[dir][1]);


    _Bool death = false;
    int x,y;
    CreateFood( &list, &x, &y, w, h);


    while(true)
    {
        for( int a = 0; a< w + 2; ++a)
            for( int b = 0; b< h + 2; ++b)
                map[a][b] = 0;

        if( kbhit())
            input = getch();
        switch(input)
        {
        case 'w':
            if( dir != DOWN) dir = UP;
            break;
        case 's':
            if( dir != UP) dir = DOWN;
            break;
        case 'a':
            if( dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if( dir != LEFT) dir = RIGHT;
            break;
        }

        if( isWall( list.head->x + dirmove[dir][0], list.head->y + dirmove[dir][1], w, h) == false && EatItself( &list, dir) == false)
        {
            if( Move( &list, x, y, dir) == FOOD_EATEN)
                CreateFood( &list, &x, &y, w, h);
        }
        else death = true;


        map[x+1][y+1] = FOOD;

        Node *temp = list.head;
        while( true)
        {
            map[ temp->x + 1][ temp->y + 1] = BODY;
            if( temp->next != NULL)
                temp = temp->next;
            else break;

        }

        map[ list.head->x+1][ list.head->y+1] = HEAD;
        map[ list.tail->x+1][ list.tail->y+1] = TAIL;

        for( int a = 0; a < w + 2; ++a)
        {
            map[a][0] = WALL;
            map[a][h + 1] = WALL;
        }
        for( int b = 0; b < w + 2; ++b)
        {
            map[0][b] = WALL;
            map[w + 1][b] = WALL;
        }
        gotoxy( 0,0);

        for( int a = 0; a < w + 2; ++a)
        {
            for( int b = 0; b < h + 2; ++b)
            {
                switch( map[a][b])
                {
                case PLACE:
                    printf(" ");
                    break;
                case WALL:
                    printf("#");
                    break;
                case HEAD:
                    printf("@");
                    break;
                case BODY:
                    printf("*");
                    break;
                case TAIL:
                    printf("+");
                    break;
                case FOOD:
                    printf("o");
                    break;
                }
            }

            printf("\n");
        }
        fflush( stdout);
        if( death == true) break;
        Sleep(250);
    }
    setvbuf(stdout, NULL, _IONBF, (w+2)*(h+2));
    printf("Game over.\n");
    DeleteAll( &list);

    system("pause");


    return 0;
}
