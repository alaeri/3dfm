/*
 * 3dfm - 3D File Manager
 * $Id: GColor.h,v 1.7 2003/04/09 03:39:10 adam Exp $
 */

/*
 * GColor
 *
 * This object defines a color to be used.
 */

#ifndef _GCOLOR_H_
#define _GCOLOR_H_

#include <GL/gl.h>

class GColor {
 private:
    GLfloat red;   // our red component
    GLfloat green; // our green component
    GLfloat blue;  // our blue component
    GLfloat alpha; // our alpha component
 public:
    GColor(GLfloat red, GLfloat green, GLfloat blue);
    GColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    GColor(void);
    GLfloat getRed(void);   // return the red component
    GLfloat getGreen(void); // return the green component
    GLfloat getBlue(void);  // return the blue component
    GLfloat getAlpha(void); // return the alpha component
    void setAlpha(GLfloat a);
    void execGL(void);    // execute the GL for this object
    void execFullGL(void);  // exectue GL and add material properties too
    GColor operator+(GLfloat amt);
};

#endif
