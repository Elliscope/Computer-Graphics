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

double Area(Vertex centerOfMass, Vertex v1,Vertex v2){
    //computer the areaRatio
    // Vertex centerOfMass;
    // centerOfMass.position[0]= tRatio*rayVector[0];
    // centerOfMass.position[1]= tRatio*rayVector[1];
    // centerOfMass.position[2]= tRatio*rayVector[2];

    //Somehow the value of AREA Doen's trafer down correctly
    //double Area= (1/2)*((v1.position[0]-centerOfMass.position[0])*(v2.position[1]-centerOfMass.position[1])-(v2.position[0]-centerOfMass.position[0])*(v1.position[1]-centerOfMass.position[1]));
    //cout<<(v1.position[0]-centerOfMass.position[0])*(v2.position[1]-centerOfMass.position[1])-(v2.position[0]-centerOfMass.position[0])*(v1.position[1]-centerOfMass.position[1])<<"the front Area value is "<<endl;
    //cout<<(v2.position[0]-centerOfMass.position[0])*(v1.position[1]-centerOfMass.position[1])<<"the back Area value is "<<endl;
    //cout<<Area<<" the Area value is "<<endl;
    return (v1.position[0]-centerOfMass.position[0])*(v2.position[1]-centerOfMass.position[1])-(v2.position[0]-centerOfMass.position[0])*(v1.position[1]-centerOfMass.position[1]);
}

//return t value
double IntersectionWithSphere(Vertex origin, Vertex ray,Sphere spheres,double length){
  Vertex InterSection;
  double a = 1;
  double b = 2*(ray.position[0]*(origin.position[0]-spheres.position[0]) + ray.position[1]*(origin.position[1]-spheres.position[1]) +ray.position[2]*(origin.position[2]-spheres.position[2]))/length;
  double c = pow((origin.position[0]-spheres.position[0]),2)+pow((origin.position[1]-spheres.position[1]),2)+pow((origin.position[2]-spheres.position[2]),2) - pow(spheres.radius,2);

  // cout<<"value of a "<<a<<endl;
  // cout<<"value of b "<<b<<endl;
  // cout<<"value of c "<<c<<endl;

  double solvable = pow(b,2)-4*a*c;
  if(solvable>0){
    double result1 = (-b + sqrt(solvable))/2;
    double result2 = (-b - sqrt(solvable))/2;

// cout<<"the value of result1 is "<<result1<<endl;
// cout<<"the value of result2 y is "<<result2<<endl;

    //need to double check the condition
    if(result1>0 && result2 >0){
      return min(result1,result2);
    }
  }else{
    return 0;
  }
}


//return t value
// double IntersectionWithTriangle(Vertex origin, Vertex ray, Triangle triangle){
//   //first side
//   double v1x = triangle.v[0].position[0]-triangle.v[1].position[0];
//   double v1y = triangle.v[0].position[1]-triangle.v[1].position[1];
//   double v1z = triangle.v[0].position[2]-triangle.v[1].position[2];

//   //second side
//   double v2x = triangle.v[0].position[0]-triangle.v[2].position[0];
//   double v2y = triangle.v[0].position[1]-triangle.v[2].position[1];
//   double v2z = triangle.v[0].position[2]-triangle.v[2].position[2];

//   //use eigen to compute the normal vector of the triangle 
//   // v1 x v2 = n; ->normalize it

//   Eigen::Vector3d trianglePoint(triangle.v[0].position[0],triangle.v[0].position[1],triangle.v[0].position[2]);
//   Eigen::Vector3d v(v1x,v1y,v1z);
//   Eigen::Vector3d w(v2x,v2y,v2z);


//   // cout<<" v is  "<< v<<endl;
//   // cout<<" w is  "<< w<<endl;


//   Eigen::Vector3d normalVector(v.cross(w)[0],v.cross(w)[1],v.cross(w)[2]);

//   normalVector.normalize();
//   // cout<<"THE NORMAL VECTOR IS "<<normalVector<<endl;

//   //n[a b c]
//   Eigen::Vector3d rayVector(ray.position[0],ray.position[1],ray.position[2]);
//   rayVector.normalize();

//   double d = - normalVector.dot(trianglePoint);
//   //double d = -n[0]*triangle[0].position[0] - n[1]*triangle[0].position[1]-n[2]*triangle[0].position[2];
//   // cout<<d<<" the value of d"<<endl;

//   //compute t
//   //t = -(d)/dot(n,d);

//   double t = -d/(normalVector.dot(rayVector));
//   // cout<<"t value "<<t<<endl;

//   if(t==0){
//     return 0;
//   }else{
//     //precompute the area of the triangle -> pass in as parameter
//     Vertex centerOfMass;
//     centerOfMass.position[0]= t*rayVector[0];
//     centerOfMass.position[1]= t*rayVector[1];
//     centerOfMass.position[2]= t*rayVector[2];

//     // cout<<centerOfMass.position[0]<<" the centerOfMass is "<< endl;
//     // cout<<centerOfMass.position[1]<<" the centerOfMass is "<< endl;
//     // cout<<centerOfMass.position[2]<<" the centerOfMass is "<< endl;

//     // double alph1 = Area(centerOfMass,triangle.v[0],triangle.v[1]);
//     // double alph2 = Area(centerOfMass,triangle.v[1],triangle.v[2]);
//     // double alph3 = Area(centerOfMass,triangle.v[0],triangle.v[2]);
//     double origin = Area(triangle.v[0],triangle.v[1],triangle.v[2]);
//     double alph1 = Area(centerOfMass,triangle.v[0],triangle.v[1])/origin;
//     double alph2 = Area(centerOfMass,triangle.v[1],triangle.v[2])/origin;
//     double alph3 = Area(centerOfMass,triangle.v[0],triangle.v[2])/origin;


//     // cout<<"alph1 value is "<<alph1<<endl;
//     // cout<<"alph2 value is "<<alph2<<endl;
//     // cout<<"alph3 value is "<<alph3<<endl;
//     // cout<<"origin value is "<<origin<<endl;
//     //if 1,2,3 all bigger than 0 and less than 1. Return true;

//     if(alph1>0 && alph2>0 &&alph3>0){
//       if(t>0){
//         return t;
//       }
//     }
//   }
//   return 0;
// }


//return t value
// double IntersectionWithTriangle(Vertex origin, Vertex ray, Triangle triangle){

//   Eigen::Vector3d P, Q;
//   Eigen::Vector3d D(ray.position[0],ray.position[1],ray.position[2]);
//   float det, inv_det, u, v;
//   float t;

//   //first side
//   double v1x = triangle.v[1].position[0]-triangle.v[0].position[0];
//   double v1y = triangle.v[1].position[1]-triangle.v[0].position[1];
//   double v1z = triangle.v[1].position[2]-triangle.v[0].position[2];

//   //second side
//   double v2x = triangle.v[2].position[0]-triangle.v[0].position[0];
//   double v2y = triangle.v[2].position[1]-triangle.v[0].position[1];
//   double v2z = triangle.v[2].position[2]-triangle.v[0].position[2];

//   Eigen::Vector3d trianglePoint(triangle.v[0].position[0],triangle.v[0].position[1],triangle.v[0].position[2]);
//   Eigen::Vector3d e1(v1x,v1y,v1z);
//   Eigen::Vector3d e2(v2x,v2y,v2z);

//   P = D.cross(e2);
//   det = e1.dot(P);

//   cout<<"the value of D "<<D<<endl;
//   cout<<"the value of det "<<det<<endl;
//   cout<<"the value of e1 "<<e1<<endl;
//   cout<<"the value of e2 "<<e2<<endl;

//   //if determinant is near zero, ray lies in plane of triangle
//   if(det > -EPSILON && det < EPSILON) return 0;

//   inv_det = 1.f/det;
//   cout<<"the value of inv_det "<<inv_det<<endl;
//   Eigen::Vector3d T(ray.position[0],ray.position[1],ray.position[2]);

//   u = T.dot(P) * inv_det;

//   cout<<"the value of u "<<u<<endl;
//   //if(u < 0.f || u > 1.f) return 0;

//   Q=T.cross(e1);
//   cout<<"the value of Q "<<Q<<endl;

//   //Calculate V parameter and test bound
//   v = D.dot(Q) * inv_det;


//   //The intersection lies outside of the triangle
//   //if(v < 0.f || u + v  > 1.f) return 0;

//   t = e2.dot(Q) * inv_det;

//   cout<<"e2.dot(Q) "<<e2.dot(Q)<<endl;

//   cout<<"the value of t inside the triangle FUNCTION "<<t<<endl;
//   if(t > EPSILON) { //ray intersection
//     return t;
//   }

//   // No hit, no win
//   return 0;

// }

double IntersectionWithTriangle(Eigen::Vector3d ray_origin,Vertex ray_direction, Triangle triangle)
{
  bool hitTriangle = false;


  Eigen::Vector3d P, Q;
  double det, inv_det, u, v;
  double t;
  Eigen::Vector3d T(ray_origin[0]-triangle.v[0].position[0],ray_origin[1]-triangle.v[0].position[1],ray_origin[2]-triangle.v[0].position[2]);

  //first edge
  double v1x = triangle.v[1].position[0]-triangle.v[0].position[0];
  double v1y = triangle.v[1].position[1]-triangle.v[0].position[1];
  double v1z = triangle.v[1].position[2]-triangle.v[0].position[2];

  //second edge
  double v2x = triangle.v[2].position[0]-triangle.v[0].position[0];
  double v2y = triangle.v[2].position[1]-triangle.v[0].position[1];
  double v2z = triangle.v[2].position[2]-triangle.v[0].position[2];


  Eigen::Vector3d e1(v1x,v1y,v1z);
  Eigen::Vector3d e2(v2x,v2y,v2z);
  Eigen::Vector3d D(ray_direction.position[0],ray_direction.position[1],ray_direction.position[2]);
  D.normalize();

  //Begin calculating determinant - also used to calculate u parameter
  P=D.cross(e2);
  //CROSS(P, D, e2);
  //if determinant is near zero, ray lies in plane of triangle
  det = e1.dot(P);

  //NOT CULLING
  if(det > -EPSILON && det < EPSILON) return 0;
  inv_det = 1.f / det;

  //Calculate u parameter and test bound
  u = T.dot(P) * inv_det;
  //The intersection lies outside of the triangle
  if(u < 0.f || u > 1.f) return 0;

  //Prepare to test v parameter
  Q = T.cross(e1);
  cout<<"Q "<<Q<<endl;


  //Calculate V parameter and test bound
  v = D.dot(Q)* inv_det;
  //The intersection lies outside of the triangle
  if(v < 0.f || u + v  > 1.f) return 0;

  t = e2.dot(Q) * inv_det;
  if(t > EPSILON) { //ray intersection
    return t;
  }
    // No hit, no win
  return 0;
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

  for(int i =0 ; i< 3; i++){
     CameraVector.position[i]=0;
  }
 
  //compute top left 
  TopLeft.position[0]= -aspectRatio * tan(angle/2);
  TopLeft.position[1]= tan(angle/2);
  TopLeft.position[2]=-1;

  //compute top right
  TopRight.position[0]=aspectRatio*tan(angle/2);
  TopRight.position[1]=tan(angle/2);
  TopRight.position[2]=-1;

  //compute bottom left;
  BotLeft.position[0]=- aspectRatio * tan(angle/2);
  BotLeft.position[1]=- tan(angle/2);
  BotLeft.position[2]=-1;

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

  //simple output
  for(x=0; x<WIDTH; x++)
  {
    glPointSize(2.0);  
    glBegin(GL_POINTS);
    for(y=0;y < HEIGHT;y++)
    {
      

      for(int i =0 ; i < num_spheres; i++){
        double length = sqrt(pow(StartVertex.position[0],2)+pow(StartVertex.position[1],2)+pow(StartVertex.position[2],2));
        double t1 = IntersectionWithSphere(CameraVector,StartVertex,spheres[i], length);
        if(t1!=0){
          plot_pixel(x,y,0,255,0);
          break;
        }
      }

      for(int i =0 ; i < num_triangles; i++){
      double t2 = IntersectionWithTriangle(ray_origin,StartVertex,triangles[i]);
        if(t2 !=0){
          plot_pixel(x,y,0,255,0);
          break;
        }
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
