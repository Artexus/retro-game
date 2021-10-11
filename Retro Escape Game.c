#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include<time.h>
typedef struct enemy{
	int x;
	int y;
	char symbol;
	int moveTime;
}Enemy;
typedef struct map{
	char format[100][100];
	int height;
	int width;
	int escapeGateX,escapeGateY;
	int count;
}Maps;
typedef struct BFS{
	int x,y;
	int before;
}BFS;
typedef struct Queue{
	BFS bfs;
	struct Queue *next;
}Queue;
typedef struct Player{
	char symbol;
	int x;
	int y;
}Player;
typedef struct enemies{
	Enemy x;
	struct enemies *next;
}Enemies;

Enemies *head = NULL,*tail = NULL;
Maps map;
Player player;
bool winning;
bool running;
char bfs(Enemies*);
void printMenu();
void createEnemy(int);
Enemies *generateEnemy(int,int,int);
void generateMap();
void cls();
Queue *createNewQueue(BFS bfs);
void putToMap();
void enqueue(Queue**,Queue **,BFS);
void dequeue(Queue**,Queue **);
void printScore();
void howToPlay();
void playGame();
void createPlayer();
void startGame();
void dispose();
void movePlayer(char);
void moveEnemy();
void printMap();
void popAllEnemy();
void selectDif();
void pushTail(Enemies *);
void cls_rh();
int main()
{
	srand(time(NULL));
    int choice;
    while(true){
    		generateMap();
	    do
	    {
	        cls();
	        printMenu();
	        scanf("%d", &choice);getchar();
	    } while (choice < 1 || choice > 3);
	    switch (choice)
	    {
		    case 1:
		        playGame();
		        dispose();
		        break;
		    case 2:
		        howToPlay();
		        break;
		    case 3:
		        return 0;
	    }
	}
}
void dispose(){
	//DELETE ENEMY
	popAllEnemy();
}
void popAllEnemy(){
	while(head){
		if(head == tail){
			free(head);
			head = tail = NULL;
		}
		else{
			Enemies *temp = head;
			head = head->next;
			free(temp);
		}
	}
}
void generateMap(){
	FILE *fptr = fopen("level1.txt","r");
	
	if(!fptr){
		printf("level1.txt not found!");
		return;
	}
	int y = 0;
	while(!feof(fptr)){
		fscanf(fptr,"%[^\n]\n",map.format[y]);
		++y;
	}
	map.width = strlen(map.format[0]);
	map.height = y;
	map.count = 0;
	for(int i = 0 ; i < map.height ; i ++){
		for(int j = 0; j < map.width ; j ++){
			if(map.format[j][i] == '|'){
				map.escapeGateX = j;
				map.escapeGateY = i;
			}
		}
	}
}
void howToPlay()
{
    cls();
    printf("\n        Welcome to FuneScape,\n\n");
    printf("        You are now locked inside this maze by the evil wizard Efes,\n\n");
    printf("        but there is no maze that has no end!\n");
    printf("        If you look carefully you could find the escape door symbolized by '|' on the map.\n");
    printf("        You should find 5 key fragments scattered around the maze in order to unlock that door and escape.\n");
    printf("        The fragments are symbolized by '*' on the map, to get it you should travel the maze carefully!\n\n");
    printf("        Use 'W' to move upwards, 'S' to move downwards, 'A' to move left and 'D' to move right.\n");
    printf("        After you gather all of the fragments the door should be automatically opened.\n");
    printf("        Be quick before the monsters of the maze catch up on you!\n\n");
    printf("        Press any key to continue . .\n");
    getchar();
}
void createEnemy(int time){
	Enemies *x1 = generateEnemy(time,map.width-3,2);
	pushTail(x1);
	
	Enemies *x2 = generateEnemy(time,2,map.height - 3);
	pushTail(x2);
	
	Enemies *x3 = generateEnemy(time,map.width-3,map.height - 3);
	pushTail(x3);
}
void pushTail(Enemies *x){
	if(!head){
		head = tail = x;
	}
	else{
		tail->next = x;
		tail = x;
	}
}
Enemies *generateEnemy(int time,int posX,int posY){
	Enemies *temp = (Enemies *) malloc(sizeof(Enemies));
	temp->x.moveTime = time;
	temp->x.x = posX;
	temp->next = NULL;
	temp->x.symbol = 'E';
	temp->x.y = posY;
	return temp;
}
void playGame()
{
	int time;
	int choice;
	do{
		cls();
		selectDif();
		scanf("%d",&choice);getchar();
	}while(choice < 1 || choice > 3);
	switch(choice){
		case 1:
			time = 1;
			break;
		case 2:
			time = 2;
			break;
		case 3:
			time = 3;
			break;
	}
	createEnemy(time);
	createPlayer();	
	putToMap();

	startGame();
}
void moveEnemy(){

	Enemies *temp = head;
	while(temp){
		int velX,velY;
		char move = bfs(temp);
		switch(move){
			case 'a':
				velX = -1;
				velY = 0;
				break;
			case 'd':
				velX = 1;
				velY = 0;
				break;
			case 'w':
				velX = 0;
				velY = -1;
				break;
			case 's':
				velX = 0;
				velY = 1;
				break;
			default:
				velX = 0;
				velY = 0;
		}
		
		if(map.format[temp->x.y + velY][temp->x.x + velX] == 'P') // ENEMY WIN
		{
			running = false;
			winning = false;
		}
		map.format[temp->x.y][temp->x.x] = ' ';
		map.format[temp->x.y + velY][temp->x.x + velX] = temp->x.symbol;
		temp->x.x += velX;
		temp->x.y += velY;
		temp = temp->next;
	}
}
char bfs(Enemies *temp){
	BFS bfs;
	BFS array[map.height * map.width];
	int index = 0;
	Queue *headQ = NULL,*tailQ = NULL;
	bool visited[map.height + 1][map.width + 1];
	for(int i = 0 ; i < map.height; i ++){
		for(int j = 0; j < map.width ; j ++){
			if(map.format[i][j] != ' ' && map.format[i][j] != 'P'){
				visited[i][j] = true;
			}
			else{
				visited[i][j] = false;
			}
		}
	}
	bfs.x = temp->x.x;
	bfs.y = temp->x.y;
	bfs.before = -1;
	enqueue(&headQ,&tailQ,bfs);
	array[index ++] = bfs;
	visited[bfs.y][bfs.x] = true;
	int curr;
	while(headQ){
		BFS t = headQ->bfs;
		for(int i = 0 ; i < index ; i ++){
			if(t.x == array[i].x && t.y == array[i].y)
				curr = i;
		}
		dequeue(&headQ,&tailQ);
		if(map.format[t.y][t.x] == 'P') // DEST FOUND
		{

			//GET 1ST MOVEMENT
			while(t.before != 0){
				t = array[t.before];
			}
			int velX,velY;
			velX = t.x - temp->x.x;
			velY = t.y - temp->x.y;
			if(velX == -1) // left
				return 'a';
			if (velX == 1) // right
				return 'd';
			if(velY == -1) //up
				return 'w';
			if(velY == 1) // down
				return 's';
		}
		
		// GET NEIGHBOUR
		//MoveUp
		if(t.y - 1 >= 0 && !visited[t.y - 1][t.x]){
			BFS t1;
			t1.x = t.x;
			t1.y = t.y - 1;
			t1.before = curr;
			enqueue(&headQ,&tailQ,t1);
			visited[t1.y][t1.x] = true;
			array[index ++] = t1;
		}
		//MoveDown
		if(t.y + 1 < map.height && !visited[t.y + 1][t.x] ){
			BFS t1;
			t1.x = t.x;
			t1.y = t.y + 1;
			t1.before = curr;
			enqueue(&headQ,&tailQ,t1);
			visited[t1.y][t1.x] = true;
			array[index ++] = t1;
		}
		//MoveLeft
		if(t.x - 1 >= 0 && !visited[t.y][t.x - 1]){
			BFS t1;
			t1.x = t.x - 1;
			t1.y = t.y;
			t1.before = curr;
			enqueue(&headQ,&tailQ,t1);
			visited[t1.y][t1.x] = true;
			array[index ++] = t1;
		}
		//MoveRight
		if(t.x + 1< map.width && !visited[t.y][t.x + 1]){
			BFS t1;
			t1.x = t.x + 1;
			t1.y = t.y;
			t1.before = curr;
			enqueue(&headQ,&tailQ,t1);
			visited[t1.y][t1.x] = true;
			array[index ++] = t1;
		}
	}
	return ' ';
}
void dequeue(Queue **headQ,Queue **tailQ){
	if((*headQ)){
		if((*headQ) == (*tailQ)){
			(*headQ) = (*tailQ) = NULL;
		}else{
			Queue *temp = (*headQ);
			(*headQ) = (*headQ)->next;
			free(temp);
		}
	}
}
void enqueue(Queue **headQ,Queue **tailQ,BFS bfs){
	Queue *temp = createNewQueue(bfs);
	if(!(*headQ)){
		(*headQ) = (*tailQ) = temp;
	}
	else {
		(*tailQ)->next = temp;
		(*tailQ) = temp;
	}
}
Queue *createNewQueue(BFS bfs){
	Queue *temp = (Queue *) malloc(sizeof(Queue));
	temp->bfs = bfs;
	temp->next = NULL;
	return temp;
}
void startGame(){
	running = true;
	winning = false;
	int loop = 0;
	double delay = 180.0;
	bool firstHit = false;
	while(running && !winning){
		//INPUT
		if(kbhit()){
			char temp = getch();
			movePlayer(temp);
			firstHit = true;	
		}			
		if(firstHit){
			if(head->x.moveTime == loop){
				moveEnemy();
				loop = 0;
			}
			loop ++;
		}
		//update
		if(map.count == 5){
			map.format[map.escapeGateY][map.escapeGateX] = ' ';
		}
		if(player.x == map.escapeGateX && player.y == map.escapeGateY){
			winning = true;
			running = false;
		}
		//DISPLAY
		cls_rh();
		printMap();
		printScore();
	
		Sleep(delay);
	}
	cls();
	if(winning){
		printf("CONGRATULATION YOU WIN !\n");
	}
	else{
		printf("YOU LOST !\n");
	}
	printf("Press enter to continue...");
	getchar();
}
void printScore(){
	printf("Key Fragments Collected : %d\n",map.count);
}
void movePlayer(char temp){
	int velX,velY;
	switch(temp){
		case 'w':
			velX = 0;
			velY = -1;
			break;
		case 'a':
			velX = -1;
			velY = 0;
			break;
		case 'd':
			velX = 1;
			velY = 0;
			break;
		case 's':
			velX = 0;
			velY = 1;
			break;
		default:
			return;
	}
	if(map.format[player.y + velY][player.x + velX] == ' ' || map.format[player.y + velY][player.x + velX] == '*'){
		if(map.format[player.y + velY][player.x + velX] == '*'){
			map.count ++;
		}
		map.format[player.y][player.x] = ' ';
		map.format[player.y + velY][player.x + velX] = player.symbol;
		player.y += velY;
		player.x += velX;
	}
}
void printMap(){
	for(int i = 0 ; i < map.height ; i ++){
		printf("%s\n",map.format[i]);
	}
}
void putToMap(){
	//Player
	map.format[player.y][player.x] = player.symbol;
	
	//ENEMY
	Enemies *temp = head;
	while(temp){
		map.format[temp->x.y][temp->x.x] = temp->x.symbol;
		temp = temp->next;
	}
	//Fragment
	for(int i = 0 ; i < 5; i ++){
		int x,y;
		do{
			x = rand() % map.width;
			y = rand() % map.height;
		}while(map.format[y][x]!= ' ');
		map.format[y][x] = '*';
	}
}
void createPlayer(){
	player.symbol = 'P';
	player.x = 2;
	player.y = 2;
}
void selectDif(){
	printf("        Select Difficulty\n");
	printf("        1. Insane\n");
	printf("        2. Hard\n");
	printf("        3. Easy\n");
	printf("        ");
}
void cls(){
	system("cls");
}
void cls_rh()
{
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}
void printMenu()
{
    printf("         _____                 ____\n");
    printf("        |  ___|   _ _ __   ___/ ___|  ___ __ _ _ __   ___\n");
    printf("        | |_ | | | | '_ \\ / _ \\___ \\ / __/ _` | '_ \\ / _ \\\n");
    printf("        |  _|| |_| | | | |  __/___) | (_| (_| | |_) |  __/\n");
    printf("        |_|   \\__,_|_| |_|\\___|____/ \\___\\__,_| .__/ \\___|\n");
    printf("                                              |_|\n\n");
    printf("        1. Play\n");
    printf("        2. How to play\n");
    printf("        3. Exit game\n");
    printf("        ");
}
