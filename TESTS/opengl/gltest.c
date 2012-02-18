#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "SDL.h"
/*#include<X11/X.h>*/
/*#include<X11/Xlib.h>*/
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#include<GL/glut.h>


#define X .525731112119133606 
#define Z .850650808352039932

static GLfloat vdata[12][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};
static GLuint tindices[20][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

static GLubyte halftone[] = {
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55};

SDL_Surface *sdl_gl_start(void)
{
        /* Information about the current video settings. */
        const SDL_VideoInfo* info = NULL;
        /* Dimensions of our window. */
        int width = 0;
        int height = 0;
        /* Color depth in bits of our window. */
        int bpp = 0;
        /* Flags we will pass into SDL_SetVideoMode. */
        int flags = 0;
        /* First, initialize SDL's video subsystem. */
        if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
                fprintf( stderr, "Video initialization failed: %s\n",
                SDL_GetError( ) );
                return(0);
        }

        info = SDL_GetVideoInfo( );

        if( !info ) {
                /* This should probably never happen. */
                fprintf( stderr, "Video query failed: %s\n",
                SDL_GetError( ) );
                return(0);
        }

        width = 640;
        height = 480;
        bpp = info->vfmt->BitsPerPixel;

        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

        flags = SDL_OPENGL;

        SDL_Surface *screen = SDL_SetVideoMode( width, height, bpp, flags );

        if (screen == 0) {
                fprintf( stderr, "Video mode set failed: %s\n",
                SDL_GetError( ) );
                return(0);
        }

        return screen;
}
/******************************************************************************/
SDL_Surface *sdl_load_bg(void)
{
        /* load bitmap to temp surface */
        SDL_Surface* temp = SDL_LoadBMP("bg.bmp");

        /* convert bitmap to display format */
        SDL_Surface* bg = SDL_DisplayFormat(temp);

        /* free the temp surface */
        SDL_FreeSurface(temp);

        return bg;
}
/******************************************************************************/
void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (0.41, 0.60, 0.54);
   glLoadIdentity (); /* clear the matrix */
   /* viewing transformation  */
   gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   glScalef (1.0, 2.0, 1.0);      /* modeling transformation */ 
   glutWireCube (1.0);
   glFlush();
}
/******************************************************************************/
void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
   glMatrixMode (GL_MODELVIEW);
}
/******************************************************************************/
void sdl_start_events(SDL_Surface *scr, SDL_Surface *bg)
{
        SDL_Event event;
        int gameover = 0;

        /* message pump */
        while (!gameover) {
                SDL_GL_SwapBuffers();
                /* look for an event */
                if (SDL_PollEvent(&event)) {
                        switch (event.type) {
                        case SDL_QUIT:
                                gameover = 1;
                                break;

                        case SDL_KEYDOWN:
                                switch (event.key.keysym.sym) {
                                case SDLK_ESCAPE:
                                case SDLK_q:
                                        gameover = 1;
                                        break;
                                }
                                break;
                        }
                }

                /* draw the background */
                SDL_BlitSurface(bg, NULL, scr, NULL);

                /* update the screen */
                SDL_UpdateRect(scr, 0, 0, 0, 0);
        }

        /* free the background surface */
        SDL_FreeSurface(bg);

        /* cleanup SDL */
        SDL_Quit();
}
/******************************************************************************/
int main(int argc, char** argv) {

        /*SDL_Surface *screen = sdl_gl_start();*/
        /*SDL_Surface *bg = sdl_load_bg();*/

        int i;

        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);
        glColor3f (1.0, 1.0, 1.0);
        glutInit(&argc, argv);
        glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
        glutInitWindowSize (500, 500); 
        glutInitWindowPosition (100, 100);
        glutCreateWindow (argv[0]);
        /*init();*/
        glutDisplayFunc(display); 
        glutReshapeFunc(reshape);
        glutMainLoop();

        return 1;
}
