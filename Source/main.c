#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/image.h"


#define TIMER_INTERVAL 25
#define PI 3.1415926535897
#define FILENAME0 "../Textures/metal_pod.bmp"
#define FILENAME1 "../Textures/crate2.bmp"
#define FILENAME2 "../Textures/brick.bmp"

static GLuint names[3];
static void initialize_texture(void);
static void apply_texture(double edge,int texture);
static void draw_score();
static void draw_game_over(char* word, int pob);
float gametime = 300;

//struktura za paukove
typedef struct{
	float x;
	float z;
	int smer;
	int p;
}Spider;
//struktura za bombermena
typedef struct{
	float x;
	float z;
	float old_smer;
	float new_smer;
}Bomberman;
//struktura za pauka
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

	glutInitWindowSize(800,600);
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
    //gametime = 300;
    randomize_walls();
    initialize_texture();
	
	
	glClearColor(0.07, 0.14, 0.20, 0);
	glutMainLoop();
	
	return 0;
}

static void initialize_texture(void){
    /* Objekat koji predstavlja teskturu ucitanu iz fajla. */
    Image * image;

    /* Postavlja se boja pozadine. */
    //glClearColor(0, 0, 0, 0);

    /* Ukljucuje se testiranje z-koordinate piksela. */
    //glEnable(GL_DEPTH_TEST);

    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);

    /* Kreira se prva tekstura. */
    image_read(image, FILENAME0);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(3, names);

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
                 
    /* Kreira se druga tekstura. */
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
                 
    /* Kreira se treca tekstura. */
    image_read(image, FILENAME2);

    glBindTexture(GL_TEXTURE_2D, names[2]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);

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
			glDeleteTextures(3, names);
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
			gametime = 300.0;
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

static void on_reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 100);
    glutFullScreen();
}

//tajmer za animaciju bombermena
void timer(int id){
	id = ID;
	
	gametime-=0.025;
	if(gametime <= 0)
	{
		game_over = 3;
		animation_ongoing = 0;
	}
	else if(spider1.p == 0 && spider2.p == 0)
	{
		game_over = 1;
		animation_ongoing = 0;
		//pobeda
	}
	
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

	spider_brojac++;
	spider_param = PI*sin(spider_brojac);
	
	//pauk 1
	if(spider1.p){
		spider_walk(&spider1);
		if(abs(spider1.x - bombermen.x) <= 5 && abs(spider1.z - bombermen.z) <= 5)
		{
			game_over = 2;
			animation_ongoing = 0;
		}
	}
	//pauk 2
	if(spider2.p){
		spider_walk(&spider2);
		if(abs(spider2.x - bombermen.x) <= 5 && abs(spider2.z - bombermen.z) <= 5)
		{
			game_over = 2;
			animation_ongoing = 0;
		}
	}
	
	
	
	if(animation_ongoing)
	glutTimerFunc(TIMER_INTERVAL,timer,id); 
	glutPostRedisplay();
}


//funkcija kojom pauci nausmicno odredjuju putanju kretanja
//i provera da li je pauk pojeo bombermena
void spider_walk(Spider* spider){
	
	
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
			/*if((int)spider->z%5 == 0 && (int)spider->z%10 != 0){
				destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 +1] = spider->id;
				destroy_block[(int)spider->x/10 + 1][(int)(spider->z+5)/10 +1] = 0;
			}*/
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
			/*if((int)spider->z%5 == 0 && (int)spider->z%10 != 0){
				destroy_block[(int)spider->x/10 + 1][(int)(spider->z+5)/10 + 1] = spider->id;
				destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 1] = 0;
			}*/
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
			/*if((int)spider->x%5 == 0 && (int)spider->x%10 != 0){
				destroy_block[(int)spider->x/10 +1][(int)spider->z/10 + 1] = spider->id;
				destroy_block[(int)(spider->x+5)/10 +1][(int)spider->z/10 + 1] = 0;
			}*/
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
			/*if((int)spider->x%5 == 0 && (int)spider->x%10 != 0){
				destroy_block[(int)(spider->x+5)/10 + 1][(int)spider->z/10 + 1] = spider->id;
			destroy_block[(int)spider->x/10 + 1][(int)spider->z/10 + 1] = 0;
			}*/
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
			game_over = 4;
			animation_ongoing = 0;
		}
	destroy_block[x+1][y+1] = 0;
	if(destroy_block[x+2][y+1] != 1){
		draw_explosion(x+1,y);
		if(abs((x+1) * 10 - bombermen.x) <= 5 && abs(y * 10 - bombermen.z) <= 5)
		{
			game_over = 4;
			animation_ongoing = 0;
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
			game_over = 4;
			animation_ongoing = 0;
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
			game_over = 4;
			animation_ongoing = 0;
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
			game_over = 4;
			animation_ongoing = 0;
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
						apply_texture(1.0,names[2]);
					glPopMatrix();
				}
				else {
					glColor3f(.1, .5, .1);
					glutSolidCube(.97);
					if(destroy_block[j][i] == 2)
					{
						glPushMatrix();
							glScalef(.9,.6,.9);
							glTranslatef(0,1.5,0);
							apply_texture(1.0,names[1]);
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

static void draw_score(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
		gluLookAt(0, 0, 1, 
					0, 0, 0
					, 0, 1, 0);
		char* word = malloc(30);
		sprintf(word,"Score: %d", (int)gametime);
		const int x = -500;
		const int y = 800;
		const int z = 0;
		glPushMatrix();
			glScalef(0.06,0.06,5);
			glPushAttrib(GL_LINE_BIT);
			  glLineWidth(4); //Postavljamo debljinu linije
				int len; //duzina stringa
				glDisable(GL_LIGHTING); //Privremeno iskljucujemo osvetljenje da bi postavili boju teksta
				glColor3f(1,0,0); //Postavljanje boje teksta
				//glRasterPos3f(x,y,z);
				//Postavljamo dimenzije slova
				//glRotatef(-30,1,0,0);
				glScalef(0.01,0.01,5);
				glTranslatef(x,y,z);
				len = strlen(word);
				for (int i = 0; i < len; i++)
				{
					glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, word[i]);
				}
				glEnable(GL_LIGHTING); //Ponovo ukljucujemo osvetljenje
			glPopAttrib();
		glPopMatrix();
		free(word);
	//glPopMatrix();
}

static void draw_game_over(char* word, int pob){   
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 1, 
                    0, 0, 0
                    , 0, 1, 0);
    //Koordinate za ispis teksta
    int x = -500;
    int y = 0;
    int z = 0;
    /*Deo za ispisivanje "game over"*/
    char* over_game = malloc(30);
    if(pob == 0)
		sprintf(over_game,"Game over!");
	else sprintf(over_game,"Victory!");
    glPushMatrix();
        glScalef(0.06,0.06,5);
        glPushAttrib(GL_LINE_BIT);
            glLineWidth(4); //Postavljamo debljinu linije
            int len; //duzina stringa
			glDisable(GL_LIGHTING); //Privremeno iskljucujemo osvetljenje da bi postavili boju teksta
			glColor3f(1,0,0); //Postavljanje boje teksta
			glScalef(0.01,0.01,5);
			glTranslatef(x,y,z);
			len = strlen(over_game);
			for (int i = 0; i < len; i++)
			{
				glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, over_game[i]);
			}
			free(over_game);
			glEnable(GL_LIGHTING); //Ponovo ukljucujemo osvetljenje
        glPopAttrib();
    glPopMatrix();
    /*Deo za ispisivanje skora*/
    glPushMatrix();
        glScalef(0.06,0.06,5);
        glPushAttrib(GL_LINE_BIT);
            glLineWidth(4); //Postavljamo debljinu linije
            if(game_over == 2){
				x-=550;
			}
			else if(game_over == 1){
				x-=200;
			}
			else if(game_over == 3){
				x-=0;
			}
			else x-=300;
            y-=200;
			glDisable(GL_LIGHTING); //Privremeno iskljucujemo osvetljenje da bi postavili boju teksta
			glColor3f(1,0,0); //Postavljanje boje teksta
			glScalef(0.006,0.006,5);
			glTranslatef(x,y,z);
			len = strlen(word);
			for (int i = 0; i < len; i++)
			{
				glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, word[i]);
			}
			glEnable(GL_LIGHTING); //Ponovo ukljucujemo osvetljenje
        glPopAttrib();
    glPopMatrix();
}

static void apply_texture(double edge,int texture){
    int coef_of_mapping=1;
    double offset=0.01; //koristimo kao nacin uklanjanje greske pri racunanju sa brojevima u pokretnom zarezu
    glBindTexture(GL_TEXTURE_2D, texture);
    glPushMatrix(); //prednja strana

            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex3f(-edge/2, -edge/2, edge/2);
                glTexCoord2f(0, edge);
                glVertex3f(-edge/2, edge/2, edge/2);
                glTexCoord2f(edge*coef_of_mapping , edge);
                glVertex3f(edge/2, edge/2, edge/2);
                glTexCoord2f(edge*coef_of_mapping, 0);
                glVertex3f(edge/2, -edge/2, edge/2);
            glEnd();
    glPopMatrix();

     //zadnja strana
    glPushMatrix();

            glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(-edge/2, -edge/2, -edge/2);
                glTexCoord2f(0, edge);
                glVertex3f(-edge/2, edge/2, -edge/2);
                glTexCoord2f(edge*coef_of_mapping, edge);
                glVertex3f(edge/2, edge/2, -edge/2);
                glTexCoord2f(edge*coef_of_mapping, 0);
                glVertex3f(edge/2, -edge/2, -edge/2);
            glEnd();

    glPopMatrix();



    glPushMatrix(); //gornja strana

            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex3f(-edge/2, edge/2,edge/2);
                glTexCoord2f(0, edge);
                glVertex3f(-edge/2, edge/2,-edge/2);
                glTexCoord2f(edge*coef_of_mapping, edge);
                glVertex3f(edge/2, edge/2,-edge/2);
                glTexCoord2f(edge*coef_of_mapping, 0);
                glVertex3f(edge/2, edge/2,edge/2);
            glEnd();


    glPopMatrix();

    glPushMatrix(); //leva strana

            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex3f(-edge/2-offset, -edge/2,edge/2);
                glTexCoord2f(0, edge);
                glVertex3f(-edge/2-offset, edge/2,edge/2);
                glTexCoord2f(edge*coef_of_mapping, edge);
                glVertex3f(-edge/2-offset, edge/2,-edge/2);
                glTexCoord2f(edge*coef_of_mapping, 0);
                glVertex3f(-edge/2-offset, -edge/2,-edge/2);
            glEnd();


    glPopMatrix();
    glPushMatrix(); //desna strana

            glBegin(GL_QUADS);
                glTexCoord2f(0 , 0);
                glVertex3f(edge/2+offset, -edge/2,edge/2);
                glTexCoord2f(0, edge);
                glVertex3f(edge/2+offset, edge/2,edge/2);
                glTexCoord2f(edge*coef_of_mapping , edge);
                glVertex3f(edge/2+offset, edge/2,-edge/2);
                glTexCoord2f(edge*coef_of_mapping, 0);
                glVertex3f(edge/2+offset, -edge/2,-edge/2);
            glEnd();


    glPopMatrix();
   
    glBindTexture(GL_TEXTURE_2D, 0);
}

void on_display(){
	
	if(game_over!=0){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		animation_ongoing = 0;
		switch(game_over){
			case 1:{
				char* word = malloc(20);
				sprintf(word,"Score: %d",(int)gametime);
				draw_game_over(word,1);
				free(word);}
				break;
			case 2:
				draw_game_over("Spider has eaten you",0);
				break;
			case 3:
				draw_game_over("Timed out",0);
				break;
			case 4:
				draw_game_over("Bomb blew you up",0);
				break;
				
		}
	}
	else{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	draw_score();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(7, 15, 10,
			  7, 0, 5,
			  0, 1, 0);
	//draw_axes(5);
	
	///////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, names[0]);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(-1, 0, -1);

        glTexCoord2f(6, 0);
        glVertex3f(15, 0, -1);

        glTexCoord2f(6, 6);
        glVertex3f(15, 0, 11);

        glTexCoord2f(0, 6);
        glVertex3f(-1, 0, 11);
    glEnd();
    
    

    glBindTexture(GL_TEXTURE_2D, 0);
	////////////////////////////////////////////
	

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
		glColor3f(.95,.95,.1);
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
	
	
	
	}
	
	glutSwapBuffers();
}





















