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
#include <math.h>  

using namespace std;

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
float g_vLandScale[3] = {0.5, 0.5, 0.5};

int CameraPoint = 0;
//ground
GLuint texture[6];

GLfloat sceneLength = 1000;
double scale = 5.5;
//speedControl range from 0~1; is the t increment value of spline.
double speedControl = 0.01;
GLboolean ini = false;

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
point * pointArray;
point * allPointsArray;
point * tangentArray;
point * normalArray;
point * BArray;

void printPoint(string name, point x){
    cout<<"The point "<<name <<" has value "<<x.x << " "<<x.y<<" " <<x.z<<endl; 
}

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


void texload(int i,char *filename)
{
 Pic* img;
 img = jpeg_read(filename, NULL);
 glBindTexture(GL_TEXTURE_2D, texture[i]);
 glTexImage2D(GL_TEXTURE_2D,
 0,
GL_RGB,
 img->nx,
 img->ny,
 0,
GL_RGB,
GL_UNSIGNED_BYTE,
 &img->pix[0]);
 pic_free(img);

} 

 
struct point CatmullRoll(float t, struct point p1, struct point p2, struct point p3, struct point p4)
{

  float t2 = t*t;
  float t3 = t*t*t;
  struct point v; // Interpolated point
 
  /* Catmull Rom spline Calculation */
 
  v.x = ((-t3 + 2*t2-t)*(p1.x) + (3*t3-5*t2+2)*(p2.x) + (-3*t3+4*t2+t)* (p3.x) + (t3-t2)*(p4.x))/2;
  v.y = ((-t3 + 2*t2-t)*(p1.y) + (3*t3-5*t2+2)*(p2.y) + (-3*t3+4*t2+t)* (p3.y) + (t3-t2)*(p4.y))/2;
  v.z = ((-t3 + 2*t2-t)*(p1.z) + (3*t3-5*t2+2)*(p2.z) + (-3*t3+4*t2+t)* (p3.z) + (t3-t2)*(p4.z))/2;

  return v; 
}

struct point tagentVector(float t, struct point p1, struct point p2, struct point p3, struct point p4)
{
  float t2 = t;
  float t3 = t*t;
  struct point v; // Interpolated point


  v.x = ((-3*t3 + 2*2*t2-1)*(p1.x) + (3*3*t3-2*5*t2)*(p2.x) + (-3*3*t3+2*4*t2+1)* (p3.x) + (3*t3-2*t2)*(p4.x));
  v.y = ((-3*t3 + 2*2*t2-1)*(p1.y) + (3*3*t3-2*5*t2)*(p2.y) + (-3*3*t3+2*4*t2+1)* (p3.y) + (3*t3-2*t2)*(p4.y));
  v.z = ((-3*t3 + 2*2*t2-1)*(p1.z) + (3*3*t3-2*5*t2)*(p2.z) + (-3*3*t3+2*4*t2+1)* (p3.z) + (3*t3-2*t2)*(p4.z));
  return v; 
}

struct point crossProduct(point p1, point p2){
  struct point n;
  n.x = (p1.y*p2.z)-(p1.z*p2.y);
  n.y = (p1.z*p2.x)-(p1.x*p2.z);
  n.z = (p1.x*p2.y)-(p1.y*p2.x);
  return n;
}

void normalizeVector(point& p){
  double length = sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
  point temp;

  p.x = p.x/length;
  p.y = p.y/length;
  p.z = p.z/length;
}

 
void initScene()                                                
{
  glGenTextures(6, texture);
  texload(0,"skybox/negy.jpg");
  texload(1,"skybox/posz.jpg");
  texload(2,"skybox/posy.jpg");
  texload(3,"skybox/negz.jpg");
  texload(4,"skybox/posx.jpg");
  texload(5,"skybox/negx.jpg");
 
  glClearColor(0.0,0.0,0.0,0.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);  // Make round points, not square points
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);   // Antialias the lines
}
 
void reshape(int w,int h)
{
  GLfloat fovy = 30.0;
  GLfloat aspect = (GLfloat) w / (GLfloat) h;
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy,aspect,0.01,1000);
  glMatrixMode(GL_MODELVIEW);  
  
}

void storePoints(double i, double t, double x, double y,double z,int sizeCounter){
  //int temp = (int)(i*100+t*100);
  allPointsArray[sizeCounter].x=x;
  allPointsArray[sizeCounter].y=y;
  allPointsArray[sizeCounter].z=z;
}


void storeTangentValue(double i, double t, double x, double y,double z,int sizeCounter){

  tangentArray[sizeCounter].x=x;
  tangentArray[sizeCounter].y=y;
  tangentArray[sizeCounter].z=z;
}


void storeNormalVector(double i, double t, double x, double y,double z,int sizeCounter){

  normalArray[sizeCounter].x=x;
  normalArray[sizeCounter].y=y;
  normalArray[sizeCounter].z=z;
}

void PopulateNormalVectorArray(){
  for(int i = 1 ; i < g_Splines[0].numControlPoints/speedControl; i++){

  struct point v; //Interpolated point 
  struct point tangent;
  struct point normal ;

  for(int i= 0 ; i < g_Splines[0].numControlPoints ; i++){
        pointArray[i].x = g_Splines[0].points[i].x/scale;
        pointArray[i].y = g_Splines[0].points[i].y/scale;
        pointArray[i].z = g_Splines[0].points[i].z/scale;
     
  }

  int sizeCounter = 0;
  glPointSize(8);
    for(int i = 0 ; i< g_Splines[0].numControlPoints - 3; i++){
        for(double t=0;t<1;t+=speedControl)
        {
          //compute the segmentation point
          v = CatmullRoll(t,pointArray[i],pointArray[i+1],pointArray[i+2],pointArray[i+3]);
          storePoints(i,t,v.x,v.y,v.z,sizeCounter);

          //computer the tangent value at each segmentation point
          tangent = tagentVector(t,pointArray[i],pointArray[i+1],pointArray[i+2],pointArray[i+3]);
          storeTangentValue(i,t,tangent.x,tangent.y,tangent.z,sizeCounter);

          sizeCounter++;
        }
    }
    //compute the normal value of each point 
    //manually create an arbitrary value and populate the array based on that
    struct point arbitraryPoint;
    struct point firstNormal;

    arbitraryPoint.x = 0;
    arbitraryPoint.y = 0;
    arbitraryPoint.z = 1;

    firstNormal = crossProduct(tangentArray[0],arbitraryPoint);
    normalizeVector(firstNormal);


    normalArray[0].x = firstNormal.x;
    normalArray[0].y = firstNormal.y;
    normalArray[0].z = firstNormal.z;

    BArray[0] = crossProduct(tangentArray[0],normalArray[0]);
    normalizeVector(tangentArray[i]);
    normalArray[i] = crossProduct(BArray[i-1],tangentArray[i]);
    normalizeVector(normalArray[i]);
    BArray[i] = crossProduct(tangentArray[i],normalArray[i]);

    normalizeVector(BArray[i]);
  }
}



 // void drawSpline(){
 //  ini = false;
 //  int sizeCounter = 0;
 //  point v0;
 //  point v1;
 //  point v2;
 //  point v3;

 //  point v4;
 //  point v5;
 //  point v6;
 //  point v7;

 //  point nbDif;
 //  point nbSum;

 //  double a = 0.001;

 //  glBegin(GL_TRIANGLE_STRIP);

 //    for(int i = 0 ; i< g_Splines[0].numControlPoints - 3; i++){
 //        for(double t=0;t<1;t+=speedControl)
 //        {
 //          nbDif.x = normalArray[sizeCounter].x- BArray[sizeCounter].x;
 //          nbDif.y = normalArray[sizeCounter].y- BArray[sizeCounter].y;
 //          nbDif.z = normalArray[sizeCounter].z- BArray[sizeCounter].z;

 //          nbSum.x = normalArray[sizeCounter].x+ BArray[sizeCounter].x;
 //          nbSum.y = normalArray[sizeCounter].y+ BArray[sizeCounter].y;
 //          nbSum.z = normalArray[sizeCounter].z+ BArray[sizeCounter].z;

 //          v0.x = allPointsArray[sizeCounter].x + a * (-nbDif.x);
 //          v0.y = allPointsArray[sizeCounter].y + a * (-nbDif.y);
 //          v0.z = allPointsArray[sizeCounter].z + a * (-nbDif.z);

 //          v1.x = allPointsArray[sizeCounter].x + a * (nbSum.x);
 //          v1.y = allPointsArray[sizeCounter].y + a * (nbSum.y);
 //          v1.z = allPointsArray[sizeCounter].z + a * (nbSum.z);

 //          v2.x = allPointsArray[sizeCounter].x + a * (nbDif.x);
 //          v2.y = allPointsArray[sizeCounter].y + a * (nbDif.y);
 //          v2.z = allPointsArray[sizeCounter].z + a * (nbDif.z);

 //          v3.x = allPointsArray[sizeCounter].x + a * (-nbSum.x);
 //          v3.y = allPointsArray[sizeCounter].y + a * (-nbSum.y);
 //          v3.z = allPointsArray[sizeCounter].z + a * (-nbSum.z);

 //          glColor3f(0.5f,0.0f,0.5f);

 //          glVertex3f(v0.x,v0.y,v0.z);
 //          glVertex3f(v1.x,v1.y,v1.z);
 //          glVertex3f(v2.x,v2.y,v2.z);
 //          glVertex3f(v3.x,v3.y,v3.z);

 //          if(((i==g_Splines[0].numControlPoints - 4))){
 //            glEnd();
 //            continue;
 //          }
 //          if(ini){
 //            glVertex3f(v1.x,v1.y,v1.z);
 //            glVertex3f(v5.x,v5.y,v5.z);
 //            glVertex3f(v4.x,v4.y,v4.z);
 //            glVertex3f(v0.x,v0.y,v0.z);

 //            glVertex3f(v2.x,v2.y,v2.z);
 //            glVertex3f(v6.x,v6.y,v6.z);
 //            glVertex3f(v5.x,v5.y,v5.z);
 //            glVertex3f(v1.x,v1.y,v1.z);

 //            glVertex3f(v2.x,v2.y,v2.z);
 //            glVertex3f(v6.x,v6.y,v6.z);
 //            glVertex3f(v7.x,v7.y,v7.z);
 //            glVertex3f(v3.x,v3.y,v3.z);

 //            glVertex3f(v0.x,v0.y,v0.z);
 //            glVertex3f(v4.x,v4.y,v4.z);
 //            glVertex3f(v7.x,v7.y,v7.z);
 //            glVertex3f(v3.x,v3.y,v3.z);
 //          }

 //          ini = true;

 //          v4.x = v0.x;
 //          v4.y = v0.y; 
 //          v4.z = v0.z ;

 //          v5.x = v1.x;
 //          v5.y = v1.y; 
 //          v5.z = v1.z; 

 //          v6.x = v2.x;
 //          v6.y = v2.y; 
 //          v6.z = v2.z; 
        
 //          v7.x = v3.x;
 //          v7.y = v3.y; 
 //          v7.z = v3.z; 

 //          sizeCounter++;
 //        }
 //    }
 //    glEnd();
 //    sizeCounter = 0;
 //    glColor3f(1.0f,1.0f,1.0f);
 // }

  void drawSpline(double offSet){
  ini = false;
  int sizeCounter = 0;
  point v0;
  point v1;
  point v2;
  point v3;

  point v4;
  point v5;
  point v6;
  point v7;

  point nbDif;
  point nbSum;
  
  double a = 0.001;

  glBegin(GL_TRIANGLE_STRIP);

    for(int i = 0 ; i< g_Splines[0].numControlPoints - 3; i++){
        for(double t=0;t<1;t+=speedControl)
        {
          nbDif.x = normalArray[sizeCounter].x- BArray[sizeCounter].x+offSet;
          nbDif.y = normalArray[sizeCounter].y- BArray[sizeCounter].y+offSet;
          nbDif.z = normalArray[sizeCounter].z- BArray[sizeCounter].z+offSet;

          nbSum.x = normalArray[sizeCounter].x+ BArray[sizeCounter].x+offSet;
          nbSum.y = normalArray[sizeCounter].y+ BArray[sizeCounter].y+offSet;
          nbSum.z = normalArray[sizeCounter].z+ BArray[sizeCounter].z+offSet;

          v0.x = allPointsArray[sizeCounter].x + a * (-nbDif.x)+offSet;
          v0.y = allPointsArray[sizeCounter].y + a * (-nbDif.y)+offSet;
          v0.z = allPointsArray[sizeCounter].z + a * (-nbDif.z)+offSet;

          v1.x = allPointsArray[sizeCounter].x + a * (nbSum.x)+offSet;
          v1.y = allPointsArray[sizeCounter].y + a * (nbSum.y)+offSet;
          v1.z = allPointsArray[sizeCounter].z + a * (nbSum.z)+offSet;

          v2.x = allPointsArray[sizeCounter].x + a * (nbDif.x)+offSet;
          v2.y = allPointsArray[sizeCounter].y + a * (nbDif.y)+offSet;
          v2.z = allPointsArray[sizeCounter].z + a * (nbDif.z)+offSet;

          v3.x = allPointsArray[sizeCounter].x + a * (-nbSum.x)+offSet;
          v3.y = allPointsArray[sizeCounter].y + a * (-nbSum.y)+offSet;
          v3.z = allPointsArray[sizeCounter].z + a * (-nbSum.z)+offSet;

          glColor3f(0.5f,0.0f,0.5f);

          glVertex3f(v0.x,v0.y,v0.z);
          glVertex3f(v1.x,v1.y,v1.z);
          glVertex3f(v2.x,v2.y,v2.z);
          glVertex3f(v3.x,v3.y,v3.z);

          if(((i==g_Splines[0].numControlPoints - 4))){
            glEnd();
            continue;
          }
          if(ini){
            glVertex3f(v1.x,v1.y,v1.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glVertex3f(v0.x,v0.y,v0.z);

            glVertex3f(v2.x,v2.y,v2.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glVertex3f(v1.x,v1.y,v1.z);

            glVertex3f(v2.x,v2.y,v2.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glVertex3f(v3.x,v3.y,v3.z);

            glVertex3f(v0.x,v0.y,v0.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glVertex3f(v3.x,v3.y,v3.z);
          }

          ini = true;

          v4.x = v0.x;
          v4.y = v0.y; 
          v4.z = v0.z ;

          v5.x = v1.x;
          v5.y = v1.y; 
          v5.z = v1.z; 

          v6.x = v2.x;
          v6.y = v2.y; 
          v6.z = v2.z; 
        
          v7.x = v3.x;
          v7.y = v3.y; 
          v7.z = v3.z; 

          sizeCounter++;
        }
    }
    glEnd();
    sizeCounter = 0;
    glColor3f(1.0f,1.0f,1.0f);
 }

void drawGround(int i){
    //display the ground image 
  
 glEnable(GL_TEXTURE_2D);
 glBindTexture(GL_TEXTURE_2D, texture[i]);
 glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

 glBegin(GL_POLYGON);
  if(i==0){
    glTexCoord2f(1.0, 0.0);
    glVertex3f(sceneLength, sceneLength, sceneLength);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-sceneLength, sceneLength, sceneLength);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-sceneLength, sceneLength, -sceneLength);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(sceneLength, sceneLength, -sceneLength);
  }else if(i==1){
    glTexCoord2f(1.0, 1.0);
    glVertex3f(sceneLength, sceneLength, sceneLength);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-sceneLength, sceneLength, sceneLength);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-sceneLength, -sceneLength, sceneLength);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(sceneLength, -sceneLength, sceneLength);
  }else if(i==2){
    glTexCoord2f(1.0, 1.0);
    glVertex3f(sceneLength, -sceneLength, sceneLength);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-sceneLength, -sceneLength, sceneLength);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-sceneLength, -sceneLength, -sceneLength);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(sceneLength, -sceneLength, -sceneLength);
  }else if(i==3){
    glTexCoord2f(0.0, 0.0);
    glVertex3f(sceneLength, -sceneLength, -sceneLength);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-sceneLength, -sceneLength, -sceneLength);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-sceneLength, sceneLength, -sceneLength);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(sceneLength, sceneLength, -sceneLength);
  }else if(i==4){
    glTexCoord2f(0.0, 1.0);
    glVertex3f(sceneLength, sceneLength, sceneLength);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(sceneLength, -sceneLength, sceneLength);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(sceneLength, -sceneLength, -sceneLength);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(sceneLength, sceneLength, -sceneLength);
  }else if(i==5){
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-sceneLength, sceneLength, sceneLength);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-sceneLength, -sceneLength, sceneLength);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-sceneLength, -sceneLength, -sceneLength);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-sceneLength, sceneLength, -sceneLength);
  }
 glEnd();
 glDisable(GL_TEXTURE_2D);
}


void updateCamera(){
  if(CameraPoint<(g_Splines[0].numControlPoints-3)/speedControl){

    int index = (g_Splines[0].numControlPoints-3)/speedControl- CameraPoint;

    GLdouble eyex = allPointsArray[index].x+0.02*normalArray[index].x;
    GLdouble eyey = allPointsArray[index].y+0.02*normalArray[index].y;
    GLdouble eyez = allPointsArray[index].z+0.02*normalArray[index].z;

    GLdouble centerx = (eyex+1*tangentArray[index].x);
    GLdouble centery = (eyey+1*tangentArray[index].y);
    GLdouble centerz = (eyez+1*tangentArray[index].z);
    
    gluLookAt(eyex,eyey,eyez,centerx,centery,centerz,-normalArray[index].x,-normalArray[index].y,-normalArray[index].z);
    
    // glBegin(GL_LINE_STRIP);
    //       glColor3f(1.0f,1.0f,0.0f);
    //       glVertex3f(eyex,eyey,eyez);
    //       glVertex3f(allPointsArray[CameraPoint].x+normalArray[CameraPoint].x,allPointsArray[CameraPoint].y+normalArray[CameraPoint].y,allPointsArray[CameraPoint].z+normalArray[CameraPoint].z);
    // glEnd();

    // glBegin(GL_LINE_STRIP);
    //       glColor3f(0.0f,1.0f,0.0f);
    //       glVertex3f(eyex,eyey,eyez);
    //       glVertex3f(centerx,centery,centerz);
    // glEnd();
  
    CameraPoint++;
  }else{
    //reset the cameara point to zero
    CameraPoint=0;
    return;
  }
}


void display(void)
{

  //rotation with movement of mouse drag
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glTranslatef(g_vLandTranslate[0],g_vLandTranslate[1],g_vLandTranslate[2]);

  glColor3f(1.0f,1.0f,1.0f);

  glRotatef(g_vLandRotate[0],1,0,0);
  glRotatef(g_vLandRotate[1],0,1,0);
  glRotatef(g_vLandRotate[2],0,0,1);
  glScalef(g_vLandScale[0],-g_vLandScale[1],-g_vLandScale[2]);

  updateCamera();
  // drawSpline();
  drawSpline(0.003);
  drawSpline(-0.003);

  drawGround(0);
  drawGround(1);
  drawGround(2);
  drawGround(3);
  drawGround(4);
  drawGround(5);

  glFlush();
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



void menufunc(int value)
{
  cout<<"menu function"<<endl;
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


int main (int argc, char ** argv)
{
  if (argc<2)
  {  
  printf ("usage: %s <trackfile>\n", argv[0]);
  exit(0);
  }

  loadSplines(argv[1]);
  pointArray = new point[(int)(g_Splines[0].numControlPoints)];
  allPointsArray = new point[(int)(g_Splines[0].numControlPoints/speedControl)];
  tangentArray = new point[(int)(g_Splines[0].numControlPoints/speedControl)];
  normalArray = new point[(int)(g_Splines[0].numControlPoints/speedControl)];
  BArray = new point[(int)(g_Splines[0].numControlPoints/speedControl)];

  //  replace with any animate code 
  // glutIdleFunc(doIdle);

  glutInit(&argc, argv);

  //glutInit(&amp;argc, argv;);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH | GLUT_RGBA);

  glutInitWindowSize(1000,1000);
  glutInitWindowPosition(1,1);
  glutCreateWindow("Catmull Roll");

  // enable depth buffering
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);            
  // interpolate colors during rasterization
  PopulateNormalVectorArray();

  glutDisplayFunc(display); 
  glutReshapeFunc(reshape);

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


  initScene();
  

  glutMainLoop();

  return 0;
}
