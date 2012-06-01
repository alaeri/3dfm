/*
 * 3dfm - 3D File Manager
 * $Id: GPoint.h,v 1.8 2003/03/26 20:39:59 adam Exp $
 */

/*
 * GLOOPoint
 *
 * This object defines a point to be used for vertices.
 */

#ifndef _GPOINT_H_
#define _GPOINT_H_

#include <GL/gl.h>
#include <stdio.h>

class GPoint {
 private:
    GLfloat x; // X coordinate
    GLfloat y; // Y coordinate
    GLfloat z; // Z coordinate
 public:
    
    /** constructors **/

    GPoint(void);
    GPoint(GLfloat x, GLfloat y, GLfloat z);
    GPoint(GPoint pt, int cnt, int idx, GLfloat dist);
    GPoint(GPoint pt, GLfloat scale);

    /** getters/setters **/

    GLfloat getX(void); // return the X coordinate
    GLfloat getY(void); // return the Y coordinate
    GLfloat getZ(void); // return the Z coordinate
    void setZ(GLfloat z);
    void setY(GLfloat y);
    void setX(GLfloat x);
    GLfloat getLength(void); // return the length of the "vector"

    /** commands **/
    
    void execGL(void);  // execute our GL (make a vertex)
    void normalize(void); // normalize to a length of one (as a vector)
    void translate(void);

    /** operators **/
    GPoint operator+(GPoint p) const;
    GPoint operator-(GPoint p) const;
    GPoint operator*(GLfloat scale) const;
    GPoint operator*(GPoint p) const;  /* CROSS PRODUCT */
    
#ifdef _DEBUG_DRAW
    /** debug **/
    void dump(FILE *dumpFile);
#endif
};

#endif
