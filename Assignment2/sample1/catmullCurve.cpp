#include <math.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <stdio.h>
 
 
struct point
{
	float x;
	float y;
	float z;
};
 
 
 
 
struct point CatmullRoll(float t, struct point p1, struct point p2, struct point p3, struct point p4)
{

	float t2 = t*t;
	float t3 = t*t*t;
	struct point v; // Interpolated point
 
	/* Catmull Rom spline Calculation */
 
	v.x = ((-t3 + 2*t2-t)*(p1.x) + (3*t3-5*t2+2)*(p2.x) + (-3*t3+4*t2+t)* (p3.x) + (t3-t2)*(p4.x))/2;
	v.y = ((-t3 + 2*t2-t)*(p1.y) + (3*t3-5*t2+2)*(p2.y) + (-3*t3+4*t2+t)* (p3.y) + (t3-t2)*(p4.y))/2;
	v.z = ((-t3 + 2*t2-t)*(p1.z) + (3*t3-5*t2+2)*(p2.z) + (-3*t3+4*t2+t)* (p3.z) + (t3-t2)*(p4.z))/2;

	printf("Values of v.x = %f and v.y = %f\n and v.z = %f\n", v.x,v.y,v.z);
 
	return v;	
}
 
void initScene()                                                
{
	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		// Antialias the lines
 
}
 
/* void mouse(int button,)
{
 
*/
void reshape(int w,int h)
{
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	if(w<=h)
		glOrtho(-60.0,60.0,-60.0*(GLfloat)h/(GLfloat)w,60.0*(GLfloat)h/(GLfloat)w,-60.0,60.0);
	else
		glOrtho(-60.0,6.0,-60.0*(GLfloat)w/(GLfloat)h,60.0*(GLfloat)w/(GLfloat)h,-60.0,60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 
}
void display(void)
{
	float t;
	struct point v;	//Interpolated point 
	struct point p1,p2,p3,p4,p5,p6,p7,p8,p9;
 
	// p1.x  = -30.25;p1.y = -10.25;
	// p2.x  = -40.5; p2.y  = -20.5;
	// p3.x  =  45.33;p3.y = 5.33;
	// p4.x  =  70.66;p4.y = 20.66;
 
 	p1.x  = -90;   p1.y = +30; p1.z = +30;
	p2.x  = -30; p2.y  = -30; p2.z = +30;
	p3.x  = +30 ;p3.y = +30; p3.z = +30;
	p4.x  = +90 ;p4.y = -30; p4.z = +30;
	p5.x  = +20 ;p5.y = -10; p5.z = +10;
	p6.x  = +60 ;p6.y = -90; p6.z = +30;


	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f,1.0f,1.0f);
	glPointSize(8);
	
 
	// glBegin(GL_POINTS);
	// 	glVertex3f(p1.x,p1.y,p1.z);
	// 	glVertex3f(p2.x,p2.y,p2.z);
	// 	glVertex3f(p3.x,p3.y,p3.z);
	// 	glVertex3f(p4.x,p4.y,p4.z);
	// glEnd();
 
	for(t=0;t<1;t+=0.01)
	{
		

		v = CatmullRoll(t,p1,p2,p3,p4);
		glBegin(GL_POINTS);
			glVertex3f(v.x,v.y,v.z);
		glEnd();

		v = CatmullRoll(t,p2,p3,p4,p5);
		glBegin(GL_POINTS);
			glVertex3f(v.x,v.y,v.z);
		glEnd();

		v = CatmullRoll(t,p3,p4,p5,p6);
		glBegin(GL_POINTS);
			glVertex3f(v.x,v.y,v.z);
		glEnd();

	}
	glFlush();
}
 
 
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	//glutInit(&amp;argc, argv;);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500,1000);
	glutInitWindowPosition(10,10);
	glutCreateWindow("Catmull Roll");
	initScene();
	glutDisplayFunc(display);	
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}