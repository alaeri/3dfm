/*
 * 3dfm - 3D File Manager
 * $Id: IColorSelect.h,v 1.4 2003/04/04 23:36:30 zavnura Exp $
 */

/*
 * IColorSelect
 */

#ifndef _ICOLORSELECT_H_
#define _ICOLORSELECT_H_

/* system includes */
#include <GL/glut.h>

/* 3dfm includes */
#include <GColor.h>
#include <GRect.h>
#include <GPoint.h>

/* constants */
#define PI 3.141592654
#define CIRCLE_PTS 2000
#define INNER_R 17
#define OUTER_R 22
#define TRIAG_SIDE 3.464101616
#define LO 20

class IColorSelect {
 private:
    GLfloat ourhue; /* ourhue value */
    GLfloat imark_x;/* inner circle part of the marker for the S and V block*/
    GLfloat imark_y;/* inner circle part of the marker for the S and V block*/
    GLfloat omark_x;/* outer circle part of the marker for the Hue Circle*/
    GLfloat omark_y;/* outer circle part of the marker for the Hue Circle*/
    GLfloat fmark_x;/* outer circle part of the marker for the Hue Circle*/
    GLfloat fmark_y;/* outer circle part of the marker for the Hue Circle*/

    GLfloat s_val;  /* Saturation value */
    GLfloat v_val;  /* V value */

    GRect *S_block; /* Saturation block */
    GRect *V_block; /* V block */

    GPoint location;
    GPoint newcenter;
    
    GLfloat exportRed, exportGreen, exportBlue;

    /* Conversion of HSV to RGB values */
    void HSVtoRGB(GLfloat hue, GLfloat sat, GLfloat val,
    	unsigned char *red, unsigned char *green, unsigned char *blue);

    /* Create Clickable space */
    void CreateClickable(void);

 public:
    IColorSelect(GPoint location);

    int execGL(void);
    bool clicked(GPoint pt, int ClickType);
    void dragged(GPoint pt);
    void setcolor(GColor preview);
    GColor getcolor();
};

#endif
