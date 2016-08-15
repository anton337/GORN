#include <boost/thread.hpp>
#include <iostream>
#include <deque>
#include <set>
#include <exception>
#include "asio/client.h"
#include <GL/glut.h>

struct node
{
    std::string host;
    std::string dir;
    node * parent;
    float x , y;
    node ( std::string _host
         , std::string _dir
         , node * _parent
         )
    : host ( _host )
    , dir  ( _dir  )
    , parent ( _parent )
    {
        x = -1 + 2*(rand () % 1000) / 1000.0;
        y = -1 + 2*(rand () % 1000) / 1000.0;
    }
};

struct NodeComparator 
{
    bool operator () ( node * a , node * b )
    {
        return a -> host < b -> host || a -> dir < b -> dir ;
    }
};

std::set < node * , NodeComparator > M;
std::deque < node * > Q;
node * min_pt = NULL;

int width  = 1000;
int height = 1000;

int mouse_x = 0;
int mouse_y = 0;

void drawString (const char *s)    //How do display text 
{
    unsigned int i; 
    for (i = 0; i < strlen (s); i++)
    {
        glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, s[i]);
    }
};

void update_positions ()
{
    std::set < node * , NodeComparator >::iterator it = M . begin ();
    while ( it != M . end () )
    {
        if ( (*it)->parent )
        {
            double dx = (*it)->x - (*it)->parent->x;
            double dy = (*it)->y - (*it)->parent->y;
            double D = sqrt ( dx*dx + dy*dy );
            double R = D - .1;
            dx *= -0.0001*R/D;
            dy *= -0.0001*R/D;
            (*it)->x += dx;
            (*it)->y += dy;
            (*it)->parent->x -= dx;
            (*it)->parent->y -= dy;
        }
        ++it;
    }
    if ( min_pt )
    {
        double mx =  (2*(double)mouse_x/width -1); 
        double my = -(2*(double)mouse_y/height-1);
        min_pt->x += 0.01*(mx - min_pt->x);
        min_pt->y += 0.01*(my - min_pt->y);
    }
}

float camera_x = 0;
float camera_y = 0;
float camera_z = 0;

void drawStuff(void)
{
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 3,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */
    glTranslatef ( camera_x
                 , camera_y
                 , camera_z
                 );
    double min_R = 1000, R;
    std::set < node * , NodeComparator >::iterator it = M . begin ();
    while ( it != M . end () )
    {
        if ( (*it)->parent )
        {
            glColor3f(1,1,1);
            glBegin(GL_LINES);
            glVertex3f ( (*it)->x
                       , (*it)->y
                       , 0
                       );
            glVertex3f ( (*it)->parent->x
                       , (*it)->parent->y
                       , 0
                       );
            glEnd();
        }
        R = sqrt ( pow ( (*it)->x -  (2*(double)mouse_x/width -1) , 2 )
                 + pow ( (*it)->y - -(2*(double)mouse_y/height-1) , 2 )
                 );
        if ( R < min_R )
        {
            min_R = R;
            min_pt = (*it);
        }
        ++it;
    }
    if ( min_pt )
    {
        glBegin(GL_LINES);
        glVertex3f ( min_pt -> x + .2 , min_pt -> y , 0 );
        glVertex3f ( min_pt -> x - .2 , min_pt -> y , 0 );
        glVertex3f ( min_pt -> x , min_pt -> y + .2 , 0 );
        glVertex3f ( min_pt -> x , min_pt -> y - .2 , 0 );
        double mx =  (2*(double)mouse_x/width -1); 
        double my = -(2*(double)mouse_y/height-1);
        glVertex3f ( mx + .2 , my , 0 );
        glVertex3f ( mx - .2 , my , 0 );
        glVertex3f ( mx , my + .2 , 0 );
        glVertex3f ( mx , my - .2 , 0 );
        double r = .15;
        for ( double th(0)
            ; th < 2*M_PI
            ; th += 0.1
            )
        {
            glVertex3f ( mx + r * cos ( th ) , my + r * sin ( th ) , 0 );
            glVertex3f ( mx - r * sin ( th ) , my + r * cos ( th ) , 0 );
            glVertex3f ( mx + r * cos ( th+0.1 ) , my + r * sin ( th+0.1 ) , 0 );
            glVertex3f ( mx - r * sin ( th+0.1 ) , my + r * cos ( th+0.1 ) , 0 );
        }
        glEnd();
    }
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawStuff();
  glutSwapBuffers();
  glutPostRedisplay();
}

void get_connections ( std::string host
                     , std::string dir
                     , node * parent
                     , std::deque < node * > & Q
                     )
{
    boost::asio::io_service svc;
    int port = 80;
    Client client(svc, host, std::to_string(port));
    std::string request ( "GET "+dir+" HTTP/1.1\r\nHost: "+host+"\r\nConnection: close\r\n\r\n" );
    //std::cout << request << std::endl;
    std::string output = client . send_complete ( request );
    //std::cout << output << std::endl;
    {
        std::size_t pos = 0;
        std::size_t href_pos = 0;
        while ( (pos = output . find ( "<a " , pos+1 )) != std::string::npos )
        {
            href_pos = output . find ( "href=" , pos );
            std::string str = output . substr ( href_pos + 6 , output . find ( "\"" , href_pos + 8 ) - 6 - href_pos );
            std::size_t host_start = str . find ( "/" , 0          ) + 2;
            std::size_t host_end   = str . find ( "/" , host_start );
            if (  host_start != std::string::npos
               )
            {
                if ( host_end != std::string::npos
                   )
                {
                    std::string host = str . substr ( host_start , host_end - host_start );
                    std::string dir  = str . substr ( host_end+1 );
                    //std::cout << "found : " << str << std::endl;
                    //std::cout << host << " " << dir << std::endl;
                    Q . push_back ( new node ( host , "/"+dir , parent ) );
                }
                else
                {
                    std::string host = str . substr ( host_start , host_end - host_start );
                    //std::cout << "found : " << str << std::endl;
                    //std::cout << host << std::endl;
                    Q . push_back ( new node ( host , "/" , parent ) );
                }
            }
            else
            {
                //std::cout << "not found : " << str << std::endl;
            }
        }
    }
}

void connections_thread ()
{
    while ( true )
    {
        if ( ! Q . empty () )
        {
            try 
            {
                node * n = Q . front ();
                std::cout << Q . size () << " " << n -> host << " " << n -> dir << std::endl;
                Q . pop_front ();
                if ( M . find ( n ) == M . end () )
                {
                    M . insert ( n );
                    get_connections ( n -> host 
                                    , n -> dir
                                    , n
                                    , Q
                                    );
                }
            }
            catch ( std::exception & e )
            {
                std::cout << "Exception : " << e . what () << std::endl;
            }
        }
    }
}

void idle()
{
    for ( int k(0)
        ; k < 1000
        ; ++k
        )
    {
        update_positions ();
    }
    usleep(100000);
}

void mouse_func ( int button
                , int state
                , int x
                , int y
                )
{
    mouse_x = x;
    mouse_y = y;
}

void move_mouse_func ( int x
                     , int y
                     )
{
    mouse_x = x;
    mouse_y = y;
}

void keyboard ( unsigned char key
              , int x
              , int y
              )
{
    switch ( key )
    {
        case 27: exit(1);
        case 'r': 
            if ( min_pt )
            {
                std::stringstream cmd;
                cmd << "firefox";
                cmd << " ";
                cmd << min_pt->host;
                cmd << "/";
                cmd << min_pt->dir;
                cmd << " &";
                std::cout << cmd . str() << std::endl;
                int ret = system ( cmd . str () . c_str () );
                std::cout << ret << std::endl;
            }
            break;
        case 'q': camera_z += 0.01; break;
        case 'z': camera_z -= 0.01; break;
        default: break;
    }
}

void init(void)
{
  // GLdouble light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  /* Red diffuse light. */
  // GLdouble light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */
  /* Enable a single OpenGL light. */
  // glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  // glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  // glEnable(GL_LIGHT0);
  // glEnable(GL_LIGHTING);
  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);
  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  gluPerspective( /* field of view in degree */ 40.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 1.0, /* Z far */ 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 3,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */
  /* Adjust cube position to be asthetic angle. */
  // glTranslatef(0.0, 0.0, -1.0);
  // glRotatef(60, 1.0, 0.0, 0.0);
  // glRotatef(-20, 0.0, 0.0, 1.0);
}

int main(int argc,char **argv)
{
    srand(time(NULL));
    std::cout << "Welcome to Crawler!" << std::endl;
    Q . push_back ( new node ( "www.ask.com" , "/" , NULL ) );
    Q . push_back ( new node ( "www.google.com" , "/" , NULL ) );
    Q . push_back ( new node ( "www.facebook.com" , "/" , NULL ) );
    Q . push_back ( new node ( "www.youtube.com" , "/" , NULL ) );
    boost::thread t ( connections_thread );
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Web Crawler");
    glutReshapeWindow(width,height);
    init();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMouseFunc ( mouse_func );
    glutMotionFunc ( move_mouse_func );
    glutKeyboardFunc ( keyboard );
    glutMainLoop();
    return 0;
}

