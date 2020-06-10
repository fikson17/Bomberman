#include<GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#define TIMER_INTERVAL 30
#define PI 3.1415926535897

//struktura za paukove
typedef struct{
	float x;
	float z;
	int smer;
	int p;
	int id;
}Spider;

typedef struct{
	float x;
	float z;
	float old_smer;
	float new_smer;
}Bomberman;

typedef struct{
	int x;
	int z;
	int ind;
	float bomb_pump;
}Bomba;

static void on_display();
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_keyboardUp(unsigned char key, int x, int y);

//f-ja za pravljenje mape
void randomize_walls(void);

//tajmer
static void timer(int id);
int ID;
float animation_ongoing = 0;
float spider_brojac = 0;

//bombermen
Bomberman bombermen;
int indikator = 1;
float move_par = 0;
int sledeci_pokret = 0;

//bomba
Bomba bomba;
void draw_bomb();
//

//spider
Spider spider1;
Spider spider2;
float spider_param;
void draw_spider(float i, float j);
void spider_walk(Spider* spider);
//

//matrica koja odredjuje celu mapu
static int destroy_block[17][13];
//indikator kada je zavrsena igrica
int game_over = 0;
//indikator kada je pritisnuto dugme
int KEY_DOWN_W = 0;
int KEY_DOWN_S = 0;
int KEY_DOWN_A = 0;
int KEY_DOWN_D = 0;
int KEY_DOWN = 0;

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(1200,800);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);

	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutKeyboardFunc(on_keyboard);
	glutKeyboardUpFunc(on_keyboardUp);
	
	//#############
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	//#############
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	float light_position[] = {-1, 3, 2, 1};
    float light_ambient[] = {.3f, .3f, .3f, 1};
    float light_diffuse[] = {.7f, .7f, .7f, 1};
    float light_specular[] = {.7f, .7f, .7f, 1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    
    glEnable(GL_COLOR_MATERIAL);
    
    //nasumicno inicijalizovanje smera kretanja paukova
    srand(time(0));
    spider1.smer = rand()%4;
    spider2.smer = rand()%4;
    //indikator da su paukovi zivi
	spider1.p = 1;
	spider2.p = 1;
    
    //inicijalizacija bombe
    bomba.x = -1;
	bomba.z = -1;
	bomba.ind = 0;
	bomba.bomb_pump = 0;
	
    //pravljenje mape
    randomize_walls();
	
	
	glClearColor(0.7, 0.7, 0.7, 0);
	glutMainLoop();
	
	return 0;
}

void randomize_walls(void){
	srand(time(0));
    int i,j;
    //rasporedjivanje unistivih i neunistivih zidova
    for(i=0; i<17; i++){
		for(j=0; j<13; j++){
			if(i == 0 || j == 0 || j == 12 || i == 16 || (j%2==0 && i%2==0))
				destroy_block[i][j] = 1;
			else if(((int)rand())%3 == 0)
				destroy_block[i][j] = 2;
			else 
				destroy_block[i][j] = 0;
		}
	}
	
	//prepravljanje mape da se bombermen ne bi stvorio izmedju zidova
	destroy_block[1][1] = 0;
	bombermen.x = 0;
	bombermen.z = 0;
	bombermen.old_smer = 0;
	bombermen.new_smer = 0;
	destroy_block[1][2] = 0;
	destroy_block[2][1] = 0;
	
	//nasumicno odredjivanje lokacije paukova
	int n = 0;
	while(n!=2){
		i = ((int)rand())%6 + 6;
		j = ((int)rand())%4 + 4;
		if(destroy_block[i][j] == 0){
			if(n==0){
				spider1.x = (i-1)*10;
				spider1.z = (j-1)*10;
				destroy_block[i][j] = 0;
			}
			else if(n==1){
				spider2.x = (i-1)*10;
				spider2.z = (j-1)*10;
				destroy_block[i][j] = 0;
			}
			n++;
		}
	}
}

void draw_axes(float len){
	glDisable(GL_LIGHTING);
	
	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(len,0,0);
		
		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,len,0);
		
		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,len);
	glEnd();
	
	glEnable(GL_LIGHTING);
}

void on_keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:
		//ESC = iskljucivanje aplikacije
			exit(0);
			break;
		case 13:
		//ENTER = pauza ili pokretanje
			if(animation_ongoing == 0 && game_over == 0){
                animation_ongoing = 1;
				glutTimerFunc(TIMER_INTERVAL,timer,0);
			}
			else if(animation_ongoing == 1 && game_over == 0){
				animation_ongoing = 0;
			}
			break;
		case 'r':
		//restart
			animation_ongoing = 0;
			ID = 0;
			bombermen.x = 0;
			bombermen.z = 0;
			indikator = 1;
            move_par = 0;
            game_over = 0;
            spider1.p = 1;
            spider2.p = 1;
            bomba.bomb_pump = 0;
			bomba.ind = 0;
			bomba.x = -1;
			bomba.z = -1;
            randomize_walls();
			glutPostRedisplay();
			break;
		////////////    KONTROLE    //////////////////	
		case 'w':
		//gore
					
					KEY_DOWN_W = 1;
					KEY_DOWN++;
				if(animation_ongoing==1 && (destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10] != 1 && 
					destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10] != 2) && (int)bombermen.x % 10 == 0 && (int)bombermen.z % 10 ==0){
					bombermen.new_smer = 2.0;
                    ID = 1;
				}
			break;
		case 's':
		//dole
					
					KEY_DOWN_S = 1;
					KEY_DOWN++;
				if(animation_ongoing==1 && (destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10 + 2] != 1 &&
					destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10 + 2] != 2) && (int)bombermen.x % 10 == 0 && (int)bombermen.z % 10 ==0){
					bombermen.new_smer = 0;
                    ID = 2;
				}
			break;
		case 'a':
		//levo
					
					KEY_DOWN_A = 1;
					KEY_DOWN++;
				if(animation_ongoing==1 && (destroy_block[(int)bombermen.x/10][(int)bombermen.z/10 + 1] != 1 &&
					destroy_block[(int)bombermen.x/10][(int)bombermen.z/10 + 1] != 2) && (int)bombermen.x % 10 == 0 && (int)bombermen.z % 10 ==0){
					bombermen.new_smer = -1.0;
                    ID = 3;
				}
			break;
		case 'd':
		//desno
		
					KEY_DOWN_D = 1;
					KEY_DOWN++;
				if(animation_ongoing==1 && (destroy_block[(int)bombermen.x/10 + 2][(int)bombermen.z/10 + 1] != 1 &&
					destroy_block[(int)bombermen.x/10 + 2][(int)bombermen.z/10 + 1] != 2) && (int)bombermen.x % 10 == 0 && (int)bombermen.z % 10 ==0){
					bombermen.new_smer = 1.0;
                    ID = 4;
				}
				break;
		case 32:
		//SPACE = postavljanje bombe
				if(animation_ongoing==1 && bomba.ind == 0){
					bomba.ind = 1;
					bomba.x = round(bombermen.x/10);
					bomba.z = round(bombermen.z/10);
					destroy_block[bomba.x+1][bomba.z+1] = 1;
					}
				break;
		
			
	}
}

void on_keyboardUp(unsigned char key, int x, int y){
	switch(key){
		////////////    KONTROLE    //////////////////	
		case 'w':
			KEY_DOWN_W = 0;
			KEY_DOWN--;
			break;
		case 's':
			KEY_DOWN_S = 0;
			KEY_DOWN--;
			break;
		case 'a':
			KEY_DOWN_A = 0;
			KEY_DOWN--;
			break;
		case 'd':
			KEY_DOWN_D = 0;
			KEY_DOWN--;
			break;
	}
}

void on_reshape(int width, int height){
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(30, (float) width/height, 1, 100);
}

//tajmer za animaciju bombermena
void timer(int id){
	id = ID;
	if(id != 0){
		if(move_par == 45)
			indikator = -1;
		else if(move_par == -45)
			indikator = 1;
		if(indikator == 1)
			move_par+=5;
		else if(indikator == -1)
			move_par-=5;
			
		if(bombermen.old_smer != bombermen.new_smer){
			if(bombermen.old_smer < bombermen.new_smer){
				if(bombermen.new_smer - bombermen.old_smer <= 0.2){
				bombermen.old_smer = bombermen.new_smer;
			} else
				bombermen.old_smer += 0.3;
			}
			else {
				if(bombermen.old_smer - bombermen.new_smer <= 0.2){
					bombermen.old_smer = bombermen.new_smer;
				}
				else
					bombermen.old_smer -= 0.3;
			}
		}
	}

	if(id == 1){
		bombermen.z--;
		if((int)bombermen.z%10 == 0)
		{
			if(KEY_DOWN_W == 0 || destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10] == 1 || 
					destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10] == 2){
				ID = 0;
			}
		}
	}
	if(id == 2){
		bombermen.z++;
		if((int)bombermen.z%10 == 0){
			if(KEY_DOWN_S == 0 || destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10 + 2] == 1 ||
						destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10 + 2] == 2){
					ID = 0;
			}
		}
	}
	if(id == 3){
		bombermen.x--;
		if((int)bombermen.x%10 == 0){
			if(KEY_DOWN_A == 0 || destroy_block[(int)bombermen.x/10][(int)bombermen.z/10 + 1] == 1 ||
						destroy_block[(int)bombermen.x/10][(int)bombermen.z/10 + 1] == 2){
					ID = 0;
			}
		}
	}
	if(id == 4){
		bombermen.x++;
		if((int)bombermen.x%10 == 0){
			if(KEY_DOWN_D == 0 || destroy_block[(int)bombermen.x/10 + 2][(int)bombermen.z/10 + 1] == 1 || 
						destroy_block[(int)bombermen.x/10 + 2][(int)bombermen.z/10 + 1] == 2){
					ID = 0;
			}
		}
	}
	if(ID == 0){
		if(KEY_DOWN_W && destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10] != 1 && 
					destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10] != 2){
						ID = 1;
						bombermen.new_smer = 2.0;
					}
						
		else if(KEY_DOWN_S && destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10 + 2] != 1 &&
						destroy_block[(int)bombermen.x/10 + 1][(int)bombermen.z/10 + 2] != 2){
						ID = 2;
						bombermen.new_smer = 0.0;
					}
						
		else if(KEY_DOWN_A && destroy_block[(int)bombermen.x/10][(int)bombermen.z/10 + 1] != 1 &&
						destroy_block[(int)bombermen.x/10][(int)bombermen.z/10 + 1] != 2){
						ID = 3;
						bombermen.new_smer = -1.0;
					}
						
		else if(KEY_DOWN_D && destroy_block[(int)bombermen.x/10 + 2][(int)bombermen.z/10 + 1] != 1 && 
						destroy_block[(int)bombermen.x/10 + 2][(int)bombermen.z/10 + 1] != 2){
						ID = 4;
						bombermen.new_smer = 1.0;
					}
	}
////////////////////////////////////////////////////////////////////	
	if(bomba.ind == 1){
		bomba.bomb_pump++;
	}
////////////////////////////////////////////////////////////////////
	if(spider1.p == 0 && spider2.p == 0)
	{
		game_over = 1;
		animation_ongoing = 0;
	}
	
	spider_brojac++;
	spider_param = PI*sin(spider_brojac);
	
	//pauk 1
	if(spider1.p){
		spider_walk(&spider1);
		if(abs(spider1.x - bombermen.x) <= 5 && abs(spider1.z - bombermen.z) <= 5)
		{
			game_over = 1;
			animation_ongoing = 0;
			printf("Ubio te pauk 1 %f %f\n %f %f",spider1.x,spider1.z,bombermen.x,bombermen.z);
		}
	}
	//pauk 2
	if(spider2.p){
		spider_walk(&spider2);
		if(abs(spider2.x - bombermen.x) <= 5 && abs(spider2.z - bombermen.z) <= 5)
		{
			game_over = 1;
			animation_ongoing = 0;
			printf("Ubio te pauk 2 %f %f\n %f %f",spider2.x,spider2.z,bombermen.x,bombermen.z);
		}
	}
	
	//printf("%d \t %d \t %d \t %d \t %d\n",KEY_DOWN_W,KEY_DOWN_S,KEY_DOWN_A,KEY_DOWN_D,ID);
	
	if(!game_over && animation_ongoing)
	glutTimerFunc(TIMER_INTERVAL,timer,id); 
	glutPostRedisplay();
}


//funkcija kojom pauci nausmicno odredjuju putanju kretanja
//i provera da li je pauk pojeo bombermena
void spider_walk(Spider* spider){
	
	//SKINITE KOMENTAR DA BI VIDELI KAKO SE MATRICA MENJA
			/*int it,jt;
			 for(it=0;it<13;it++){
				for(jt=0;jt<17;jt++){
					printf("%d ",destroy_block[jt][it]);
				}
				printf("\n");
			}
			printf("\n");*/
	if(spider->smer == 0){
		if((int)spider->z%10 == 0){
			int num = rand()%3;
			if(num == 0 && (destroy_block[(int)spider->x/10 + 1][(int)spider->z/10] != 1 &&
							destroy_block[(int)spider->x/10 + 1][(int)spider->z/10] != 2)){
				spider->z -= 1;
			}
			else if(num == 1 && (destroy_block[(int)spider->x/10][(int)spider->z/10 + 1] != 1 &&
								destroy_block[(int)spider->x/10][(int)spider->z/10 + 1] != 2)){
				spider->smer = 2;
			}
			else if(num == 2 && (destroy_block[(int)spider->x/10 + 2][(int)spider->z/10 + 1] != 1 &&
								destroy_block[(int)spider->x/10 + 2][(int)spider->z/10 + 1] != 2)){
				spider->smer = 3;
			}
			else{
				spider->smer = 1;
			}
		}
		else {
			spider->z -= 1;
			if((int)spider->z%5 == 0 && (int)spider->z%10 != 0){
				destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 +1] = spider->id;
				destroy_block[(int)spider->x/10 + 1][(int)(spider->z+5)/10 +1] = 0;
			}
		}
	}
	if(spider->smer == 1){
		if((int)spider->z%10 == 0){
			int num = rand()%3;
			if(num == 0 && (destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 2] != 1 && 
							destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 2] != 2)){
				spider->z += 1;
			}
			else if(num == 1 && (destroy_block[(int)spider->x/10][(int)spider->z/10 + 1] != 1 &&
								destroy_block[(int)spider->x/10][(int)spider->z/10 + 1] != 2)){
				spider->smer = 2;
			}
			else if(num == 2 && (destroy_block[(int)spider->x/10 + 2][(int)spider->z/10 + 1] != 1 &&
								destroy_block[(int)spider->x/10 + 2][(int)spider->z/10 + 1] != 2)){
				spider->smer = 3;
			}
			else{
				spider->smer = 0;
			}
		}
		else {
			spider->z += 1;
			if((int)spider->z%5 == 0 && (int)spider->z%10 != 0){
				destroy_block[(int)spider->x/10 + 1][(int)(spider->z+5)/10 + 1] = spider->id;
				destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 1] = 0;
			}
		}
	}
	if(spider->smer == 2){
		if((int)spider->x%10 == 0){
			int num = rand()%3;
			if(num == 0 && (destroy_block[(int)spider->x/10][(int)spider->z/10 + 1] != 1 &&
							destroy_block[(int)spider->x/10][(int)spider->z/10 + 1] != 2)){
				spider->x -= 1;
			}
			else if(num == 1 && (destroy_block[(int)spider->x/10 + 1][(int)spider->z/10] != 1 &&
								destroy_block[(int)spider->x/10 + 1][(int)spider->z/10] != 2)){
				spider->smer = 0;
			}
			else if(num == 2 && (destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 2] != 1 &&
								destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 2] != 2)){
				spider->smer = 1;
			}
			else{
				spider->smer = 3;
			}
		}
		else {
			spider->x -= 1;
			if((int)spider->x%5 == 0 && (int)spider->x%10 != 0){
				destroy_block[(int)spider->x/10 +1][(int)spider->z/10 + 1] = spider->id;
				destroy_block[(int)(spider->x+5)/10 +1][(int)spider->z/10 + 1] = 0;
			}
		}
	}
	if(spider->smer == 3){
		if((int)spider->x%10 == 0){
			int num = rand()%3;
			if(num == 0 && (destroy_block[(int)spider->x/10 + 2][(int)spider->z/10 + 1] != 1 &&
							destroy_block[(int)spider->x/10 + 2][(int)spider->z/10 + 1] != 2)){
				spider->x += 1;
			}
			else if(num == 1 && (destroy_block[(int)spider->x/10 + 1][(int)spider->z/10] != 1 &&
								destroy_block[(int)spider->x/10 + 1][(int)spider->z/10] != 2)){
				spider->smer = 0;
			}
			else if(num == 2 && (destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 2] != 1 &&
								destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 2] != 2)){
				spider->smer = 1;
			}
			else{
				spider->smer = 2;
			}
		}
		else {
			spider->x += 1;
			if((int)spider->x%5 == 0 && (int)spider->x%10 != 0){
				destroy_block[(int)(spider->x+5)/10 + 1][(int)spider->z/10 + 1] = spider->id;
			destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 1] = 0;
			}
		}
	}
}

//f-ja za crtanje eksplozije
void draw_explosion(int x, int y){
	glPushMatrix();
		glTranslatef(x,0.5,y);
		glColor3f(1,.5,0);
		glutSolidCube((bomba.bomb_pump-100)/30);
	glPopMatrix();
	}

//f-ja za crtanje ekplozije i unistavanje zidova/bombermena/pauka
void explosion(int x, int y){
	draw_explosion(x,y);
	if(abs(x * 10 - bombermen.x) <= 5 && abs(y * 10 - bombermen.z) <= 5)
		{
			game_over = 1;
			animation_ongoing = 0;
			printf("Ubila te bomba %d %d \t %f %f \n",x,y,bombermen.x,bombermen.z);
		}
	destroy_block[x+1][y+1] = 0;
	if(destroy_block[x+2][y+1] != 1){
		draw_explosion(x+1,y);
		if(abs((x+1) * 10 - bombermen.x) <= 5 && abs(y * 10 - bombermen.z) <= 5)
		{
			game_over = 1;
			animation_ongoing = 0;
			printf("Ubila te bomba %d %d \t %f %f \n",x+1,y,bombermen.x,bombermen.z);
		}
		if(abs((x+1) * 10 - spider1.x) <= 5 && abs(y * 10 - spider1.z) <= 5)
		{
			spider1.p = 0;
		}
		if(abs((x+1) * 10 - spider2.x) <= 5 && abs(y * 10 - spider2.z) <= 5)
		{
			spider2.p = 0;
		}
		destroy_block[x+2][y+1] = 0;
	}
	if(destroy_block[x][y+1] != 1){
		draw_explosion(x-1,y);
		if(abs((x-1) * 10 - bombermen.x) <= 5 && abs(y * 10 - bombermen.z) <= 5)
		{
			game_over = 1;
			animation_ongoing = 0;
			printf("Ubila te bomba %d %d \t %f %f \n",x-1,y,bombermen.x,bombermen.z);
		}
		if(abs((x-1) * 10 - spider1.x) <= 5 && abs(y * 10 - spider1.z) <= 5)
		{
			spider1.p = 0;
		}
		if(abs((x-1) * 10 - spider2.x) <= 5 && abs(y * 10 - spider2.z) <= 5)
		{
			spider2.p = 0;
		}
		destroy_block[x][y+1] = 0;
	}
	if(destroy_block[x+1][y+2] != 1){
		draw_explosion(x,y+1);
		if(abs(x * 10 - bombermen.x) <= 5 && abs((y+1) * 10 - bombermen.z) <= 5)
		{
			game_over = 1;
			animation_ongoing = 0;
			printf("Ubila te bomba %d %d \t %f %f \n",x,y+1,bombermen.x,bombermen.z);
		}
		if(abs(x * 10 - spider1.x) <= 5 && abs((y+1) * 10 - spider1.z) <= 5)
		{
			spider1.p = 0;
		}
		if(abs(x * 10 - spider2.x) <= 5 && abs((y+1) * 10 - spider2.z) <= 5)
		{
			spider2.p = 0;
		}
		destroy_block[x+1][y+2] = 0;
	}
	if(destroy_block[x+1][y] != 1){
		draw_explosion(x,y-1);
		if(abs(x * 10 - bombermen.x) <= 5 && abs((y-1) * 10 - bombermen.z) <= 5)
		{
			game_over = 1;
			animation_ongoing = 0;
			printf("Ubila te bomba %d %d \t %f %f \n",x,y-1,bombermen.x,bombermen.z);
		}
		if(abs(x * 10 - spider1.x) <= 5 && abs((y-1) * 10 - spider1.z) <= 5)
		{
			spider1.p = 0;
		}
		if(abs(x * 10 - spider2.x) <= 5 && abs((y-1) * 10 - spider2.z) <= 5)
		{
			spider2.p = 0;
		}
		destroy_block[x+1][y] = 0;
	}
}

//f-ja za crtanje bombe
void draw_bomb(){
	glPushMatrix();
		if(bomba.bomb_pump > 100){
			explosion(bomba.x,bomba.z);
			if(bomba.bomb_pump >= 130){
				bomba.bomb_pump = 0;
				bomba.ind = 0;
				bomba.x = -1;
				bomba.z = -1;
			}
		}
		else {
			glTranslatef(bomba.x,0.5,bomba.z);
			glColor3f(.7,0,0);
			glutSolidSphere(0.4+cos((bomba.bomb_pump)/5)*0.05,20,20);
		}
	glPopMatrix();
}

//f-ja za crtanje pauka
void draw_spider(float i, float j){
	glTranslatef(i,0,j);
	glScalef(.67,.7,.7);
	glPushMatrix();
		glTranslatef(0,.5,0);
		glScalef(1,.5,1);
		glutSolidSphere(0.5,20,20);
		//gornja desna
		glPushMatrix();
			glRotatef(45+spider_param,0,1,0);
			glTranslatef(.8,-.1,0);
			glScalef(8,1,1);	
			glutSolidCube(.1);
		glPopMatrix();
		//desna
		glPushMatrix();
			glRotatef(spider_param,0,1,0);
			glTranslatef(.8,-.1,0);
			glScalef(8,1,1);	
			glutSolidCube(.1);
		glPopMatrix();
		//donja desna
		glPushMatrix();
			glRotatef(-45-spider_param,0,1,0);
			glTranslatef(.8,-.1,0);
			glScalef(8,1,1);	
			glutSolidCube(.1);
		glPopMatrix();
		//gornja leva
		glPushMatrix();
			glRotatef(-45-spider_param,0,1,0);
			glTranslatef(-.8,-.1,0);
			glScalef(8,1,1);	
			glutSolidCube(.1);
		glPopMatrix();
		//leva
		glPushMatrix();
			glRotatef(-spider_param,0,1,0);
			glTranslatef(-.8,-.1,0);
			glScalef(8,1,1);	
			glutSolidCube(.1);
		glPopMatrix();
		//donja leva
		glPushMatrix();
			glRotatef(45+spider_param,0,1,0);
			glTranslatef(-.8,-.1,0);
			glScalef(8,1,1);	
			glutSolidCube(.1);
		glPopMatrix();
	glPopMatrix();
}

//f-ja za crtanje platforme i zidova
void draw_platform(){
	glPushMatrix();
		glTranslatef(-1, -0.5, -1);
		glPushMatrix();
			glColor3f(.1, .3, .1);
			glTranslatef(8,.2,5.5);
			glScalef(17,.5,12);
			glutSolidCube(1);
		glPopMatrix();
		int i;
		for(i=0; i<13; i++){
			int j;
			for(j=0; j<17; j++){
				if(i == 0 || i == 12 || j==0 || j==16 || (j%2==0 && i%2==0)){
					glPushMatrix();
						glScalef(1,.7,1);
						glTranslatef(0,1,0);
						glColor3f(.2, 0.2, 0.2);
						glutSolidCube(1);
					glPopMatrix();
				}
				else {
					glColor3f(.1, .5, .1);
					glutSolidCube(.97);
					if(destroy_block[j][i] == 2)
					{
						glPushMatrix();
							glScalef(.9,.6,.9);
							glTranslatef(0,1,0);
							glColor3f(1, 1, 0);
							glutSolidCube(1);
						glPopMatrix();
					}
				}
				glTranslatef(1,0,0);
			}
			glTranslatef(-17,0,1);
		}
		
	glPopMatrix();
}

//f-ja za crtanje ruku i nogu bombermena
void draw_ud(){
	glPushMatrix();
		glScalef(1,3,1);
		glutSolidCube(.35);
	glPopMatrix();
}
//f-ja za crtanje tela bombermena
void draw_torso(){
	glPushMatrix();
		glScalef(1,1.3,0.7);
		glutSolidCube(0.85);
	glPopMatrix();
}
//f-ja za crtanje glave bombermena
void draw_head(){
	glPushMatrix();
		glutSolidSphere(.4,20,20);
	glPopMatrix();
}

void draw_bomberman(){
	glTranslatef(bombermen.x/10,0,bombermen.z/10);
	glScalef(.67,.7,.7);
	glRotatef(90*bombermen.old_smer,0,1,0);
	glPushMatrix();
	//leva noga
		glPushMatrix();
			glTranslatef(.25,.5,0);
			glTranslatef(0, 0.4, 0);
			glRotatef(move_par,1,0,0);
			glTranslatef(0,-0.4,0);
			draw_ud();
		glPopMatrix();
	//desna noga
		glPushMatrix();
			glTranslatef(-.25,.5,0);
			glTranslatef(0, 0.4, 0);
			glRotatef(-move_par,1,0,0);
			glTranslatef(0,-0.4,0);
			draw_ud();
		glPopMatrix();
	//telo
		glPushMatrix();
			glTranslatef(0,1.6,0);
			draw_torso();
		glPopMatrix();
	//leva ruka
		glPushMatrix();
			glTranslatef(.62,1.62,0);
			glTranslatef(0, 0.4, 0);
			glRotatef(-move_par,1,0,0);
			glTranslatef(0,-0.4,0);
			draw_ud();
		glPopMatrix();
	//desna ruka
		glPushMatrix();
			glTranslatef(-.62,1.62,0);
			glTranslatef(0, 0.4, 0);
			glRotatef(move_par,1,0,0);
			glTranslatef(0,-0.4,0);
			draw_ud();
		glPopMatrix();
	//glava
		glPushMatrix();
			glTranslatef(0,2.5,0);
			draw_head();
		glPopMatrix();
	glPopMatrix();
}

void on_display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(7, 25, 15,
			  7, 0, 5,
			  0, 1, 0);
	//draw_axes(5);

	glPushMatrix();
		glColor3f(0, 0.3, 0.4);
		draw_platform();
	glPopMatrix();
	
	glPushMatrix();
		glColor3f(0.7, 0, 0.4);
		draw_bomberman();
	glPopMatrix();
	
	if(bomba.ind == 1){
		glPushMatrix();
			glColor3f(0.7, 0, 0);
			draw_bomb();
		glPopMatrix();
	}
	
	glPushMatrix();
		glColor3f(0,.1,1);
		if(spider1.p){
			glPushMatrix();
				draw_spider(spider1.x/10,spider1.z/10);
			glPopMatrix();
		}
		if(spider2.p){
			glPushMatrix();
				draw_spider(spider2.x/10,spider2.z/10);
			glPopMatrix();
		}
	glPopMatrix();
	
	glutSwapBuffers();
	
}

