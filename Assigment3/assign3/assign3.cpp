/*
CSCI 480
Assignment 3 Raytracer

Name: Mingzhe Fang
ID: 5301084138

*/

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "../pic/pic.h"
#include <string.h>
#include <vector>
#include <math.h>
#include <iostream>
#include "Dense"

using namespace std;
// using Eigen::MatrixXd;


#define MAX_TRIANGLES 2000
#define MAX_SPHERES 10
#define MAX_LIGHTS 10
#define PI 3.1415926
#define EPSILON 0.000001

char *filename=0;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2
int mode=MODE_DISPLAY;
//Test Matrix
Eigen::MatrixXd m(2,2);

//you may want to make these smaller for debugging purposes
#define WIDTH 640
#define HEIGHT 480

//the field of view of the camera

unsigned char buffer[HEIGHT][WIDTH][3];


struct Vertex
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double normal[3];
  double shininess;
};

typedef struct _Triangle
{
  struct Vertex v[3];
} Triangle;

typedef struct _Sphere
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double shininess;
  double radius;
} Sphere;

typedef struct _Light
{
  double position[3];
  double color[3];
} Light;

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles=0;
int num_spheres=0;
int num_lights=0;

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);

//return t value
void IntersectionWithSphere(Eigen::Vector3d origin, Eigen::Vector3d ray,Sphere* spheres,double* spherT){
  Vertex InterSection;
  for(int i =0 ; i < num_spheres; i++){
  ray.normalize();
  double length = sqrt(pow(ray[0],2)+pow(ray[1],2)+pow(ray[2],2));
  double a = 1;
  double b = 2*(ray[0]*(origin[0]-spheres[i].position[0]) + ray[1]*(origin[1]-spheres[i].position[1]) +ray[2]*(origin[2]-spheres[i].position[2]))/length;
  double c = pow((origin[0]-spheres[i].position[0]),2)+pow((origin[1]-spheres[i].position[1]),2)+pow((origin[2]-spheres[i].position[2]),2) - pow(spheres[i].radius,2);

  double solvable = pow(b,2)-4*a*c;
  if(solvable>0){
    double result1 = (-b + sqrt(solvable))/2;
    double result2 = (-b - sqrt(solvable))/2;
  
    //need to double check the condition
    if(result1>0 && result2 >0){
      spherT[i]= min(result1,result2);
    }
    }else{
        spherT[i]=-1;
    }
  }
}

bool isBlockedBySpheres(Eigen::Vector3d origin, Eigen::Vector3d ray,Sphere* spheres,int current, bool isSphere){
  Vertex InterSection;
  for(int i =0 ; i < num_spheres; i++){
    //jump over the current point
    if(i==current && isSphere) i++;

  ray.normalize();
  double length = sqrt(pow(ray[0],2)+pow(ray[1],2)+pow(ray[2],2));
  double a = 1;
  double b = 2*(ray[0]*(origin[0]-spheres[i].position[0]) + ray[1]*(origin[1]-spheres[i].position[1]) +ray[2]*(origin[2]-spheres[i].position[2]))/length;
  double c = pow((origin[0]-spheres[i].position[0]),2)+pow((origin[1]-spheres[i].position[1]),2)+pow((origin[2]-spheres[i].position[2]),2) - pow(spheres[i].radius,2);

  double solvable = pow(b,2)-4*a*c;
  if(solvable>0){
    double result1 = (-b + sqrt(solvable))/2;
    double result2 = (-b - sqrt(solvable))/2;
  
    //need to double check the condition
    if(result1>0 && result2 >0){
      return true;
    }
   }
  }
  return false;
}





bool isBlockedByTriangles(Eigen::Vector3d ray_origin, Eigen::Vector3d ray_direction, Triangle* triangle,int current,bool isSphere)
{
  Eigen::Vector3d P, Q;
  double det, inv_det, u, v;
  double t;

  for(int i=0;i<num_triangles;i++){
  //jump over the current point
  if(i==current && !isSphere) i++;

  Eigen::Vector3d T(ray_origin[0]-triangles[i].v[0].position[0],ray_origin[1]-triangles[i].v[0].position[1],ray_origin[2]-triangles[i].v[0].position[2]);

  //first edge
  double v1x = triangles[i].v[1].position[0]-triangles[i].v[0].position[0];
  double v1y = triangles[i].v[1].position[1]-triangles[i].v[0].position[1];
  double v1z = triangles[i].v[1].position[2]-triangles[i].v[0].position[2];

  //second edge
  double v2x = triangles[i].v[2].position[0]-triangles[i].v[0].position[0];
  double v2y = triangles[i].v[2].position[1]-triangles[i].v[0].position[1];
  double v2z = triangles[i].v[2].position[2]-triangles[i].v[0].position[2];


  Eigen::Vector3d e1(v1x,v1y,v1z);
  Eigen::Vector3d e2(v2x,v2y,v2z);
  Eigen::Vector3d D(ray_direction[0],ray_direction[1],ray_direction[2]);
  D.normalize();

  //Begin calculating determinant - also used to calculate u parameter
  P=D.cross(e2);

  //if determinant is near zero, ray lies in plane of triangle
  det = e1.dot(P);

  //NOT CULLING
  if(det > -EPSILON && det < EPSILON) {continue;}
  inv_det = 1.f / det;

  //Calculate u parameter and test bound
  u = T.dot(P) * inv_det;
  //The intersection lies outside of the triangle
  if(u < 0.f || u > 1.f) {continue;}

  //Prepare to test v parameter
  Q = T.cross(e1);

  //Calculate V parameter and test bound
  v = D.dot(Q)* inv_det;
  //The intersection lies outside of the triangle
  if(v < 0.f || u + v  > 1.f) { continue;}

  t = e2.dot(Q) * inv_det;
  cout<<"the value of t "<<t<<endl;
  if(t > EPSILON) { //ray intersection
    return true;
  }
  }
  return false;
}

void IntersectionWithTriangle(Eigen::Vector3d ray_origin,Eigen::Vector3d ray_direction, Triangle* triangle, double* trianT)
{
  Eigen::Vector3d P, Q;
  double det, inv_det, u, v;
  double t;

  for(int i=0;i<num_triangles;i++){

  Eigen::Vector3d T(ray_origin[0]-triangles[i].v[0].position[0],ray_origin[1]-triangles[i].v[0].position[1],ray_origin[2]-triangles[i].v[0].position[2]);

  //first edge
  double v1x = triangles[i].v[1].position[0]-triangles[i].v[0].position[0];
  double v1y = triangles[i].v[1].position[1]-triangles[i].v[0].position[1];
  double v1z = triangles[i].v[1].position[2]-triangles[i].v[0].position[2];

  //second edge
  double v2x = triangles[i].v[2].position[0]-triangles[i].v[0].position[0];
  double v2y = triangles[i].v[2].position[1]-triangles[i].v[0].position[1];
  double v2z = triangles[i].v[2].position[2]-triangles[i].v[0].position[2];


  Eigen::Vector3d e1(v1x,v1y,v1z);
  Eigen::Vector3d e2(v2x,v2y,v2z);
  Eigen::Vector3d D(ray_direction[0],ray_direction[1],ray_direction[2]);
  D.normalize();

  //Begin calculating determinant - also used to calculate u parameter
  P=D.cross(e2);

  //if determinant is near zero, ray lies in plane of triangle
  det = e1.dot(P);

  //NOT CULLING
  if(det > -EPSILON && det < EPSILON) {trianT[i]=-1; continue;}
  inv_det = 1.f / det;

  //Calculate u parameter and test bound
  u = T.dot(P) * inv_det;
  //The intersection lies outside of the triangle
  if(u < 0.f || u > 1.f) {trianT[i]=-1; continue;}

  //Prepare to test v parameter
  Q = T.cross(e1);  

  //Calculate V parameter and test bound
  v = D.dot(Q)* inv_det;
  //The intersection lies outside of the triangle
  if(v < 0.f || u + v  > 1.f) {trianT[i]=-1; continue;}

  t = e2.dot(Q) * inv_det;
  if(t > EPSILON) { //ray intersection
    trianT[i]=t;
  }else{
    trianT[i]=-1;
  }
  }
}

double FindMininum(double* array, int length){
  double min = 10000;
  for(int i = 0; i < length; i++){
    if(array[i]<min && array[i]>0){
      min=array[i];
    }
  }
  return min;
}

int FindIndexOf(double min, double* array, int length){
  for(int i = 0; i < length; i++){
    if(array[i]==min){
      return i;
    }
  }
  return -1;
}


Eigen::Vector3d ApplyPhongModelWithTriangle(Eigen::Vector3d intersection,Eigen::Vector3d _v, Eigen::Vector3d l, Triangle triangle, Light Light){
  
  Eigen::Vector3d v = - _v;

  //find the normal of triangle 
  //constant for single triangle
  Eigen::Vector3d n(triangle.v[0].normal[0],triangle.v[0].normal[1],triangle.v[0].normal[2]);

  //coefficient
  // double redL = L.color[0];
  // double greL = L.color[1];
  // double bluL = L.color[2];
  
  //light coefficient 
  Eigen::Vector3d L(Light.color[0],Light.color[1],Light.color[2]);
  //diffuse
  //assume use the first point by default
  Eigen::Vector3d Diffuse(triangle.v[0].color_diffuse[0],triangle.v[0].color_diffuse[1],triangle.v[0].color_diffuse[2]);
  //specular 
  Eigen::Vector3d Specular(triangle.v[0].color_specular[0],triangle.v[0].color_specular[1],triangle.v[0].color_specular[2]);

  //computer the alph value (shineness value)
  double alph = triangle.v[0].shininess;


  //reflecDir
  Eigen::Vector3d r = 2* l.dot(n)*n-l;
  r.normalize();

  Eigen::Vector3d pre_I = (Diffuse*(l.dot(n)) + Specular*pow(r.dot(v),alph));


  Eigen::Vector3d I(pre_I[0]*L[0],pre_I[1]*L[1],pre_I[2]*L[2]);
  return I;
}

Eigen::Vector3d ApplyPhongModelWithSphere(Eigen::Vector3d intersection,Eigen::Vector3d _v, Eigen::Vector3d l, Sphere sphere, Light Light){
  
  Eigen::Vector3d v = - _v;
  v.normalize();

  //find the normal of spheres
  Eigen::Vector3d n((1/sphere.radius)*(intersection[0]-sphere.position[0]),(1/sphere.radius)*(intersection[1]-sphere.position[1]),(1/sphere.radius)*(intersection[2]-sphere.position[2]));
  //light coefficient 
  Eigen::Vector3d L(Light.color[0],Light.color[1],Light.color[2]);
  //diffuse
  //assume use the first point by default
  Eigen::Vector3d Diffuse(sphere.color_diffuse[0],sphere.color_diffuse[1],sphere.color_diffuse[2]);
  //specular 
  Eigen::Vector3d Specular(sphere.color_specular[0],sphere.color_specular[1],sphere.color_specular[2]);

  //computer the alph value (shininess value)
  double alph = sphere.shininess;

  //reflecDir
  Eigen::Vector3d r = 2* l.dot(n)*n-l;
  r.normalize();
  double dr = r.dot(v);
  // if(dr<0){
  //   dr = 0;
  // }

  Eigen::Vector3d pre_I = (Diffuse*(l.dot(n)) + Specular*pow(dr,alph));

  Eigen::Vector3d I(pre_I[0]*L[0],pre_I[1]*L[1],pre_I[2]*L[2]);
  return I;

}


//MODIFY THIS FUNCTION
void draw_scene()
{
  unsigned int x,y;
  Vertex CameraVector;
  Vertex TopLeft;
  Vertex TopRight;
  Vertex BotLeft;
  Vertex BotRight;

  double DeltaX;
  double DeltaY;

  double aspectRatio = 1.333;
  double angle = PI/3;

  //compute top left 
  TopLeft.position[0]= -aspectRatio * tan(angle/2);
  TopLeft.position[1]= tan(angle/2);
  TopLeft.position[2]= -1;

  //compute top right
  TopRight.position[0]= aspectRatio*tan(angle/2);
  TopRight.position[1]= tan(angle/2);
  TopRight.position[2]= -1;

  //compute bottom left;
  BotLeft.position[0]= -aspectRatio * tan(angle/2);
  BotLeft.position[1]= -tan(angle/2);
  BotLeft.position[2]= -1;

  //compute bottom right
  BotRight.position[0] = aspectRatio*tan(angle/2);
  BotRight.position[1] = -tan(angle/2);
  BotRight.position[2] = -1;

  //computer Delta
  DeltaX = (TopRight.position[0]-TopLeft.position[0])/WIDTH;
  DeltaY = (TopLeft.position[1]-BotLeft.position[1])/HEIGHT;

  Vertex StartVertex;
  StartVertex.position[0] = BotLeft.position[0];
  StartVertex.position[1] = BotLeft.position[1];
  StartVertex.position[2] = BotLeft.position[2];

  Eigen::Vector3d ray_origin(0,0,0);
  double* trianT = new double[num_triangles];
  double* sphereT = new double[num_spheres];




  //simple output
  for(x=0; x<WIDTH; x++)
  {
    glPointSize(2.0);  
    glBegin(GL_POINTS);
    for(y=0;y < HEIGHT;y++)
    {

      bool isTriangle = false;
      bool isSphere = false;
      Eigen::Vector3d ray_direction(StartVertex.position[0],StartVertex.position[1],StartVertex.position[2]);

      IntersectionWithSphere(ray_origin,ray_direction,spheres,sphereT);
      //
       for(int a = 0; a < num_spheres;a++){
        if(sphereT[a]>0){
           plot_pixel(x,y,0,255,0);
        }
      }

      IntersectionWithTriangle(ray_origin,ray_direction,triangles,trianT);
      //
      for(int a = 0; a < num_triangles;a++){
        if(trianT[a]>0){
           plot_pixel(x,y,0,255,0);
        }
      }

      //detect the shortest intersection ray 
      //help to identify which sphere provides the smallest t value
      //for drawing color purpose

      double tsphere = FindMininum(sphereT,num_spheres);
      double ttriangle = FindMininum(trianT,num_triangles);
      int index;
      double min;

      if(tsphere<ttriangle){
        index = FindIndexOf(tsphere,sphereT,num_spheres);
        isSphere = true;
        min = tsphere;
      }else{
        index = FindIndexOf(ttriangle,trianT,num_triangles);
        isTriangle = true;
        min = ttriangle;
      }
      
      if(index!=-1){
        // cout<<"the value of min"<<min<<endl;
        // cout<<"the value of index "<<index<<endl;
          Eigen::Vector3d intersection(ray_origin[0]+min*ray_direction[0],ray_origin[1]+min*ray_direction[1],ray_origin[2]+min*ray_direction[2]);
        //use the minimum t to compute if blocked or not by light

          Eigen::Vector3d Color(0,0,0);

          for(int l = 0; l < num_lights ; l++){
          //plot the pixel as default 
          //as long as there is one light that's not blocked, print red
          //plot_pixel(x,y,255,0,0);

          Eigen::Vector3d Light(lights[l].position[0],lights[l].position[1],lights[l].position[2]);
          Eigen::Vector3d rayTowardsLight = Light - intersection;

          //Shadow Code 
          //Need fix
          if(isBlockedByTriangles(intersection,rayTowardsLight,triangles,index,isSphere) || isBlockedBySpheres(intersection,rayTowardsLight,spheres,index,isSphere)){
            plot_pixel(x,y,255,0,0);
          }

          //phong model with Trinagle
          // if(isTriangle){
          //   Color += ApplyPhongModelWithTriangle(intersection,ray_direction,rayTowardsLight, triangles[index], lights[l]);
          // }

          //phong model with Sphere
          // if(isSphere){
          //   Color += ApplyPhongModelWithSphere(intersection,ray_direction,rayTowardsLight, spheres[index], lights[l]);
          // }
          
          }
           // plot_pixel(x,y,Color[0]*255,Color[1]*255,Color[2]*255);
      }
      StartVertex.position[1]+=DeltaY;
    }
    StartVertex.position[1] = BotLeft.position[1];
    StartVertex.position[0]+=DeltaX;
    glEnd();
    glFlush();
  }
  printf("Done!\n"); fflush(stdout);
}



void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  glColor3f(((double)r)/256.f,((double)g)/256.f,((double)b)/256.f);
  glVertex2i(x,y);
}

void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  buffer[HEIGHT-y-1][x][0]=r;
  buffer[HEIGHT-y-1][x][1]=g;
  buffer[HEIGHT-y-1][x][2]=b;
}

void plot_pixel(int x,int y,unsigned char r,unsigned char g, unsigned char b)
{
  plot_pixel_display(x,y,r,g,b);
  if(mode == MODE_JPEG)
      plot_pixel_jpeg(x,y,r,g,b);
}

void save_jpg()
{
  Pic *in = NULL;

  in = pic_alloc(640, 480, 3, NULL);
  printf("Saving JPEG file: %s\n", filename);

  memcpy(in->pix,buffer,3*WIDTH*HEIGHT);
  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);      

}

void parse_check(char *expected,char *found)
{
  if(strcasecmp(expected,found))
    {
      char error[100];
      printf("Expected '%s ' found '%s '\n",expected,found);
      printf("Parse error, abnormal abortion\n");
      exit(0);
    }

}

void parse_doubles(FILE*file, char *check, double p[3])
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check(check,str);
  fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
  printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE*file,double *r)
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check("rad:",str);
  fscanf(file,"%lf",r);
  printf("rad: %f\n",*r);
}

void parse_shi(FILE*file,double *shi)
{
  char s[100];
  fscanf(file,"%s",s);
  parse_check("shi:",s);
  fscanf(file,"%lf",shi);
  printf("shi: %f\n",*shi);
}

int loadScene(char *argv)
{
  FILE *file = fopen(argv,"r");
  int number_of_objects;
  char type[50];
  int i;
  Triangle t;
  Sphere s;
  Light l;
  fscanf(file,"%i",&number_of_objects);

  printf("number of objects: %i\n",number_of_objects);
  char str[200];

  parse_doubles(file,"amb:",ambient_light);

  for(i=0;i < number_of_objects;i++)
    {
      fscanf(file,"%s\n",type);
      printf("%s\n",type);
      if(strcasecmp(type,"triangle")==0)
	{

	  printf("found triangle\n");
	  int j;

	  for(j=0;j < 3;j++)
	    {
	      parse_doubles(file,"pos:",t.v[j].position);
	      parse_doubles(file,"nor:",t.v[j].normal);
	      parse_doubles(file,"dif:",t.v[j].color_diffuse);
	      parse_doubles(file,"spe:",t.v[j].color_specular);
	      parse_shi(file,&t.v[j].shininess);
	    }

	  if(num_triangles == MAX_TRIANGLES)
	    {
	      printf("too many triangles, you should increase MAX_TRIANGLES!\n");
	      exit(0);
	    }
	  triangles[num_triangles++] = t;
	}
      else if(strcasecmp(type,"sphere")==0)
	{
	  printf("found sphere\n");

	  parse_doubles(file,"pos:",s.position);
	  parse_rad(file,&s.radius);
	  parse_doubles(file,"dif:",s.color_diffuse);
	  parse_doubles(file,"spe:",s.color_specular);
	  parse_shi(file,&s.shininess);

	  if(num_spheres == MAX_SPHERES)
	    {
	      printf("too many spheres, you should increase MAX_SPHERES!\n");
	      exit(0);
	    }
	  spheres[num_spheres++] = s;
	}
      else if(strcasecmp(type,"light")==0)
	{
	  printf("found light\n");
	  parse_doubles(file,"pos:",l.position);
	  parse_doubles(file,"col:",l.color);

	  if(num_lights == MAX_LIGHTS)
	    {
	      printf("too many lights, you should increase MAX_LIGHTS!\n");
	      exit(0);
	    }
	  lights[num_lights++] = l;
	}
      else
	{
	  printf("unknown type in scene description:\n%s\n",type);
	  exit(0);
	}
    }
  return 0;
}

void display()
{

}

void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(0,WIDTH,0,HEIGHT,1,-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  //hack to make it only draw once
  static int once=0;
  if(!once)
  {
      draw_scene();
      if(mode == MODE_JPEG)
	save_jpg();
    }
  once=1;
}

int main (int argc, char ** argv)
{
  if (argc<2 || argc > 3)
  {  
    printf ("usage: %s <scenefile> [jpegname]\n", argv[0]);
    exit(0);
  }
  if(argc == 3)
    {
      mode = MODE_JPEG;
      filename = argv[2];
    }
  else if(argc == 2)
    mode = MODE_DISPLAY;

  glutInit(&argc,argv);
  loadScene(argv[1]);

  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(WIDTH,HEIGHT);
  int window = glutCreateWindow("Ray Tracer");
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  init();
  glutMainLoop();
}
