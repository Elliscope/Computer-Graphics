/*
  CSCI 480
  Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "pic.h"
#include <iostream>

  using namespace std;

/* represents one control point along the spline */
struct point {
   double x;
   double y;
   double z;
};

/* spline struct which contains how many control points, and an array of control points */
struct spline {
   int numControlPoints;
   struct point *points;
};

/* the spline array */
struct spline *g_Splines;

/* total number of splines */
int g_iNumOfSplines;


int loadSplines(char *argv) {
  char *cName = (char *)malloc(128 * sizeof(char));
  FILE *fileList;
  FILE *fileSpline;
  int iType, i = 0, j, iLength;


  /* load the track file */
  fileList = fopen(argv, "r");
  if (fileList == NULL) {
    printf ("can't open file\n");
    exit(1);
  }
  
  /* stores the number of splines in a global variable */
  fscanf(fileList, "%d", &g_iNumOfSplines);

  g_Splines = (struct spline *)malloc(g_iNumOfSplines * sizeof(struct spline));

  /* reads through the spline files */
  for (j = 0; j < g_iNumOfSplines; j++) {
    i = 0;
    fscanf(fileList, "%s", cName);
    fileSpline = fopen(cName, "r");

    if (fileSpline == NULL) {
      printf ("can't open file\n");
      exit(1);
    }

    /* gets length for spline file */
    fscanf(fileSpline, "%d %d", &iLength, &iType);

    /* allocate memory for all the points */
    g_Splines[j].points = (struct point *)malloc(iLength * sizeof(struct point));
    g_Splines[j].numControlPoints = iLength;

    /* saves the data to the struct */
    while (fscanf(fileSpline, "%lf %lf %lf", 
	   &g_Splines[j].points[i].x, 
	   &g_Splines[j].points[i].y, 
	   &g_Splines[j].points[i].z) != EOF) {
      i++;
    }
  }

  free(cName);

  return 0;
}


void display()
{
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); // reset transformation

    // draw a triangle
    glBegin(GL_TRIANGLES);
  //glColor4f(1.0,0.0,0.0,1.0);
        glVertex3f(0.0, 0.0, -10.0);
  //glColor4f(0.0,1.0,0.0,1.0);
        glVertex3f(1.0, 0.0, -10.0);
  //glColor4f(0.0,0.0,1.0,1.0);
        glVertex3f(0.0, 1.0, -10.0);
    glEnd();

    glutSwapBuffers(); // double buffer flush
}

// called every time window is resized to update projection matrix
void reshape(int w, int h)
{
    // setup image size
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // setup camera
    glFrustum(-0.1, 0.1, -float(h)/(10.0*float(w)), float(h)/(10.0*float(w)), 0.5, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



int main (int argc, char ** argv)
{

  //s = 1/2
  // double **basis = new double[4][4]=
  //   {
  //       {-1/2,3/2,-3/2,1/2},
  //       {1,-5/2,2,-1/2},
  //       {-1/2,0,1/2,0},
  //       {0,1,0,0}
  //   };

  double basis[4][4]=
    {
        {1,2,3,4},
        {5,6,7,8},
        {9,10,11,12},
        {13,14,15,16}
    };

  // double controlMatrix = new double[3][4];

  // printf（"can't open file\n");
  // printf("%s\n",*g_Splines );



  if (argc<2)
  {  
  printf ("usage: %s <trackfile>\n", argv[0]);
  exit(0);
  }

  //  glutInit(&argc,argv);
  //  // request double buffer

  //  //initialize
  //  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
   
  //  // set window size
  //  glutInitWindowSize(640, 480);
   
  //  // set window position
  //  glutInitWindowPosition(0, 0);
   
    
  //  // creates a window
  //  glutCreateWindow("Roller Coaster");


   
  //  // enable depth buffering
  //   glEnable(GL_DEPTH_TEST);
  //   glShadeModel(GL_SMOOTH);            // interpolate colors during rasterization

  


  // //  tells glut to use a particular display function to redraw 
  // glutDisplayFunc(display);
  // glutReshapeFunc(reshape);

  
  //  allow the user to quit using the right mouse button menu 
  // // g_iMenuId = glutCreateMenu(menufunc);
  // // glutSetMenu(g_iMenuId);
  // glutAddMenuEntry("Quit",0);
  // glutAttachMenu(GLUT_RIGHT_BUTTON);


  
  // // /* replace with any animate code */
  // // glutIdleFunc(doIdle);

  // // /* callback for mouse drags */
  // // glutMotionFunc(mousedrag);
  // // /* callback for idle mouse movement */
  // // glutPassiveMotionFunc(mouseidle);
  // // /* callback for mouse button changes */
  // // glutMouseFunc(mousebutton);


  // // //connect to keyboard function
  // // glutKeyboardFunc(keyboardFunciton);

  // /* do initialization */
  // // myinit();

  // glutMainLoop();

  loadSplines(argv[1]);

  cout<<"hello world"<<endl;
  cout<<g_iNumOfSplines<<endl;

   cout<< g_Splines[0].points[1].x<<endl; 
   cout<<g_Splines[0].points[1].y<<endl;
   cout<<g_Splines[0].points[1].z<<endl; 


  return 0;
}
