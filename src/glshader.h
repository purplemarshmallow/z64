/*
 * z64
 *
 * Copyright (C) 2007  ziggy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
**/

#ifndef _GLSHADER_H_
#define _GLSHADER_H_

#if defined(__MACOSX__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#elif defined(__MACOS__)
#include <gl.h>
#include <glext.h>
#else
#include <GL/gl.h>
#include "GL/glext.h"
#endif
#include "rdp.h"

typedef struct {
    GLhandleARB vs, fs, prog;
#ifdef RDP_DEBUG
    const char * vsrc, * fsrc;
#endif
} rglShader_t;

extern int init_GL_extensions(void);

extern PFNGLBINDFRAMEBUFFERPROC xglBindFramebuffer;

extern PFNGLGENFRAMEBUFFERSPROC xglGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC xglDeleteFramebuffers;

extern PFNGLDELETERENDERBUFFERSPROC xglDeleteRenderbuffers;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC xglFramebufferRenderbuffer;

extern PFNGLCHECKFRAMEBUFFERSTATUSPROC xglCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC xglFramebufferTexture2D;

extern PFNGLUNIFORM1IPROC xglUniform1i;
extern PFNGLGETUNIFORMLOCATIONPROC xglGetUniformLocation;
extern PFNGLBLENDFUNCSEPARATEPROC xglBlendFuncSeparate;

typedef void (APIENTRYP PFNGLMULTITEXCOORD2FPROC)(GLenum target, GLfloat s, GLfloat t);
extern PFNGLACTIVETEXTUREPROC xglActiveTexture;
extern PFNGLMULTITEXCOORD2FPROC xglMultiTexCoord2f;

rglShader_t * rglCreateShader(const char * vsrc, const char * fsrc);
void rglUseShader(rglShader_t * shader);
void rglDeleteShader(rglShader_t * shader);

#endif
