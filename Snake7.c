#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>


// Spielfeldgrößen
#define WIDTH 60
#define HEIGHT 20
//Spielfeld definitionen
#define SCOREHEIGHT 1

// Funktion Prototypen
void gameField(int score); // Zeichnet das Spielfeld auf
void gotoxy(short x, short y); //Setzt den Konsolencursor an die Position (x, y).
void setup(int *score, int *tailLength, int (*posTail)[2], int (*posFruit)[2]); // Setup für das Spiel - Schlangenposition und erste Frucht generierung
void input(int *direction, _Bool *gameOver); // Bestimmt die Spielereingabe für die Richtung
void backgroundProcess(int (*posTail)[2], int (*posFruit)[2], int direction, int *tailLength, _Bool *gameOver, int *score);// Hintergrundprozesse wie Bewegung der Schlange, prüft Kollisionen, verwaltet das Spielfeld und aktualisiert die Anzeige.
void startScreen(); // Gibt Spielinformation wieder, von informationGame() genutzt um einen Tutorialbilschirm darzustellen
void informationGame(); // Gibt Spielinformation wieder ( Tutorialbilschirm)
void endScreen(int score, int highScore); //Bildschirmausgabe wenn das Spiel vorbei ist (nicht gewonnen)
void highScoreScreen(int score);
int compareScore(int score); //Speichert aktuellen Score und vergleicht mit Highscore
int getHighestScore(int* scores, int count); //Sucht den größten Wert im Array

int getHighestScore(int* scores, int count){
	int max = scores[0];
	for(int i=0; i<count;i++){
		if(max<scores[i]){
			max = scores[i];
		}
	}
	return max;
}
// Speichert den aktuellen Score und gibt den Highscore zurück
int compareScore(int score){
    FILE *fp;
	int countNumber=0;
	int count =0;
	int *allScores;
	int highScore =0;
	char c;
    fp = fopen("test.txt","a+");
    if(fp == NULL){
        return -1;
    }
    fprintf(fp,"%d\n",score);
	fseek(fp,0,SEEK_SET); //Auf den Anfang des Dokuments Springen

	//Scores einlesen aus dem Dokument
	allScores = (int*) malloc(sizeof(int)); 
	if(allScores==NULL){
		return -1;
	}
	while (fscanf(fp,"%d",&allScores[count])==1){
		//printf("%d\n",allScores[count]);
		count++;
		int *tmp = (int*) realloc((void*)allScores,(count+1)*sizeof(int));
		if(allScores==NULL){
			return -1;
		}
		allScores = tmp;
	}
	
	highScore = getHighestScore(allScores, count);
	free(allScores);
	return highScore;
}

//Setzt den Konsolencursor an die Position (x, y).
void gotoxy(short x, short y) {
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { x, y };
    SetConsoleCursorPosition(hStdout, position);
}

// Zeichnet das Spielfeld auf
void gameField(int score) { 
    gotoxy(0, 0); 
    //Ausgabe obere Linie
    printf("Score: %d\n", score);
    for (int i = 0; i < WIDTH; i++) { 
        printf("%c", 205);         
    }
    printf("\n");
    //Ausgabe Seitenlinien
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH-1) {
                printf("%c", 186); 
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    //Ausgabe untere Linie
    for (int i = 0; i < WIDTH; i++) { 
        printf("%c", 205);
    }
    printf("\n");
}

// Setup für das Spiel - Schlangenposition und erste Frucht generierung
void setup(int *score, int *tailLength, int (*posTail)[2], int (*posFruit)[2]) {
    *score = 0;
    *tailLength = 2;
    posTail[0][0] = WIDTH / 2;
    posTail[0][1] = HEIGHT / 2;
    // Setzt die Position der Schlange
    gotoxy(posTail[0][0], posTail[0][1] + SCOREHEIGHT);
    printf("O");
    posTail[1][0] = posTail[0][0];
    posTail[1][1] = posTail[0][1] + 1; //Schwanzteil hinzufügen
    gotoxy(posTail[1][0], posTail[1][1] + SCOREHEIGHT);
    printf("o");
    // Generiert die Frucht
    posFruit[0][0] = rand() % (WIDTH - 3) + 1; 
    posFruit[0][1] = rand() % (HEIGHT - 2) + 1;
    gotoxy(posFruit[0][0], posFruit[0][1] + SCOREHEIGHT);
    printf("F");
}

// Hintergrundprozesse wie Bewegung der Schlange, prüft Kollisionen, verwaltet das Spielfeld und aktualisiert die Anzeige.
void backgroundProcess(int (*posTail)[2], int (*posFruit)[2], int direction, int *tailLength, _Bool *gameOver, int *score) {
    int lastPointX = posTail[0][0];
    int lastPointY = posTail[0][1];
    int tmpX, tmpY;
    _Bool fruit = 0;

    gotoxy(lastPointX, lastPointY + SCOREHEIGHT);
    printf(" "); 

    // Schlangensteuerung
    switch (direction) {
        case 1: 
            posTail[0][0]--;
            break;
        case 2: 
            posTail[0][0]++;
            break;
        case 3: 
            posTail[0][1]--;
            break;
        case 4: 
            posTail[0][1]++;
            break;
        default:
            break;
    }

    // prüft ob die Grenzen erreicht worden sind
    if (posTail[0][0] >= WIDTH - 2) {
        posTail[0][0] = 1;
    } else if (posTail[0][0] < 1) {
        posTail[0][0] = WIDTH - 3;
    }
    if (posTail[0][1] >= HEIGHT + 1) {
        posTail[0][1] = 1;
    } else if (posTail[0][1] < 1) {
        posTail[0][1] = HEIGHT - 1;
    }

    // Prüft ob die Schlange sich selbst getroffen hat
    for (int i = 1; i < *tailLength; i++) {
        if (posTail[0][0] == posTail[i][0] && posTail[0][1] == posTail[i][1]) {
            *gameOver = 1; 
            break;
        }
    }

    // Gibt den neuen Kopf aus
    gotoxy(posTail[0][0], posTail[0][1] + SCOREHEIGHT);
    printf("O"); 

    
    for (int i = 1; i < *tailLength; i++) {
        tmpX = posTail[i][0];
        tmpY = posTail[i][1];
        posTail[i][0] = lastPointX;
        posTail[i][1] = lastPointY;
        lastPointX = tmpX;
        lastPointY = tmpY;

        gotoxy(posTail[i][0], posTail[i][1] + SCOREHEIGHT); 
        printf("o");
    }

    // Prüft ob die Schlange eine Frucht gefressen hat
    if ((posTail[0][0] == posFruit[0][0]) && (posTail[0][1] == posFruit[0][1])) {
        *score +=10;
        gotoxy(7 , 0);
        printf("%d", *score);
        (*tailLength)++;
        posTail[*tailLength - 1][0] = lastPointX; 
        posTail[*tailLength - 1][1] = lastPointY; 

        
        gotoxy(posFruit[0][0], posFruit[0][1] + SCOREHEIGHT);
        printf(" ");
        while (!fruit){
            fruit = 1;
            posFruit[0][0] = rand() % (WIDTH - 3) + 1; 
            posFruit[0][1] = rand() % (HEIGHT - 2) + 1; 
            for (int i = 1; i < *tailLength; i++){
                if(posTail[i][0] == posFruit[0][0] && posTail[i][1] == posFruit[0][1]){
                    fruit = 0;
                }
            }
        }
        
        gotoxy(posFruit[0][0], posFruit[0][1] + SCOREHEIGHT);
        printf("F");
    } else {
        gotoxy(lastPointX, lastPointY + SCOREHEIGHT); 
        printf(" ");
    }
    gotoxy(0, HEIGHT + 3) ;
}
//Speichert aktuellen Score und vergleicht mit Highscore


// Bestimmt die Spielereingabe für die Richtung
void input(int *direction, _Bool *gameOver) { 
    if (_kbhit()) { 
        switch (_getch()) { 
            case 'a': 
                *direction = 1; 
                break;
            case 'd': 
                *direction = 2; 
                break;
            case 'w': 
                *direction = 3; 
                break;
            case 's': 
                *direction = 4; 
                break;
            case 75: 
                *direction = 1; 
                break;
            case 77: 
                *direction = 2; 
                break;
            case 72: 
                *direction = 3; 
                break;
            case 80: 
                *direction = 4; 
                break;
            case 27: 
                *gameOver = 1; 
                break;
            default: 
                break;
        }
    }
}

// Gibt Spielinfos wieder
void startScreen(){
    gotoxy(5,5);
    printf("Snake is a video game where you control a snake");
    gotoxy(5,6);
    printf("and try to eat as many fruits as you can");
    gotoxy(5,7);
    printf("without colliding with the snakes itself.");
    gotoxy(5,9);
    printf("Press 'w' to move up");
    gotoxy(5,10);
    printf("Press 'a' to move left");
    gotoxy(5,11);
    printf("Press 's' to move down");
    gotoxy(5,12);
    printf("Press 'd' to move righ");
    gotoxy(5,13);
    printf("Press 'ESC' to exit");
    gotoxy(5,14);
    printf("Press any key to start the game");
    
}

// Gibt Spielinformation wieder ( Tutorialbilschirm)
void informationGame(){
    gameField(0); 
    startScreen(); 
    if (_getch()) { 
        system("cls"); 
    }
}

// Funktion wenn das Spiel vorbei ist (nicht gewonnen)
void endScreen(int score, int highScore){
    gotoxy(5,5);
    printf("Game Over!");
    gotoxy(5,7);
    printf("Your Score: %d", score);
    gotoxy(5,8);
    printf("Your Highscore: %d", highScore);
    gotoxy(5,10);
    printf("Thanks for playing our Snake Game!");
    gotoxy(0,25);
    /*
    if (_getch()) { //Wait for any key input
        main();
    }
    */
}

void highScoreScreen(int score){
    gotoxy(5,5);
    printf("New Highscore!");
    gotoxy(5,7);
    printf("Your Score: %d", score);
    gotoxy(5,10);
    printf("Thanks for playing our Snake Game!t");
    gotoxy(0,25);
}



// main Funktion 
int main() {
    int posTail[1000][2] = {0}; 
    int posFruit[100][2] = {0}; 
    int score = 0; 
    int tailLength;
    int direction = 0;
    _Bool gameOver = 0;

    srand(time(NULL)); 
    informationGame(); 
    gameField(score); 
    setup(&score, &tailLength, posTail, posFruit); 
    
    // Game Loop
    while (!gameOver) {
        input(&direction, &gameOver); 
        if(direction != 0){
            backgroundProcess(posTail, posFruit, direction, &tailLength, &gameOver, &score);
        }
        if (score > 50) {
            Sleep(150);
        } else if (score > 100) {
            Sleep(125);
        } else if (score > 150) {
            Sleep(115);
        } else if (score > 200) {
            Sleep(105);
        } else {
            Sleep(200);
        }
    }
    int highscore = compareScore(score);
    if(highscore == score){
        highScoreScreen(score);
    }else{
        endScreen(score,highscore);
    }
    system("Pause");
    return 0;
}
