/*
 * 3dfm - 3D File Manager
 * $Id: GLine.h,v 1.3 2003/01/30 21:40:30 adam Exp $
 */

/*
 * GLine
 *
 * This object defines a line
 */

#ifndef _GLINE_H_
#define _GLINE_H_

#include <GL/gl.h>
#include <GPoint.h>
#include <GColor.h>

class GLine {
 private:
    GPoint pt1; // first endpoint
    GPoint pt2; // second endpoint
    GColor color;
 public:
    
    /** constructors **/
    GLine(GPoint pt1, GPoint pt2, GColor color);
//    GLine(GPoint pt1, GPoint pt2);

    /** getters/setters **/

    /** commands **/    
    void execGL(void);  // execute our GL (make a line)
};

#endif
