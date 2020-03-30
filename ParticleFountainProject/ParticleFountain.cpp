// Ben Gardner
// 1146755
// This project was compiled in Visual Studio 2012

// Adjust int period to modify rate of particle generation

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <gl/gl.h>
#include <gl/freeglut.h>
#include "ParticleSystem.h"

//initialize the fountain
float origin[3] = {0, 0, 0};
ParticleSystem fountain = ParticleSystem(origin);
int particleCount;

//half length of particle base
float baseLength = 100.0;
//base colours
GLfloat base_material_f[] = {0.5f, 0.4f, 0.0f};
GLfloat base_material[] = {0.1f, 0.08f, 0.1f};

//system modifiers
int period = 100; //time per cycle
bool paused = false;

//view modifiers
int xLookAt = 100;
int yLookAt = 150;
int mouseButton = 0;
int lastX = 0; // last x, y position for cursor motion
int lastY = 0;

//particle modifiers
int shapeType = 1;
bool friction = true;
bool wind = false;
int windCount = 0;
float windSpeed = 1;

/* makeRandomParticle() - generates a random particle and returns it
 */
Particle * makeRandomParticle()
{
	vec3D position(0.0f, 0.0001f, 0.0f);
	vec3D direction((rand() % 101)/500.0f - 0.1f, 15, (rand() % 101)/500.0f - 0.1f);	// pick x and z direction from -0.1 to 0.1; y direction is 15
	float speed = rand() % 6 / 10.0f + 1;	// pick a speed from 1 to 1.5
	vec3D angle(0.0f, 0.0f, 0.0f);
	vec3D rotation((rand() % 101)/50.0f - 1, (rand() % 101)/50.0f - 1, (rand() % 101)/50.0f - 1); // pick angle of rotation from -1 to 1
	float size = (rand() % 101)/100.0f + 2;	// pick a size from 2 to 3
	vec3D colour((rand() % 101)/100.0f, (rand() % 101)/100.0f, (rand() % 101)/100.0f); // pick random RGB colours from 0-1
	int age = 0;
	int shape = shapeType;	// shapes: 1-cube, 2-sphere, 3-cone, 4-teapot
	Particle * p = new Particle(position, direction, speed, angle, rotation, size, colour, age, shape);
	return p;
}

/* moveParticles() - updates the fountain particles' positions to sync to the next frame
 */
void moveParticles()
{
	// set the amount of bounce depending on whether friction is enabled
	float bounciness;
	if (friction) {
		bounciness = 0.8f;
	}
	else {
		bounciness = 0.95f;
	}

	for (unsigned int i = 0; i < fountain.particles.size(); i++)
	{
		// rotate particle
		fountain.particles[i]->angle.add(fountain.particles[i]->rotation.scalarMultiply(fountain.particles[i]->speed));
		// translate particle
		// set y position to 0 if particle is too slow and close to the ground
		if (abs(fountain.particles[i]->direction.y) < 0.5 && abs(fountain.particles[i]->position.y) < 3 &&
			-baseLength < fountain.particles[i]->position.x && fountain.particles[i]->position.x < baseLength &&
			-baseLength < fountain.particles[i]->position.z && fountain.particles[i]->position.z < baseLength) {
			fountain.particles[i]->position.y = 0;
			fountain.particles[i]->direction.y = 0;
		}
		else {
			fountain.particles[i]->direction.y -= 9.8f * 0.1f;	// acceleration due to gravity
		}
		
		if (wind) {
			windCount++;
			// randomize the wind speed every 500 cycles
			if (windCount % 500 == 0) {
				windSpeed = rand() % 11 / 5.0f;
			}

			fountain.particles[i]->direction.x += 0.001f * windSpeed;
			fountain.particles[i]->direction.z += 0.001f * windSpeed;
		}

		vec3D distance = fountain.particles[i]->direction.scalarMultiply(fountain.particles[i]->speed);
		fountain.particles[i]->position.add(distance);	// movement of particle from initial speed + gravity

		if (fountain.particles[i]->position.y <= 0 &&
			-baseLength < fountain.particles[i]->position.x && fountain.particles[i]->position.x < baseLength &&
			-baseLength < fountain.particles[i]->position.z && fountain.particles[i]->position.z < baseLength) {
			fountain.particles[i]->direction.y *= -bounciness;	// change direction if particle hits the ground
			fountain.particles[i]->position.y = -fountain.particles[i]->position.y; // physics
		}
	}
}

/* updateParticles() - updates the fountain to the next frame
 *  draws new particles and applies physics
 */
void updateParticles()
{
	// iterate through particles in the fountain
	for (unsigned int i = 0; i < fountain.particles.size(); i++)
	{
		glPushMatrix();
		GLfloat ambient_material[] = {fountain.particles[i]->colour.x, fountain.particles[i]->colour.y, fountain.particles[i]->colour.z};
		GLfloat shininess[] = { (float) (rand() % 101) };	// pick a random shininess 0-100
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_material);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		// translate particle
		glTranslatef(fountain.particles[i]->position.x, fountain.particles[i]->position.y, fountain.particles[i]->position.z);
		// rotate particle
		glRotatef(fountain.particles[i]->angle.x, 1, 0, 0);
		glRotatef(fountain.particles[i]->angle.y, 0, 1, 0);
		glRotatef(fountain.particles[i]->angle.z, 0, 0, 1);
		// select particle shape
		if (fountain.particles[i]->shape == 1) {
			glutSolidCube(fountain.particles[i]->size);
		}
		else if (fountain.particles[i]->shape == 2) {
			glutSolidSphere(fountain.particles[i]->size/2, 8, 6);
		}
		else if (fountain.particles[i]->shape == 3) {
			glutSolidCone(fountain.particles[i]->size, fountain.particles[i]->size, 4, 4);
		}
		else if (fountain.particles[i]->shape == 4) {
			glutSolidTeapot(fountain.particles[i]->size);
		}
		// delete particles after 1500 cycles
		fountain.particles[i]->age++;
		if (fountain.particles[i]->age >= 1500) {
			fountain.removeParticle(i);
			i--;
		}
		glPopMatrix();
	}
}

/* drawBase() -- draws a square brown base at the origin of the coordinate system
 */
void drawBase()
{
	// change the colour if the surface is rubber or mud
	if (friction) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, base_material_f);
	}
	else {
		glMaterialfv(GL_FRONT, GL_AMBIENT, base_material);
	}
	glBegin(GL_QUADS);
		glVertex3f(baseLength,0,baseLength);
		glVertex3f(baseLength,0,-baseLength);
		glVertex3f(-baseLength,0,-baseLength);
		glVertex3f(-baseLength,0,baseLength);
	glEnd();
}

/* timer() - timer
 */
void timer(int id)
{
	glutPostRedisplay();
}

/* display() - the OpenGL display function, this draws the screen
 *  it displays a particle fountain
 */
void display()
{
	if (!paused) {
		//clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		drawBase();
		moveParticles();
		updateParticles();
	
		//swap buffers - rendering is done to the back buffer, bring it forward to display
		glutSwapBuffers();

		//update every x ms
		glutTimerFunc(period, timer, 0);
	}
}

/* keyboard -- the GLUT keyboard function 
 *  key -- the key pressed
 *  x and y - mouse x and y coordinates at the time the function is called
 */
void keyboard(unsigned char key, int x, int y)
{
	//if the "q" key is pressed, quit the program
	if(key == 'q' || key == 'Q')
	{
		exit(0);
	}

	//if "r" is pressed, the simulation will reset
	if(key == 'r' || key == 'R')
	{
		do
		{
			fountain.removeParticle(0);
		} while (fountain.particles.size() > 0);
	}

	//if spacebar is pressed, pause/unpause the simulation
	if( key == ' ' && paused == true)
	{
		paused = false;
	}
	else if(key == ' ' && paused == false)
	{
		paused = true;
	}

	//if "f" is pressed, friction is toggled
	if((key == 'f' || key == 'F') && friction == true)
	{
		friction = false;
	}
	else if((key == 'f' || key == 'F') && friction == false)
	{
		friction = true;
	}

	//if "w" is pressed, wind is toggled
	if((key == 'w' || key == 'W') && wind == true)
	{
		wind = false;
	}
	else if((key == 'w' || key == 'W') && wind == false)
	{
		wind = true;
	}
	
	//shape toggles
	if(key == '1' || key == '2' || key == '3' || key == '4')
	{
		const char tempChar = key;
		shapeType = atoi(&tempChar);
	}
}

/* specialKeyboard - contains callbacks for arrow keystrokes
 */
void specialKeyboard(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP:
		glRotatef(1, 1, 0, 0);
		break;
	case GLUT_KEY_DOWN:
		glRotatef(-1, 1, 0, 0);
		break;
	case GLUT_KEY_LEFT:
		glRotatef(1, 0, 1, 0);
		break;
	case GLUT_KEY_RIGHT:
		glRotatef(-1, 0, 1, 0);
		break;
	}
}
 /* mouse - GLUT mouse function
  *  btn - button pressed or raised
  *  state - up or down
  *  x, y - coords of cursor during event
  */
void mouse(int btn, int state, int x, int y)
{
	//reset last cursor location
	lastX = -1;
	lastY = -1;

	switch(btn)
	{
	case GLUT_LEFT_BUTTON:
		mouseButton = 0;
		break;
	case GLUT_RIGHT_BUTTON:
		mouseButton = 1;
		break;
	case 3:
		glTranslatef(0, 0, 0.1f);
		break;
	case 4:
		glTranslatef(0, 0, -0.1f);
		break;
	}
}

/* motion - GLUT motion function for mouse movement
 *  x, y - cursor location
 */
void motion(int x, int y)
{
	if (lastX == -1) { // set current cursor coordinates to last if none are set
		lastX = x;
		lastY = y;
	}

	switch(mouseButton)
	{
	case 0:	// rotate if left button is held
		glRotatef((x-lastX)/5.0f, 0.0f, 1.0f, 0.0f);
		glRotatef((y-lastY)/5.0f, 1.0f, 0.0f, 0.0f);
		break;
	case 1:	// translate if right button is held
		glTranslatef((x-lastX)/5.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, (lastY-y)/5.0f, 0.0f);
		break;
	}

	// set current coordinates as previous coordinates for next motion event
	lastX = x;
	lastY = y;
}

void idle()
{
	if (!paused) {
		//new particle generator
		particleCount++;
		if (particleCount % 100000 == 0) {
			fountain.addParticle(makeRandomParticle());
		}
	}
}

int main(int argc, char** argv)
{
	//glut initialization stuff:
	// set the window size, display mode, and create the window
	srand(1);
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Particle Fountain");

	//enable Z buffer test, otherwise things appear in the order they're drawn
	glEnable(GL_DEPTH_TEST);

	//turn on backface culling
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	//initialize and enable light
	//portions of this code were borrowed from http://www.glprogramming.com
	glEnable(GL_LIGHTING);
	GLfloat light_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat light_diffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 100.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);

	// set up the initial view
	// change to projection matrix mode, set the extents of our viewing volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, 1, 1, 1000);

	//set clear colour to black
	glClearColor(0, 0, 0, 0);

	glMatrixMode(GL_MODELVIEW);

	gluLookAt(xLookAt, yLookAt+100, 300, 0, 50, 0, 0, 1, 0);

	//register glut callbacks for keyboard and display function
	glutSpecialFunc(specialKeyboard);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	
	//print instructions
	using std::cout;
	using std::endl;
	cout << "Welcome to the Particle Fountain" << endl;
	cout << "Keys:" << endl;
	cout << "F: switch between a sticky or bouncy surface" << endl;
	cout << "W: add wind" << endl;
	cout << "1, 2, 3, 4: choose cube, sphere, cone, or teapot particles" << endl;
	cout << "Spacebar: pause the fountain (hit again to resume)" << endl;
	cout << "R: reset the fountain" << endl;
	cout << "Arrow keys: change your viewing angle" << endl;
	cout << "Mouse: change your view -- click left and drag to tilt, right to pan, scroll to zoom" << endl;
	cout << "The fountain will reset itself after a while" << endl;

	//start the program!
	glutMainLoop();

	return 0;
}