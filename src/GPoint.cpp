/*
 * 3dfm - 3D File Manager
 * $Id: GPoint.cpp,v 1.10 2003/04/08 21:26:17 adam Exp $
 */

/* system includes */
#include <stdio.h>
#include <math.h>

/* 3dfm includes */
#include <GPoint.h>
#include <Prefs.h>

#define PI 3.1415

/* Construct a new GPoint with the specified coordinates.  This
 * constructor does not do any parameter checking at this time.
 *
 * PARAMS
 *   x - the X coordinate
 *   y - the Y coordinate
 *   z - the Z coordinate
 */
GPoint::GPoint(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

/* Construct a new GPoint with the default coordinates.  This
 * constructor does not do any parameter checking at this time.
 * The default coordinates are (0.0, 0.0, 0.0).
 *
 * PARAMS
 *   none
 */
GPoint::GPoint(void) {
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
}

/* Construct a new GPoint that is away from another GPoint.  The cnt specifies
 * how many other points will be around the original point, and idx specifies
 * which point this one will be.
 *
 * PARAMS
 *   pt - center of the circle
 *   cnt - how many points around the circle
 *   idx - which point around the circle this is
 *   dist - radius of circle
 */
GPoint::GPoint(GPoint pt, int cnt, int idx, GLfloat dist) {
    float angle = 2*PI/(float)cnt;
    this->x = pt.getX() + dist * cos(angle*idx+Prefs::child_rotate);
    this->y = pt.getY() + dist * sin(angle*idx+Prefs::child_rotate);
//    this->z = pt.getZ() - 5;
    this->z = pt.getZ() - 8.87;
#ifdef _DEBUG_DRAW
    fprintf(stderr, "GPoint::GPoint(pt, cnt=%d, idx=%d, dist=%f)\n",
	    cnt, idx, dist);
    fprintf(stderr, "GPoint::GPoint new (x=%f, y=%f, z=%f)\n", x, y, z);
#endif
}

/* Construct a new GPoint that is scaled by a factor specified.
 *
 * PARAMS
 *   pt - the point to scale
 *   scale - scaling factor
 */
GPoint::GPoint(GPoint pt, GLfloat scale) {
    this->x = pt.getX() * scale;
    this->y = pt.getY() * scale;
    this->z = pt.getZ() * scale;
}

GPoint GPoint::operator+(GPoint p) const {
    return GPoint(x+p.getX(), y+p.getY(), z+p.getZ());
}

GPoint GPoint::operator-(GPoint p) const {
    return GPoint(x-p.getX(), y-p.getY(), z-p.getZ());
}

GPoint GPoint::operator*(GLfloat scale) const {
    return GPoint(x*scale, y*scale, z*scale);
}

/* CROSS-PRODUCT */
GPoint GPoint::operator*(GPoint p) const {
    return GPoint(y*p.getZ() - z*p.getY(),
		  z*p.getX() - x*p.getZ(),
		  x*p.getY() - y*p.getX());
}

/* Returns the X coordinate
 *
 * RETURN
 *   the X coordinate
 */
float GPoint::getX(void) {
    return x;
}

/* Returns the Y coordinate
 *
 * RETURN
 *   the Y coordinate
 */
float GPoint::getY(void) {
    return y;
}

/* Sets the X coordinate
 *
 * PARAM
 *   x - the new x coord
 */
void GPoint::setX(GLfloat x) {
    this->x = x;
}

/* Sets the Y coordinate
 *
 * PARAM
 *   y - the new y coord
 */
void GPoint::setY(GLfloat y) {
    this->y = y;
}

/* Returns the Z coordinate
 *
 * RETURN
 *   the Z coordinate
 */
float GPoint::getZ(void) {
    return z;
}

/* Sets the Z coordinate
 *
 * PARAM
 *   z - the new z coord
 */
void GPoint::setZ(GLfloat z) {
    this->z = z;
}

/* Execute our GL.  This will place a vertex into the engine.  This method
 * should be called between glBegin() and glEnd() calls.
 *
 * POST-CONDITION
 *   The vertex is inserted into the engine.
 */
void GPoint::execGL(void) {
    glVertex3f(x, y, z);
}

#ifdef _DEBUG_DRAW
/* Dump the data for debug purposes.
 *
 */
void GPoint::dump(FILE *dumpFile) {
    fprintf(dumpFile, "(x=%f, y=%f, z=%f)", x, y, z);
}
#endif

/* Normalize the vector.
 */
void GPoint::normalize(void) {
    GLfloat length = (GLfloat) hypot(hypot((double)x, (double)y), (double)z);
    x /= length;
    y /= length;
    z /= length;
}

/* Return the length of the vector 
 */
GLfloat GPoint::getLength(void) {
    return (GLfloat) hypot(hypot((double)x, (double)y), (double)z);
}

void GPoint::translate(void) {
    glTranslatef(x, y, z);
}

    
