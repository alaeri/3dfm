/*
 * 3dfm - 3D File Manager
 * $Id: GRect.h,v 1.4 2003/03/23 22:13:59 kristopher Exp $
 */

/*
 * GRect
 */

#ifndef _GRECT_H_
#define _GRECT_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>

class GRect {
 private:
    GPoint topleft;		/* top left corner of the rectangle */
    GPoint topright;		/* top right corner of the rectangle */
    GPoint bottomleft;		/* bottom left corner of the rectangle */
    GPoint bottomright;		/* bottom right corner of the rectangle */

 public:
    GRect();
    GRect(GPoint upperleft, GPoint lowerright);
    ~GRect(void);
    GPoint Gettopleft();
    GPoint Gettopright();
    GPoint Getbottomleft();
    GPoint Getbottomright();
    GLfloat Getheight();
    GLfloat Getwidth();
    bool PtinRect(GPoint point);
    void ReSize(GPoint upperleft, GPoint lowerright);
};

#endif
