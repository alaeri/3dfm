/*
 * 3dfm - 3D File Manager
 * $Id: GLine.cpp,v 1.6 2003/04/08 19:21:11 adam Exp $
 */

/* System includes */
#include <stdio.h>

/* 3dfm includes */
#include <GLine.h>

/* Construct a new GLine with the specified endpoints.  This
 * constructor does not do any parameter checking at this time.
 *
 * PARAMS
 *   pt1 - the first endpoint
 *   pt2 - the second endpoint
 */
GLine::GLine(GPoint pt1, GPoint pt2, GColor color) {
//GLine::GLine(GPoint pt1, GPoint pt2) {
    this->pt1 = pt1;
    this->pt2 = pt2;
    this->color = color;
}

/* Execute our GL.  This will place a vertex into the engine.  This method
 * should be called between glBegin() and glEnd() calls.
 *
 * POST-CONDITION
 *   The vertex is inserted into the engine.
 */
void GLine::execGL(void) {
  glEnable(GL_BLEND);
    color.execFullGL();
#ifdef _DEBUG_DRAW
    fprintf(stderr, "GLine::execGL() pt1: ");
    pt1.dump(stderr);
    fprintf(stderr, "\nGLine::execGL() pt2: ");
    pt2.dump(stderr);
    fprintf(stderr, "\n");
#endif
    glEnable(GL_CULL_FACE);

    glCullFace(GL_FRONT);
    glLineWidth(1.0);
    
    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    pt1.execGL();
    pt2.execGL();
    glEnd();

    glCullFace(GL_BACK);
    glNormal3f(0.0, 0.0, -1.0);
    glBegin(GL_LINES);
    (pt2 + GPoint(0.0,0.0,-0.05)).execGL();
    (pt1 + GPoint(0.0,0.0,-0.05)).execGL();
    glEnd();

    glDisable(GL_CULL_FACE);
    glNormal3f(0.0, 0.0, 1.0);
    glLineWidth(1.0);
    glDisable(GL_BLEND);
}
