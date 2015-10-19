/*
  CSCI 480 Computer Graphics
  Assignment 1: Height Fields
  C++ starter code
*/

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include <iostream>

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {0.005, 0.005, 0.005};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
  int i, j;
  Pic *in = NULL;

  if (filename == NULL)
    return;

  /* Allocate a picture buffer */
  in = pic_alloc(640, 480, 3, NULL);

  printf("File to save to: %s\n", filename);

  for (i=479; i>=0; i--) {
    glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 &in->pix[i*in->nx*in->bpp]);
  }

  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);
}

void myinit()
{
  /* setup gl view here */
  glClearColor(0.0, 0.0, 0.0, 0.0);
}

void reshape(int w, int h)
{
     GLfloat aspect = (GLfloat) w / (GLfloat) h;
     glViewport(0, 0, w, h);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     gluPerspective(30.0, aspect, 0, 40);

     // if (w <= h) /* aspect <= 1 */
     //   glOrtho(-2.0, 2.0, -2.0/aspect, 2.0/aspect, -10, 10);
     // else  aspect > 1 
     //   glOrtho(-2.0*aspect, 2.0*aspect, -2.0, 2.0, -10, 10);
      glMatrixMode(GL_MODELVIEW); 
}

void MakeFile(int a){
  char myFilenm[2048];
 sprintf(myFilenm, "animation/anim.%04d.jpg", a);
 saveScreenshot(myFilenm);
 // myFilenm will be anim.0001.jpg, anim.0002.jpg..........anim.0999.jpg
 // ..
}


int a;

void display()
{
  /* draw 1x1 cube about origin */
  /* replace this code with your height field implementation */
  /* you may also want to precede it with your 
rotation/translation/scaling */

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();


  glTranslatef(g_vLandTranslate[0],g_vLandTranslate[1],g_vLandTranslate[2]);


  glRotatef(g_vLandRotate[0],1,0,0);
  glRotatef(g_vLandRotate[1],0,1,0);
  glRotatef(g_vLandRotate[2],0,0,1);
  glScalef(g_vLandScale[0],-g_vLandScale[1],-g_vLandScale[2]);


  glPushMatrix();
    glTranslatef(-(g_pHeightData->nx)/2, -(g_pHeightData->nx)/2, 0); // move object to centre

//for loop goes through the pixel and draw the heightfield
    for(int i=0;i<g_pHeightData->ny-1;i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for(int j=0;j<g_pHeightData->nx;j++) {
            float index0= PIC_PIXEL(g_pHeightData,j,i,0);
            float index1= PIC_PIXEL(g_pHeightData,j,i+1,0);

            float index2 = index0/255;
            float index3 = index1/255;
 //set the color based on the color depth of image
             glColor3f(index2, index2, index2);
             glVertex3f(j, i, index2*100);

             glColor3f(index3, index3, index3);
             glVertex3f(j, i+1, index3*100);
            
        }// next pixel in current row
        glEnd(); // finish drawing
    }// next row

  glPopMatrix();
    
  if(a<300){
      MakeFile(a);
      a++;
  }



  glutSwapBuffers(); // double buffer flush
}

void menufunc(int value)
{
  switch (value)
  {
    case 0:
      exit(0);
      break;
  }
}

void doIdle()
{
  /* do some stuff... */

  /* make the screen update */
  glutPostRedisplay();
}


void keyboardFunciton(unsigned char key, int x, int y){

//when click on 1,2,3, it change the heightfield to vertices,wireframe or solid triangles.

  if(key == '1'){
    //vertices
    glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );

   }else if(key == '2'){
    // render wireframe 
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

   }else if(key == '3'){
    // render solid triangles 
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
   }
}



/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
  
  switch (g_ControlState)
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.01;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.01;
      }
      break;
    case ROTATE:
      if (g_iLeftMouseButton)
      {
        g_vLandRotate[0] += vMouseDelta[1];
        g_vLandRotate[1] += vMouseDelta[0];
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandRotate[2] += vMouseDelta[1];
      }
      break;
    case SCALE:
      if (g_iLeftMouseButton)
      {
        g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
        g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
      }
      break;
  }
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;
  }
 
  switch(glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      g_ControlState = TRANSLATE;
      break;
    case GLUT_ACTIVE_SHIFT:
      g_ControlState = SCALE;
      break;
    default:
      g_ControlState = ROTATE;
      break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

int main (int argc, char ** argv)
{
  if (argc<2)
  {  
    printf ("usage: %s heightfield.jpg\n", argv[0]);
    exit(1);
  }

  g_pHeightData = jpeg_read(argv[1], NULL);
  if (!g_pHeightData)
  {
    printf ("error reading %s.\n", argv[1]);
    exit(1);
  }

   glutInit(&argc,argv);
   // request double buffer

   //initialize
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
   
   // set window size
   glutInitWindowSize(640, 480);
   
   // set window position
   glutInitWindowPosition(0, 0);
   
    
   // creates a window
   glutCreateWindow("First Open GL Project");


   
   // enable depth buffering
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);            // interpolate colors during rasterization

  


  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);

  
  /* allow the user to quit using the right mouse button menu */
  g_iMenuId = glutCreateMenu(menufunc);
  glutSetMenu(g_iMenuId);
  glutAddMenuEntry("Quit",0);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  
  /* replace with any animate code */
  glutIdleFunc(doIdle);

  /* callback for mouse drags */
  glutMotionFunc(mousedrag);
  /* callback for idle mouse movement */
  glutPassiveMotionFunc(mouseidle);
  /* callback for mouse button changes */
  glutMouseFunc(mousebutton);


  //connect to keyboard function
  glutKeyboardFunc(keyboardFunciton);

  /* do initialization */
  myinit();

  glutMainLoop();
  return(0);
}
