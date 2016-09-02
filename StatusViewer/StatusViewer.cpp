#include <boost/thread.hpp>
#include "system/memory.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <GL/glut.h>
#include <vector>
#include "asio/client.h"
#include "asio/server.h"
#include "info.h"
#include "serializers/system_status_serialize.h"

#define BUFFER_SIZE -1

int width = 1000;
int height = 1000;

struct Chunk
{
    std::string message;
    Chunk ( std::string _message 
          )
    : message ( _message )
    {

    }
};

Server < Chunk , ProducerConsumerQueue < Chunk > > * node_server = NULL;

ProducerConsumerQueue < Chunk > * Queue = new ProducerConsumerQueue < Chunk > (BUFFER_SIZE);

void drawStuff()
{

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawStuff();
    glutSwapBuffers();
    glutPostRedisplay();
}

void idle()
{
    sleep(1);
}

void mouse_func ( int button
                , int state
                , int x
                , int y
                )
{

}

void move_mouse_func ( int x
                     , int y
                     )
{

}

void keyboard ( unsigned char key
              , int x
              , int y
              )
{
    switch ( key )
    {
        case 27: exit(1);
        default: break;
    }
}

void init(void)
{
    glEnable(GL_DEPTH_TEST);
    /* Setup the view of the cube. */
    glMatrixMode(GL_PROJECTION);
    gluPerspective( /* field of view in degree */ 40.0,
      /* aspect ratio */ 1.0,
      /* Z near */ 0.01, /* Z far */ 10.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 3,  /* eye is at (0,0,5) */
      0.0, 0.0, 0.0,      /* center is at (0,0,0) */
      0.0, 1.0, 0.);      /* up is in positive Y direction */
}

host_info host;

std::vector < connection_info > connections;


void request_status_thread ( std::string host 
                           , std::size_t port 
                           , std::string ret_host
                           , std::size_t ret_port
                           ) 
{
    boost::asio::io_service svc;
    Client client(svc, ret_host, std::to_string(ret_port));
    std::stringstream get_status;
    get_status << GET_SYSTEM_STATUS_TYPE << " " << host << " " << port;
    client.send ( get_status . str () );
}


void request_status_loop ( std::string host
                         , std::size_t port
                         , std::string ret_host
                         , std::size_t ret_port
                         )
{
    while ( 1 )
    {
        request_status_thread ( host 
                              , port 
                              , ret_host
                              , ret_port
                              );
        sleep(1);
    }
}

void consumeItem ( Chunk * item )
{
    if ( item != NULL )
    {
        switch ( Serialize :: extract_type ( item -> message ) )
        {
            case SYSTEM_STATUS_TYPE :
            {
                std::cout << "Receiving system status report ... " << std::endl;
                SystemStatusMessage < InfoPackage > report;
                if ( report . deserialize ( item -> message ) != 0 )
                {
                    std::cout << "failed to deserialize status message " << std::endl;
                    break;
                }
                std::cout << "Status Report : " << item -> message << std::endl;
                break;
            }
            case GET_SYSTEM_STATUS_TYPE :
            {
                std::cout << "Get system status request ... " << std::endl;
                break;
            }
            default :
            {
                // message type not recognized 
                std::cout << "Unrecognized message : " << item -> message << std::endl;
                break;
            }
        }
    }
}

void consumer_thread()
{
    while(1)
    {
        Chunk * item = (Queue) -> get();
        if ( item == NULL )
        {
            std::cout << "item is NULL ... " << std::endl;
            continue;
        }
        consumeItem(item);
        delete item;
        item = NULL;
    }
}


int main(int argc,char ** argv)
{
    std::string hosts_file;
    if ( argc != 2 )
    {
        std::cout << "try ./StatusViewer <hosts_file>" << std::endl;
        return 1;
    }

    hosts_file = std::string(argv[1]);

    parse_config_file ( hosts_file 
                      , host
                      , connections
                      );

    std::cout << "Monitor Address : " << host . host_name << " " << host . port_no << std::endl;

    node_server = new Server < Chunk , ProducerConsumerQueue < Chunk > > ( Queue 
                                                                         , host . port_no 
                                                                         );

    std::vector < boost::thread * > threads;
    for ( std::size_t k(0)
        ; k < connections . size ()
        ; ++k
        )
    {
        std::cout << "Monitoring : " << connections[k] . host_name << " " << connections[k] . port_no << std::endl;
        threads . push_back ( new boost::thread ( request_status_loop 
                                                , host . host_name 
                                                , host . port_no
                                                , connections[k] . host_name
                                                , connections[k] . port_no
                                                )
                            );
    }

    threads . push_back ( new boost::thread ( consumer_thread ) );

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Status Viewer");
    glutReshapeWindow(width,height);
    init();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMouseFunc ( mouse_func );
    glutMotionFunc ( move_mouse_func );
    glutKeyboardFunc ( keyboard );
    glutMainLoop();
    for ( std::size_t k(0)
        ; k < threads . size ()
        ; ++k
        )
    {
        threads[k] -> join ();
    }
    return 0;
}

