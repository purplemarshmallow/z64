#ifdef _WIN32
#include <windows.h>
#else
#include <X11/X.h>
#include <GL/glx.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rgl_assert.h"
#include <GL/gl.h>
#include "GL/glext.h"
#include "glshader.h"

/*
 * 2016.03.07 cxd4 -- Instead of depending on GLEW, it should be easy enough
 * to establish what's in common between the extension-loading variations.
 */
#ifdef __egl_h_ /* mobile or embedded devices and OpenGL ES */
#define glGetProcAddress(fn)    eglGetProcAddress(fn)
#elif defined(GLX_H) /* Linux X-windowing system interface */
#define glGetProcAddress(fn)    glXGetProcAddress((const GLubyte *)fn)
#elif defined(_WINGDI_) /* Windows GDI generic software interface */
#define glGetProcAddress(fn)    wglGetProcAddress((LPCSTR)fn)
#elif defined(_SDL_H) /* Simple DirectMedia Layer */
#define glGetProcAddress(fn)    SDL_GL_GetProcAddress(fn)
#else
#error \
Unimplemented on this OS, or you forgot to include context creation API for it?
#endif

PFNGLLINKPROGRAMPROC xglLinkProgram;
PFNGLSHADERSOURCEPROC xglShaderSource;

PFNGLCOMPILESHADERPROC xglCompileShader;
PFNGLUSEPROGRAMOBJECTARBPROC xglUseProgramObject;

PFNGLATTACHOBJECTARBPROC xglAttachObject;
PFNGLDELETEOBJECTARBPROC xglDeleteObject;

PFNGLCREATESHADEROBJECTARBPROC xglCreateShaderObject;
PFNGLCREATEPROGRAMOBJECTARBPROC xglCreateProgramObject;

PFNGLGETINFOLOGARBPROC xglGetInfoLog;
PFNGLGETOBJECTPARAMETERIVARBPROC xglGetObjectParameteriv;

int init_GL_extensions(void)
{
    int number_of_errors;

    number_of_errors = 0;
    xglLinkProgram = (PFNGLLINKPROGRAMPROC)
        glGetProcAddress("glLinkProgramARB");
    xglShaderSource = (PFNGLSHADERSOURCEPROC)
        glGetProcAddress("glShaderSourceARB");
    xglCompileShader = (PFNGLCOMPILESHADERPROC)
        glGetProcAddress("glCompileShaderARB");
    xglUseProgramObject = (PFNGLUSEPROGRAMOBJECTARBPROC)
        glGetProcAddress("glUseProgramObjectARB");
    xglAttachObject = (PFNGLATTACHOBJECTARBPROC)
        glGetProcAddress("glAttachObjectARB");
    xglDeleteObject = (PFNGLDELETEOBJECTARBPROC)
        glGetProcAddress("glDeleteObjectARB");
    xglCreateShaderObject = (PFNGLCREATESHADEROBJECTARBPROC)
        glGetProcAddress("glCreateShaderObjectARB");
    xglCreateProgramObject = (PFNGLCREATEPROGRAMOBJECTARBPROC)
        glGetProcAddress("glCreateProgramObjectARB");
    xglGetInfoLog = (PFNGLGETINFOLOGARBPROC)
        glGetProcAddress("glGetInfoLogARB");
    xglGetObjectParameteriv = (PFNGLGETOBJECTPARAMETERIVARBPROC)
        glGetProcAddress("glGetObjectParameterivARB");

    number_of_errors += (xglLinkProgram == NULL) ? 1 : 0;
    number_of_errors += (xglCompileShader == NULL) ? 1 : 0;
    number_of_errors += (xglShaderSource == NULL) ? 1 : 0;
    number_of_errors += (xglUseProgramObject == NULL) ? 1 : 0;
    number_of_errors += (xglAttachObject == NULL) ? 1 : 0;
    number_of_errors += (xglDeleteObject == NULL) ? 1 : 0;
    number_of_errors += (xglCreateShaderObject == NULL) ? 1 : 0;
    number_of_errors += (xglCreateProgramObject == NULL) ? 1 : 0;
    number_of_errors += (xglGetInfoLog == NULL) ? 1 : 0;
    number_of_errors += (xglGetObjectParameteriv == NULL) ? 1 : 0;
    return (number_of_errors);
}

static void printInfoLog(GLhandleARB obj, const char * src)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

    xglGetObjectParameteriv(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        xglGetInfoLog(obj, infologLength, &charsWritten, infoLog);
    if (*infoLog)
      printf("%s\n%s", src, infoLog);
		free(infoLog);
  }
}

//#define rglAssert(...)
rglShader_t * rglCreateShader(const char * vsrc, const char * fsrc)
{
  GLhandleARB vs, fs, prog;

  //printf("Compiling shader :\n%s", fsrc);

    vs = xglCreateShaderObject(GL_VERTEX_SHADER_ARB);
    rglAssert(glGetError() == GL_NO_ERROR);
    fs = xglCreateShaderObject(GL_FRAGMENT_SHADER_ARB);
    rglAssert(glGetError() == GL_NO_ERROR);

    xglShaderSource(vs, 1, &vsrc, NULL);
    rglAssert(glGetError() == GL_NO_ERROR);
    xglShaderSource(fs, 1, &fsrc, NULL);
    rglAssert(glGetError() == GL_NO_ERROR);
    xglCompileShader(vs);
    rglAssert(glGetError() == GL_NO_ERROR);
    xglCompileShader(fs);
  rglAssert(glGetError() == GL_NO_ERROR);
  printInfoLog(vs, vsrc);
  printInfoLog(fs, fsrc);
    prog = xglCreateProgramObject();
    xglAttachObject(prog, fs);
    rglAssert(glGetError() == GL_NO_ERROR);
    xglAttachObject(prog, vs);
    rglAssert(glGetError() == GL_NO_ERROR);

    xglLinkProgram(prog);
    rglAssert(glGetError() == GL_NO_ERROR);

  rglShader_t * s = (rglShader_t *) malloc(sizeof(rglShader_t));
  s->vs = vs;
  s->fs = fs;
  s->prog = prog;
  //LOG("Creating shader %d %d %d\n", s->vs, s->fs, s->prog);
#ifdef RDP_DEBUG
  s->vsrc = _strdup(vsrc);
  s->fsrc = _strdup(fsrc);
#endif
  return s;
}

void rglUseShader(rglShader_t * shader)
{
    if (!shader)
        xglUseProgramObject(0);
    else
        xglUseProgramObject(shader->prog);
}

void rglDeleteShader(rglShader_t * shader)
{
  //LOG("Deleting shader %d %d %d\n", shader->vs, shader->fs, shader->prog);
    xglDeleteObject(shader -> prog);
    rglAssert(glGetError() == GL_NO_ERROR);
    xglDeleteObject(shader -> vs);
    rglAssert(glGetError() == GL_NO_ERROR);
    xglDeleteObject(shader -> fs);
    rglAssert(glGetError() == GL_NO_ERROR);
    free(shader);
}
