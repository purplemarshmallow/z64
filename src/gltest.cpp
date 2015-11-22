#include "Gfx #1.3.h"
#include "rdp.h"
#include "rgl.h"

#include <SDL/SDL.h>
#include <IL/il.h>
#include <assert.h>

#include "glshader.cpp"

int screen_width = 1024, screen_height = 768;
SDL_Surface * sdl_Screen;

int main(int argc, char * * argv)
{
  const SDL_VideoInfo *videoInfo;
  Uint32 videoFlags = 0;
   
  /* Initialize SDL */
  printf("(II) Initializing SDL video subsystem...\n");
  if(SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
  {
    printf("(EE) Error initializing SDL video subsystem: %s\n", SDL_GetError());
    return -1;
  }
   
  /* Video Info */
  printf("(II) Getting video info...\n");
  if(!(videoInfo = SDL_GetVideoInfo()))
  {
    printf("(EE) Video query failed: %s\n", SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return -1;
  }
   
  /* Setting the video mode */
  videoFlags |= SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;
  
  if(videoInfo->hw_available)
    videoFlags |= SDL_HWSURFACE;
  else
    videoFlags |= SDL_SWSURFACE;
  
  if(videoInfo->blit_hw)
    videoFlags |= SDL_HWACCEL;
  
  //videoFlags |= SDL_FULLSCREEN;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
//   SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
//   SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//   SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
   
  printf("(II) Setting video mode %dx%d...\n", screen_width, screen_height);
  if(!(sdl_Screen = SDL_SetVideoMode(screen_width, screen_height, 0, videoFlags)))
  {
    printf("(EE) Error setting videomode %dx%d: %s\n", screen_width, screen_height, SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return -1;
  }
   
  char caption[500];
  sprintf(caption, "z64, LLE video plugin by Ziggy");
  SDL_WM_SetCaption(caption, caption);



  static char pixels[256*256*4];
  ilInit();
  glewInit();

  ilLoadImage("tex1.png");
  glBindTexture(GL_TEXTURE_2D, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               ilGetData());
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                   GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                   GL_LINEAR );
  ilLoadImage("tex2.png");
  glBindTexture(GL_TEXTURE_2D, 2);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               ilGetData());
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                   GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                   GL_LINEAR );

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glViewport(0, 0, screen_width, screen_height);

  glLoadIdentity();
  glScalef(2, 2, 1);
  glTranslatef(-0.5, -0.5, 0);

  rglShader_t * shader = rglCreateShader(
    "void main()                                                    \n"
    "{                                                              \n"
    "  gl_Position = ftransform();                                  \n"
    "  gl_FrontColor = gl_Color;                                    \n"
    "  gl_TexCoord[0] = gl_MultiTexCoord0;                          \n"
    "}                                                              \n"
    ,
    "uniform sampler2D texture0;       \n"
    "uniform sampler2D texture1;       \n"
    "                                  \n"
    "void main()                       \n"
    "{                                 \n"
    "  gl_FragColor = gl_Color * (texture2D(texture0, vec2(gl_TexCoord[0])) + texture2D(texture1, vec2(gl_TexCoord[0]))); \n"
    "}                                 \n"
  );
  rglUseShader(shader);

  int location;
  location = glGetUniformLocationARB(shader->prog, "texture0");
  glUniform1iARB(location, 0);
  location = glGetUniformLocationARB(shader->prog, "texture1");
  glUniform1iARB(location, 1);
  
  //glEnable(GL_TEXTURE_2D);

  glActiveTextureARB(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_2D, 1);
  glActiveTextureARB(GL_TEXTURE1_ARB);
  glBindTexture(GL_TEXTURE_2D, 2);
  

  glColor4ub(255,255,255,255);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0, 0);
  glVertex2f  (0, 0);
  
  glTexCoord2f(0, 1);
  glVertex2f  (0, 1);
  
  glTexCoord2f(1, 1);
  glVertex2f  (1, 1);
  
  glTexCoord2f(1, 0);
  glVertex2f  (1, 0);
  
  glEnd();

  SDL_GL_SwapBuffers();

  getchar();

  return 0;
}

