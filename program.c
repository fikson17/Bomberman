#include<GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>

#define TIMER_INTERVAL 50
#define TIMER_ID 0

#define PI 3.1415926535897

static void on_display();
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_timer(int id);

float animation_parameterld = 0;
float animation_parametergd = 0;
float animation_parameternz = 3.0;
float animation_parameter = 0;
float indikator = 1.0;

float animation_ongoing = 0;


float q=0;
float z=0;

/*static void draw_bomberman();
void draw_ud();
void draw_torso();
void draw_head();
void draw_platform();

void draw_platform();*/

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(1200,800);
	glutInitWindowPosition(400,100);
	glutCreateWindow(argv[0]);

	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutKeyboardFunc(on_keyboard);
	
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

	glClearColor(0.7, 0.7, 0.7, 0);
	glutMainLoop();
	
	return 0;
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
			exit(0);
			break;
		case 'g':
			if(animation_ongoing==0){
				animation_ongoing=1;
				glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
			}
			break;
		case 'f':
			animation_ongoing=0;
			break;
		case 'r':
			//animation_parameterld = 0;
			//animation_parameternz = 3;
			animation_parameter = 0;
			glutPostRedisplay();
			break;
		case 'l':
			animation_parameterld+=0.06;
			glutPostRedisplay();
			break;
		case 'j':
			animation_parameterld-=0.06;
			glutPostRedisplay();
			break;
		case 'k':
			animation_parametergd-=0.06;
			glutPostRedisplay();
			break;
		case 'i':
			animation_parametergd+=0.06;
			glutPostRedisplay();
			break;
		case 'n':
			animation_parameternz+=0.2;
			glutPostRedisplay();
			break;
		case 'm':
			animation_parameternz-=0.2;
			glutPostRedisplay();
			break;
		////////////    KONTROLE    //////////////////	
		case 'w':
			if(z>0){
				z--;
				glutPostRedisplay();
				}
			break;
		case 's':
			if(z<10){
				z++;
				glutPostRedisplay();
				}
			break;
		case 'a':
			if(q>0){
				x--;
				glutPostRedisplay();
				}
			break;
		case 'd':
			if(q<14){
				x++;
				glutPostRedisplay();
				}
			break;
			
		
			
	}
}

void on_reshape(int width, int height){
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(30, (float) width/height, 1, 100);
}

void on_timer(int id){
	if(id == TIMER_ID){
		if(indikator == 1.0)
			animation_parameter+=1;
		else animation_parameter-=1;
	if(animation_parameter>360)
		animation_parameter-=360;
	}
	if(animation_ongoing==1)
	glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID); 
	glutPostRedisplay();
}

void draw_platform(){
	glPushMatrix();
		glTranslatef(-1, -0.5, -1);
		int i;
		for(i=0; i<12; i++){
			int j;
			for(j=0; j<17; j++){
				if(i == 0 || j==0 || j==16 || (j%2==0 && i%2==0)){
					glPushMatrix();
						glScalef(1,2,1);
						glTranslatef(0,.4,0);
						glutSolidCube(1);
					glPopMatrix();
				}
				else glutSolidCube(.9);
				glTranslatef(1,0,0);
			}
			glTranslatef(-17,0,1);
		}
		
	glPopMatrix();
}

void draw_ud(){
	glPushMatrix();
		glScalef(1,3,1);
		glutSolidCube(.35);
	glPopMatrix();
}
void draw_torso(){
	glPushMatrix();
		glScalef(1,1.3,0.7);
		glutSolidCube(0.85);
	glPopMatrix();
}
void draw_head(){
	glPushMatrix();
		glutSolidSphere(.4,20,20);
	glPopMatrix();
}

static void draw_bomberman(){
	glTranslatef(q,0,z);
	glScalef(.67,.7,.7);
	glPushMatrix();
	//leva noga
		glPushMatrix();
			glTranslatef(.25,.5,0);
			draw_ud();
		glPopMatrix();
	//desna noga
		glPushMatrix();
			glTranslatef(-.25,.5,0);
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
			glRotatef(-animation_parameter,1,0,0);
			glTranslatef(0,-0.4,0);
			draw_ud();
		glPopMatrix();
	//desna ruka
		glPushMatrix();
			glTranslatef(-0.62,1.62,0);
			glTranslatef(0, 0.4, 0);
			float par = -animation_parameter;
			if((animation_parameter>45 && animation_parameter<=135) ||
				(animation_parameter>225 && animation_parameter<=45)){
					indikator = 1;
			}
			else indikator = 0;
				glRotatef(par,1,0,0);
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
	
	/*gluLookAt(7, 25, 15,
			  7, 0, 5,
			  0, 1, 0);*/
	gluLookAt(animation_parameternz*PI*sin(animation_parameterld), 
				animation_parameternz*PI*sin(animation_parametergd), 
				animation_parameternz*PI*cos(animation_parameterld)*cos(animation_parametergd),
			  0, 1, 0,
			  0, 1, 0);
	draw_axes(5);
	
	glPushMatrix();
		glColor3f(0, 0.3, 0.4);
		draw_platform();
	glPopMatrix();
	
	glPushMatrix();
		glColor3f(0.7, 0, 0.4);
		draw_bomberman();
	glPopMatrix();
	
	glutSwapBuffers();
	
}

































