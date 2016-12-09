#include "Gfx #1.3.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "GL/glext.h"

static void
le32_to_header(unsigned char * header, unsigned long value)
{
#if 0
    if (value > 0x00000000FFFFFFFFul)
        value = 0x00000000FFFFFFFFul;
#endif
    *(header + 3) = (unsigned char)((value >> 24) & 0xFFu);
    *(header + 2) = (unsigned char)((value >> 16) & 0xFFu);
    *(header + 1) = (unsigned char)((value >>  8) & 0xFFu);
    *(header + 0) = (unsigned char)((value >>  0) & 0xFFu);
}

static void
glGetScreenDimensions(GLsizei* width, GLsizei* height)
{
    GLsizei viewport[4];

    *(width) = *(height) = 0; /* Or to be OCD, fetch the max dimensions. */
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    if (viewport[2] > 0)
        *(width)  = viewport[2];
    if (viewport[3] > 0)
        *(height) = viewport[3];
}

/*
 * Pack server-side video memory color components into client-side CPU pixel
 * storage of 8-bit-per-channel color component integers.
 *
 * The BMP image format is the only format directly co-aligning with this.
 */
static void
glDownloadFramebuffer(
    GLubyte* pixel_table,
    GLsizei width, GLsizei height
)
{
#ifdef _WIN32
    RECT screen_offset;
#endif
    GLint viewport[4];
    GLint x, y;

    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    x = viewport[0];
    y = viewport[1];
#ifdef _WIN32
    if (gfx.hStatusBar != NULL)
        GetClientRect(gfx.hStatusBar, &screen_offset);
    x += screen_offset.left;
    y += screen_offset.bottom;
#endif

#ifdef _DEBUG
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
#endif
    glReadPixels(
        x, y,
        width, height,
        GL_BGR,
        GL_UNSIGNED_BYTE,
        pixel_table
    );
}

void
glCaptureScreen(const char* path)
{
    GLsizei width, height;
    FILE* file_stream;
    GLubyte* file_data;
    size_t bytes_per_bitmap, elements_written;
    const size_t bytes_per_pixel = 3; /* 24-bit B8G8R8 is the universal BMP. */

    file_stream = fopen(path, "wb");
    if (file_stream == NULL) {
        fprintf(stderr, "Could not access for writing:\n%s\n\n", path);
        return;
    }

    glGetScreenDimensions(&width, &height);
    bytes_per_bitmap = bytes_per_pixel * (size_t)width * (size_t)height;
    file_data = (GLubyte *)calloc(bytes_per_bitmap + 128, sizeof(GLbyte));
    if (file_data == NULL)
        return; /* serv0r probably needs moar dedotated wam */

    /* magic number for bit-mapped image files on any platform */
    file_data[ 0] = 'B';
    file_data[ 1] = 'M';

#if 0
    file_data[14] = 12; /* smallest, universal minimalist BMP core header */
#else
    file_data[14] = 40;
#endif
    file_data[10 + 3 - (0 ^ 3)] = 64;

    file_data[26] = 1; /* required hard value of 1 color plane */
    file_data[28] = 24; /* forced to 24-bit B8G8R8 (not part of basic header) */

    le32_to_header(&file_data[34], bytes_per_bitmap);
    bytes_per_bitmap += file_data[10];
    le32_to_header(&file_data[ 2], bytes_per_bitmap);
    le32_to_header(&file_data[18], (GLuint)width);
    le32_to_header(&file_data[22], (GLuint)height);

    glDownloadFramebuffer(file_data + file_data[10], width, height);
    elements_written = fwrite(
        file_data,
        sizeof(unsigned char), bytes_per_bitmap,
        file_stream
    );
    if (elements_written != bytes_per_bitmap)
        fprintf(
            stderr,
            "ERROR:  Should have written %lu bytes but only wrote %lu.\n",
            (unsigned long)bytes_per_bitmap, (unsigned long)elements_written
        )
    ;
    while (fclose(file_stream) != 0)
        ;
    return;
}

static const char* path_modes[] = {
    "%s/%08X.bmp", "%s%08X.bmp"
}; /* Because some emulators on Windows forgot to add a '/' at the end. :) */
void capture(char* Directory)
{
    static unsigned long screen_count; /* very basic file-naming convention */
    FILE* file_stream;
    char* full_path;
    int problems_closing;
    const size_t path_length = strlen(Directory);
    const char end_char = *(Directory + path_length - 1);

    full_path = (char *)malloc(path_length + sizeof("/FFFFFFFF.bmp"));
    if (full_path == NULL)
        return;

    problems_closing = 0;
    do { /* Loop possible BMP file names until we hit one that isn't in use. */
        sprintf(
            full_path,
            path_modes[(end_char == '/' || end_char == '\\') ? 1 : 0],
            Directory, screen_count
        );
        file_stream = fopen(full_path, "rb");
        if (file_stream != NULL)
            problems_closing += fclose(file_stream) ? 1 : 0;

        screen_count = (screen_count + 1) & 0xFFFFFFFFul;
        if (screen_count == 0)
            return; /* User's shots folder has pow(2, 32) BMPs in it.  LOL? */
    } while (file_stream != NULL);

    if (problems_closing != 0)
        fprintf(
            stderr,
            "Warning:  %i problems with testing stream closures.\n",
            problems_closing
        );
    glCaptureScreen(full_path);
    return;
}
