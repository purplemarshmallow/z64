/*
 * z64
 *
 * This program is free software; you can redistribute it and/
 * or modify it under the terms of the GNU General Public Li-
 * cence as published by the Free Software Foundation; either
 * version 2 of the Licence, or any later version.
 *
 * This program is distributed in the hope that it will be use-
 * ful, but WITHOUT ANY WARRANTY; without even the implied war-
 * ranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public Licence for more details.
 *
 * You should have received a copy of the GNU General Public
 * Licence along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
 * USA.
 *
**/

#include "Gfx #1.3.h"
#include "rdp.h"
#include "rgl.h"

#include <stdio.h>
#include <SDL.h>
#include <assert.h>

int screen_width = 1024, screen_height = 768;
SDL_Surface *sdl_Screen;

static __inline__ unsigned long long RDTSC(void)
{
  unsigned long long int x;
  __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
  return x;
}

void renderquad(int nbtex)
{
    glBegin(GL_TRIANGLE_STRIP);

    if (nbtex == 2)
        xglMultiTexCoord2f(GL_TEXTURE1_ARB, 0.5, 0);
    if (nbtex == 3)
        xglMultiTexCoord2f(GL_TEXTURE2_ARB, 0.5, 0);
    xglMultiTexCoord2f(GL_TEXTURE0_ARB, 1, 0);
    glVertex3f(1, 0, 0);

    if (nbtex == 2)
        xglMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 0);
    if (nbtex == 3)
        xglMultiTexCoord2f(GL_TEXTURE2_ARB, 0, 0);
    xglMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 0);
    glVertex3f(0, 0, 1);

    if (nbtex == 2)
        xglMultiTexCoord2f(GL_TEXTURE1_ARB, 0.5, 0.5);
    if (nbtex == 3)
        xglMultiTexCoord2f(GL_TEXTURE2_ARB, 0.5, 0.5);
    xglMultiTexCoord2f(GL_TEXTURE0_ARB, 1, 1);
    glVertex3f(1, 1, 1);

    if (nbtex == 2)
        xglMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 0.5);
    if (nbtex == 3)
        xglMultiTexCoord2f(GL_TEXTURE2_ARB, 0, 0.5);
    xglMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 1);
    glVertex3f(0, 1, 0);

    glEnd();
}

void test(int num, const char * vsrc, const char * fsrc, int nbtex)
{
  int i;
  rglShader_t * shader;
  static char src[10000];
  char * p = src;

  if (nbtex == 2)
    p += sprintf(p,
		 "uniform sampler2D texture1;       \n"
		 );
  if (nbtex == 3)
    p += sprintf(p,
		 "uniform sampler2D texture2;       \n"
		 );
  
  p += sprintf(p,
    "uniform sampler2D texture0;       \n"
    "                                  \n"
    "void main()                       \n"
    "{                                 \n"
	       );	       

  p += sprintf(p,
	       fsrc
	       );	       

  p += sprintf(p,
    "}                                 \n"
	       );	       

  shader = rglCreateShader(vsrc, src);

  rglUseShader(shader);

    int location;

    location = xglGetUniformLocation(shader->prog, "texture0");
    xglUniform1i(location, 0);
    if (nbtex == 2) {
        location = xglGetUniformLocation(shader->prog, "texture1");
        xglUniform1i(location, 1);
    }
    if (nbtex == 3) {
        location = xglGetUniformLocation(shader->prog, "texture2");
        xglUniform1i(location, 2);
    }

  glDisable(GL_DEPTH_TEST);
  glViewport(0, 0, screen_width, screen_height);
  glColor4ub(255,255,255,255);

  renderquad(nbtex);

  glFinish();
  uint64_t t = RDTSC();
  for (i=0; i<10; i++) {
    renderquad(nbtex);
  }
  glFinish();
  t = RDTSC() - t;

  printf("test #%02d %g\n", num, double(t) / 1000000);
  fflush(stdout);
}

static int GL_failed = -1;
void dobenchmark()
{
  static char tex[256*256*4];
  int i;
  float env[4];

    if (GL_failed < 0) {
        GL_failed = init_GL_extensions();
	printf("Errors loading OpenGL extensions:  %i\n", GL_failed);
    }

  glViewport(0, 0, screen_width, screen_height);

  glLoadIdentity();
  glScalef(2, 2, 1);
  glTranslatef(-0.5, -0.5, 0);

  for (i=0; i<sizeof(tex); i+=4) {
    tex[i] = i;
    tex[i] = i*3;
    tex[i] = i*5;
    tex[i] = i*7;
  }
  tex[1] = 255;
    glBindTexture(GL_TEXTURE_2D, 1);
    xglActiveTexture(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_2D, 1);
    xglActiveTexture(GL_TEXTURE2_ARB);
    glBindTexture(GL_TEXTURE_2D, 1);
    xglActiveTexture(GL_TEXTURE0_ARB);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex
    );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                   GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                   GL_LINEAR );
  glEnable(GL_TEXTURE_2D);

  const char * commonv =
    "void main()                                                    \n"
    "{                                                              \n"
    "  gl_Position = ftransform();                                  \n"
    "  gl_FrontColor = gl_Color;                                    \n"
    "  gl_BackColor = gl_FrontColor;                                \n"
    "  gl_TexCoord[0] = gl_MultiTexCoord0;                          \n"
    "}                                                              \n"
    ;
  
  const char * commonv2 =
    "void main()                                                    \n"
    "{                                                              \n"
    "  gl_Position = ftransform();                                  \n"
    "  gl_FrontColor = gl_Color;                                    \n"
    "  gl_BackColor = gl_FrontColor;                                \n"
    "  gl_TexCoord[0] = gl_MultiTexCoord0;                          \n"
    "  gl_TexCoord[2] = gl_MultiTexCoord2;                          \n"
    "}                                                              \n"
    ;
  
  test(0, commonv,
    "  gl_FragColor = texture2D(texture0, vec2(gl_TexCoord[0])); \n"
//     "int i; \n"
//     "for (gl_FragColor = 0, i=0; \n"
//     " i<texture2D(texture0, vec2(gl_TexCoord[0]))[0]*255; \n"
//     "i++)\n"
//     " gl_FragColor += vec4(0.01, 0.02, 0.03, -0.05);\n"
    ,1
  );

  env[0] = 0.2;
  env[1] = 0.6;
  env[2] = 0.8;
  env[3] = 1;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, env);
  test(1, 
    "void main()                                                    \n"
    "{                                                              \n"
    "  gl_Position = ftransform();                                  \n"
    "  gl_FrontColor = gl_Color;                                    \n"
    "  gl_BackColor = gl_FrontColor;                                \n"
    "  gl_FrontSecondaryColor = gl_LightSource[0].diffuse;          \n"
    "  gl_BackSecondaryColor = gl_FrontSecondaryColor;              \n"
    "  gl_TexCoord[0] = gl_MultiTexCoord0;                          \n"
    "}                                                              \n"
    ,
    "  gl_FragColor = gl_SecondaryColor * texture2D(texture0, vec2(gl_TexCoord[0])); \n"
    ,1
  );

//             "vec4  c = vec4(0,0,0,0);\n"
//             "vec4  e = gl_TextureEnvColor[0];\n"
//             "float k5 = gl_TextureEnvColor[1][0];\n"
//             "vec4  p = gl_LightSource[0].specular;\n"
//             "vec4  fog = gl_SecondaryColor;    \n"
//             "vec4  b = gl_LightSource[0].ambient;  \n"

  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env);
  test(2, commonv,
    "  gl_FragColor = gl_TextureEnvColor[0] * texture2D(texture0, vec2(gl_TexCoord[0])); \n"
    ,1
  );

  glLightfv(GL_LIGHT0, GL_AMBIENT, env);
  test(3, commonv,
    "  gl_FragColor = gl_LightSource[0].ambient * texture2D(texture0, vec2(gl_TexCoord[0])); \n"
    ,1
  );

    xglActiveTexture(GL_TEXTURE1_ARB);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env);
    xglActiveTexture(GL_TEXTURE0_ARB);
  test(4, commonv,
    "  gl_FragColor = gl_TextureEnvColor[1] * texture2D(texture0, vec2(gl_TexCoord[0])); \n"
    ,1
  );

  glLightfv(GL_LIGHT0, GL_SPECULAR, env);
  test(5, commonv,
    "  gl_FragColor = gl_LightSource[0].specular * texture2D(texture0, vec2(gl_TexCoord[0])); \n"
    ,1
  );

  test(6, commonv,
    "  gl_FragColor = clamp(vec4(0.5) - texture2D(texture0, vec2(gl_TexCoord[0])), 0.0, 1.0); \n"
    ,1
  );

  test(7, 
    "void main()                                                    \n"
    "{                                                              \n"
    "  gl_Position = ftransform();                                  \n"
    "  gl_FrontColor = gl_Color;                                    \n"
    "  gl_BackColor = gl_FrontColor;                                \n"
    "  gl_TexCoord[0] = gl_MultiTexCoord0;                          \n"
    "  gl_TexCoord[1] = gl_MultiTexCoord1;                          \n"
    "}                                                              \n"
    ,
    "  gl_FragColor = texture2D(texture0, vec2(gl_TexCoord[0])) + texture2D(texture1, vec2(gl_TexCoord[1])); \n"
    ,2
  );

  test(8, commonv2,
    "  gl_FragColor = texture2D(texture0, vec2(gl_TexCoord[0])) + texture2D(texture2, vec2(gl_TexCoord[2])); \n"
    ,3
  );

  test(9, commonv2,
       "vec4  c = vec4(0,0,0,0);\n"
       "vec4  e = gl_TextureEnvColor[0];\n"
       "float k5 = gl_TextureEnvColor[1][0];\n"
       "vec4  p = gl_LightSource[0].specular;\n"
       "vec4  fog = gl_SecondaryColor;    \n"
       "vec4  b = gl_LightSource[0].ambient;  \n"
       "vec4 t1 = texture2D(texture0, vec2(gl_TexCoord[0]));\n"
       "gl_FragColor = t1; \n"
    ,1
  );

  test(10, commonv2,
       "vec4  c = vec4(0,0,0,0);\n"
       "vec4  e = gl_TextureEnvColor[0];\n"
       "float k5 = gl_TextureEnvColor[1][0];\n"
       "vec4  p = gl_LightSource[0].specular;\n"
       "vec4  fog = gl_SecondaryColor;    \n"
       "vec4  b = gl_LightSource[0].ambient;  \n"
       "vec4 t1 = texture2D(texture0, vec2(gl_TexCoord[0]));\n"
       "gl_FragColor = (e+p+fog+b)*t1*k5; \n"
    ,1
  );

  test(11, commonv2,
       "vec4  c = vec4(0,0,0,0);\n"
       "vec4  e = gl_TextureEnvColor[0];\n"
       "float k5 = gl_TextureEnvColor[1][0];\n"
       "vec4  p = gl_LightSource[0].specular;\n"
       "vec4  fog = gl_SecondaryColor;    \n"
       "vec4  b = gl_LightSource[0].ambient;  \n"
       "vec4 t1 = texture2D(texture0, vec2(gl_TexCoord[0]));\n"
       "gl_FragColor = (e+p)*t1; \n"
    ,1
  );
}

int main(int argc, char ** argv)
{
  printf("main\n");

  const SDL_VideoInfo *videoInfo;
  Uint32 videoFlags = 0;

  FILE * newstdout = freopen("zbench64log.txt", "w", stdout);
  //FILE * newstderr = freopen("zbench64err.txt", "w", stderr);
  //_dup2(_fileno(newstdout), _fileno(stderr));
  

  /* Initialize SDL */
  //printf("(II) Initializing SDL video subsystem...\n");
  if(SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
  {
    printf("(EE) Error initializing SDL video subsystem: %s\n", SDL_GetError());
    return -1;
  }
   
  /* Video Info */
  //printf("(II) Getting video info...\n");
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

//   if (rglStatus == RGL_STATUS_FULLSCREEN)
//     videoFlags |= SDL_FULLSCREEN;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
   
  //printf("(II) Setting video mode %dx%d...\n", screen_width, screen_height);
  if(!(sdl_Screen = SDL_SetVideoMode(screen_width, screen_height, 0, videoFlags)))
  {
    printf("(EE) Error setting videomode %dx%d: %s\n", screen_width, screen_height, SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return -1;
  }
   
  char caption[500];
  sprintf(caption, "z64 benchmarker");
  SDL_WM_SetCaption(caption, caption);

    dobenchmark();

  rglUseShader(0);
  SDL_GL_SwapBuffers();
  //getchar();

  return 0;
}
