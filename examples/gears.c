/* sdlGears.c */
/*
 * 3-D gear wheels by Brian Paul. This program is in the public domain.
 *
 * ported to libSDL/TinyGL by Gerald Franz (gfz@o2online.de)
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "GL/gl.h"

#include "SDL2/SDL.h"
#include "zbuffer.h"

#ifndef M_PI
#  define M_PI 3.14159265
#endif


/*
 * Draw a gear wheel.  You'll probably want to call this function when
 * building a display list since we do a lot of trig here.
 *
 * Input:  inner_radius - radius of hole at center
 *         outer_radius - radius at center of teeth
 *         width - width of gear
 *         teeth - number of teeth
 *         tooth_depth - depth of tooth
 */
static void gear( GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
                 GLint teeth, GLfloat tooth_depth )
{
    GLint i;
    GLfloat r0, r1, r2;
    GLfloat angle, da;
    GLfloat u, v, len;

    r0 = inner_radius;
    r1 = outer_radius - tooth_depth/2.0;
    r2 = outer_radius + tooth_depth/2.0;

    da = 2.0*M_PI / teeth / 4.0;

    glShadeModel( GL_FLAT );

    glNormal3f( 0.0, 0.0, 1.0 );

    /* draw front face */
    glBegin( GL_QUAD_STRIP );
    for (i=0;i<=teeth;i++) {
        angle = i * 2.0*M_PI / teeth;
        glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
        glVertex3f( r1*cos(angle), r1*sin(angle), width*0.5 );
        glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
    }
    glEnd();

    /* draw front sides of teeth */
    glBegin( GL_QUADS );
    da = 2.0*M_PI / teeth / 4.0;
    for (i=0;i<teeth;i++) {
        angle = i * 2.0*M_PI / teeth;

        glVertex3f( r1*cos(angle),      r1*sin(angle),      width*0.5 );
        glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   width*0.5 );
        glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
    }
    glEnd();

    glNormal3f( 0.0, 0.0, -1.0 );

    /* draw back face */
    glBegin( GL_QUAD_STRIP );
    for (i=0;i<=teeth;i++) {
        angle = i * 2.0*M_PI / teeth;
        glVertex3f( r1*cos(angle), r1*sin(angle), -width*0.5 );
        glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
        glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
    }
    glEnd();

    /* draw back sides of teeth */
    glBegin( GL_QUADS );
    da = 2.0*M_PI / teeth / 4.0;
    for (i=0;i<teeth;i++) {
        angle = i * 2.0*M_PI / teeth;

        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
        glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
        glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
        glVertex3f( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
    }
    glEnd();

    /* draw outward faces of teeth */
    glBegin( GL_QUAD_STRIP );
    for (i=0;i<teeth;i++) {
        angle = i * 2.0*M_PI / teeth;

        glVertex3f( r1*cos(angle),      r1*sin(angle),       width*0.5 );
        glVertex3f( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
        u = r2*cos(angle+da) - r1*cos(angle);
        v = r2*sin(angle+da) - r1*sin(angle);
        len = sqrt( u*u + v*v );
        u /= len;
        v /= len;
        glNormal3f( v, -u, 0.0 );
        glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),    width*0.5 );
        glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
        glNormal3f( cos(angle), sin(angle), 0.0 );
        glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da),  width*0.5 );
        glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
        u = r1*cos(angle+3*da) - r2*cos(angle+2*da);
        v = r1*sin(angle+3*da) - r2*sin(angle+2*da);
        glNormal3f( v, -u, 0.0 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da),  width*0.5 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
        glNormal3f( cos(angle), sin(angle), 0.0 );
    }

    glVertex3f( r1*cos(0), r1*sin(0), width*0.5 );
    glVertex3f( r1*cos(0), r1*sin(0), -width*0.5 );

    glEnd();


    glShadeModel( GL_SMOOTH );

    /* draw inside radius cylinder */
    glBegin( GL_QUAD_STRIP );
    for (i=0;i<=teeth;i++) {
        angle = i * 2.0*M_PI / teeth;
        glNormal3f( -cos(angle), -sin(angle), 0.0 );
        glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
        glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
    }
    glEnd();

}


static GLfloat view_rotx=20.0, view_roty=30.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;

void draw() {
    angle += 2.0;
    glPushMatrix();
    glRotatef( view_rotx, 1.0, 0.0, 0.0 );
    glRotatef( view_roty, 0.0, 1.0, 0.0 );
    //glRotatef( view_rotz, 0.0, 0.0, 1.0 );

    glPushMatrix();
    glTranslatef( -3.0, -2.0, 0.0 );
    glRotatef( angle, 0.0, 0.0, 1.0 );
    glCallList(gear1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( 3.1, -2.0, 0.0 );
    glRotatef( -2.0*angle-9.0, 0.0, 0.0, 1.0 );
    glCallList(gear2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( -3.1, 4.2, 0.0 );
    glRotatef( -2.0*angle-25.0, 0.0, 0.0, 1.0 );
    glCallList(gear3);
    glPopMatrix();

    glPopMatrix();
}


void initScene() {
    static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0 };
    static GLfloat red[4] = {1.0, 0.0, 0.0, 1.0 };
    static GLfloat green[4] = {0.0, 1.0, 0.0, 0.0 };
    static GLfloat blue[4] = {0.0, 0.0, 1.0, 1.0 };

    glLightfv( GL_LIGHT0, GL_POSITION, pos );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );

    /* make the gears */
    gear1 = glGenLists(1);
    glNewList(gear1, GL_COMPILE);
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red );
    gear( 1.0, 4.0, 1.0, 20, 0.7 );
    glEndList();

    gear2 = glGenLists(1);
    glNewList(gear2, GL_COMPILE);
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green );
    gear( 0.5, 2.0, 2.0, 10, 0.7 );
    glEndList();

    gear3 = glGenLists(1);
    glNewList(gear3, GL_COMPILE);
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue );
    gear( 1.3, 2.0, 0.5, 10, 0.7 );
    glEndList();
    glEnable( GL_NORMALIZE );

    glEnable( GL_LIGHTING );
}

int main(int argc, char **argv)
{
    // initialize SDL video:
    int winSizeX = 640;
    int winSizeY = 480;

    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        fprintf(stderr,"ERROR: cannot initialize SDL video.\n");
        return 1;
    }

	SDL_Window* window = SDL_CreateWindow("TinyGL test - 'Gears'",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              winSizeX, winSizeY,
                              SDL_WINDOW_INPUT_GRABBED);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_SOFTWARE);

    SDL_Surface* screen = SDL_CreateRGBSurface(0, winSizeX, winSizeY, 32, 0, 0, 0, 0);
    if (screen == NULL)
    {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_Texture* screenTexture;

	SDL_ShowCursor(SDL_FALSE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

    // initialize TinyGL:
    unsigned int pitch;
    int	mode;
    switch( screen->format->BitsPerPixel )
    {
    case  8:
        fprintf(stderr,"ERROR: Palettes are currently not supported.\n");
        return 1;
    case 16:
        pitch = screen->pitch;
        mode = ZB_MODE_5R6G5B;
        screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_TARGET, winSizeX, winSizeY);
        break;
    case 24:
        pitch = ( screen->pitch * 2 ) / 3;
        mode = ZB_MODE_RGB24;
        /*!!! bug with 24-bit color :-( !!!*/
        screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, winSizeX, winSizeY);
        break;
    case 32:
        pitch = screen->pitch / 2;
        mode = ZB_MODE_RGBA;
        screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, winSizeX, winSizeY);
        break;
    default:
        return 1;
        break;
    }
    ZBuffer* frameBuffer = ZB_open( winSizeX, winSizeY, mode, 0, 0, 0, 0);
    glInit( frameBuffer );

    // initialize GL:
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glViewport (0, 0, winSizeX, winSizeY);
    glEnable(GL_DEPTH_TEST);
    GLfloat  h = (GLfloat) winSizeY / (GLfloat) winSizeX;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -45.0 );

    initScene();

    // variables for timing:
    unsigned int frames=0;
    unsigned int tNow=SDL_GetTicks();
    unsigned int tLastFps=tNow;

    // main loop:
    int isRunning=1;
    while(isRunning)
    {
        ++frames;
        tNow=SDL_GetTicks();
        // do event handling:
        SDL_Event evt;
        while( SDL_PollEvent( &evt ) ) switch(evt.type) {
        case SDL_KEYDOWN:
            switch(evt.key.keysym.sym) {
            case SDLK_UP:
                view_rotx += 5.0;
                break;
            case SDLK_DOWN:
                view_rotx -= 5.0;
                break;
            case SDLK_LEFT:
                view_roty += 5.0;
                break;
            case SDLK_RIGHT:
                view_roty -= 5.0;
                break;
            case SDLK_ESCAPE :
            case SDLK_q :
                isRunning=0;
            default:
                break;
            }
            break;
        case SDL_QUIT:
            isRunning=0;
            break;
        }

        // draw scene:
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        draw();

        // swap buffers:
        ZB_copyFrameBuffer(frameBuffer, screen->pixels, pitch);
        SDL_UpdateTexture(screenTexture, NULL, screen->pixels, screen->pitch);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);

        // check for error conditions:
        char* sdl_error = SDL_GetError( );
        if( sdl_error[0] != '\0' )
        {
            fprintf(stderr,"SDL ERROR: \"%s\"\n",sdl_error);
            SDL_ClearError();
        }
        // update fps:
        if(tNow>=tLastFps+5000)
        {
            printf("%i frames in %f secs, %f frames per second.\n",frames,(float)(tNow-tLastFps)*0.001f,(float)frames*1000.0f/(float)(tNow-tLastFps));
            tLastFps=tNow;
            frames=0;
        }
    }
    printf("%i frames in %f secs, %f frames per second.\n",frames,(float)(tNow-tLastFps)*0.001f,(float)frames*1000.0f/(float)(tNow-tLastFps));
    // cleanup:
    ZB_close(frameBuffer);
    if(SDL_WasInit(SDL_INIT_VIDEO))
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
    return 0;
}
