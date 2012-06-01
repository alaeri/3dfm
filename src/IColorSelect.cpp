/*
 * 3dfm - 3D File Manager
 * $Id: IColorSelect.cpp,v 1.9 2003/04/10 20:20:47 zavnura Exp $
 */


/*
 * IColorSelect
 */

/* system includes */
//#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <GColor.h>
#include <IColorSelect.h>
#include <FFolder.h>
#include <GRect.h>
#include <GPoint.h>

IColorSelect::IColorSelect(GPoint location) {

    ourhue = 0;    /* ourhue value */
    imark_x = 0.0; /* inner circle part of the marker for the S and V block*/
    imark_y = 0.0; /* inner circle part of the marker for the S and V block*/
    omark_x = 0.0; /* outer circle part of the marker for the Hue Circle*/
    omark_y = 0.0; /* outer circle part of the marker for the Hue Circle*/
    fmark_x = 0.0; /* outer circle part of the marker for the Hue Circle*/
    fmark_y = 0.0; /* outer circle part of the marker for the Hue Circle*/

    s_val = 0.0;   /* S value */
    v_val = 0.0;   /* V value */
    
    this->location = location;
    CreateClickable();
}

float MINI(float x, float y, float z) {
    if ((x <= y) && (x <= z))
	return x;
    if ((y <= x) && (y <= z))
	return y;
    if ((z <= x) && (z <= y))
	return z;
}

float MAXI(float x, float y, float z) {
    if ((x >= y) && (x >= z))
	return x;
    if ((y >= x) && (y >= z))
	return y;
    if ((z >= x) && (z >= y))
	return z;
}

GColor gethue(float r, float g, float b)
{
    float h, s, v;
    float min, max, delta;
    min = MINI( r, g, b );
    max = MAXI( r, g, b );
    v = max;                               // v
    delta = max - min;
    if( max != 0 )
	s = delta / max;               // s
    else {
	// r = g = b = 0                // s = 0, v is undefined
	s = 0;
	h = -1;
	return (GColor(h, s, v));
    }
    if( r == max )
	h = ( g - b ) / delta;         // between yellow & magenta
    else if( g == max )
	h = 2 + ( b - r ) / delta;     // between cyan & yellow
    else
	h = 4 + ( r - g ) / delta;     // between magenta & cyan
    h *= 60;                               // degrees
    if( h < 0 )
	h += 360;

    return (GColor(h, s, v));
}

/* Conversion from HSV to RGB */
void IColorSelect::HSVtoRGB(GLfloat hue, GLfloat sat, GLfloat val,
			      unsigned char *red, unsigned char *green,
			      unsigned char *blue) {
  GLfloat f, p, q, t;

  if (sat == 0) {
      *red = (unsigned char) val * 255;
      *green = (unsigned char) val * 255;
      *blue = (unsigned char) val * 255;
  } else {
      while (hue < 0)
        hue += 360.0;
      while (hue >= 360)
        hue -= 360.0;

      hue /= 60;
      f = hue - (int) hue;
      p = val * (1 - sat);
      q = val * (1 - (sat * f));
      t = val * (1 - (sat * (1 - f)));

      switch ((int) hue) {
        case 0:
          *red = (unsigned char) (val * 255);
          *green = (unsigned char) (t * 255);
          *blue = (unsigned char) (p * 255);
          break;
        case 1:
          *red = (unsigned char) (q * 255);
          *green = (unsigned char) (val * 255);
          *blue = (unsigned char) (p * 255);
          break;
        case 2:
          *red = (unsigned char) (p * 255);
          *green = (unsigned char) (val * 255);
          *blue = (unsigned char) (t * 255);
          break;
        case 3:
          *red = (unsigned char) (p * 255);
          *green = (unsigned char) (q * 255);
          *blue = (unsigned char) (val * 255);
          break;
        case 4:
          *red = (unsigned char) (t * 255);
          *green = (unsigned char) (p * 255);
          *blue = (unsigned char) (val * 255);
          break;
        case 5:
          *red = (unsigned char) (val * 255);
          *green = (unsigned char) (p * 255);
          *blue = (unsigned char) (q * 255);
          break;
        default:
          break;
      }
    }
}

/*
 * draw
 *
 * This function performs the OpenGL drawing everything it is necessary.  This
 * usually happens when the window is first presented, or any time all or
 * part of the window is unhidden.
 */
int IColorSelect::execGL(void) {
    unsigned char red, green, blue;

    /* our selected color */
    //HSVtoRGB(ourhue, 1.0, 1.0, red, green, blue);
    HSVtoRGB(ourhue, 1.0, 1.0, &red, &green, &blue);


    /* draw the s and v box */

    /***************************
      White to Pure Color part
     ***************************/
    glBegin(GL_QUADS);

    /* white s=0, v=1 */
    HSVtoRGB(ourhue, 0.0, 1.0, &red, &green, &blue);
    GColor(red / 255.0, green / 255.0, blue / 255.0).execGL();
    glVertex3f((2 * OUTER_R) + 5.0 + location.getX(), location.getY(), 0.0);
    glVertex3f((2 * OUTER_R) + 5.0 + 5.0 + location.getX(), location.getY(), 0.0);

    /* Pure Color (Hue) s=1,v=1)*/
    HSVtoRGB(ourhue, 1.0, 1.0, &red, &green, &blue);
    GColor(red / 255.0, green / 255.0, blue / 255.0).execGL();
    glVertex3f((2 * OUTER_R) + 5.0 + 5.0 + location.getX(), -(2*OUTER_R) + location.getY(), 0.0);
    glVertex3f((2 * OUTER_R) + 5.0 + location.getX(), -(2*OUTER_R) + location.getY(), 0.0);

    glEnd();

    /***************************
      Pure Color to Black part
     ***************************/
    glBegin(GL_QUADS);

    /* Pure Color (Hue) s=1,v=1)*/
    HSVtoRGB(ourhue, 0.0, 0.0, &red, &green, &blue);
    GColor(red / 255.0, green / 255.0, blue / 255.0).execGL();
    glVertex3f((2 * OUTER_R) + 15.0 + location.getX(), location.getY(), 0.0);
    glVertex3f((2 * OUTER_R) + 15.0 + 5.0 + location.getX(), location.getY(), 0.0);

    /* Pure Color (Hue) s=1,v=1)*/
    HSVtoRGB(ourhue, 1.0, 1.0, &red, &green, &blue);
    GColor(red / 255.0, green / 255.0, blue / 255.0).execGL();
    glVertex3f((2 * OUTER_R) + 15.0 + 5.0 + location.getX(), -(2*OUTER_R) + location.getY(), 0.0);
    glVertex3f((2 * OUTER_R) + 15.0 + location.getX(), -(2*OUTER_R) + location.getY(), 0.0);

    glEnd();

    /* the circle */
    GColor(1.0, 0.0, 1.0).execGL();
    glBegin(GL_TRIANGLE_STRIP);
    int i;
    float theta;
    GLfloat x, y;
    for (i = 0; i < CIRCLE_PTS; i++) {
        theta = (4 * PI) * ((i / (float) CIRCLE_PTS) / 2);
	/* if even, inner radius */
	if (i % 2) {
	    x = INNER_R * sin(theta);
	    y = INNER_R * cos(theta);
	}
	/* else odd, outer radius */
	else {
	    x = OUTER_R * sin(theta);
	    y = OUTER_R * cos(theta);
	}
	float color_deg = (theta/20*PI) * 360;
	HSVtoRGB(color_deg, 1.0, 1.0, &red, &green, &blue);
	GColor(red / 255.0, green / 255.0, blue / 255.0).execGL();
	glVertex3f(location.getX() + OUTER_R + x, y - OUTER_R + location.getY(), 0.0);

    }
    glEnd();
    /* --------------
       Marker Circles
       ---------------*/

    /* inner circle that points to color positions (inner marker)*/
    GColor(1.0, 1.0, 1.0).execGL();
    glBegin(GL_TRIANGLE_STRIP);
    int a;
    float theta_1;
    GLfloat ax, ay;
     for (a = 0; a < 1000; a++) {
	theta_1 = (4 * PI) * ((a / (float) 1000 ) / 2);
	/* if even, inner radius */
	if (a % 2) {
	    //ax = 0.05 * sin(theta_1) + imark_x;
	    //ay = 0.05 * cos(theta_1) + imark_y;

	    ax = 1.0 * sin(theta_1) + imark_x;
	    ay = 1.0 * cos(theta_1) + imark_y;
	}
	/* else odd, outer radius */
	else {
	    //ax = 0.1 * sin(theta_1) + imark_x;
	    //ay = 0.1 * cos(theta_1) + imark_y;

	    ax = 0.5 * sin(theta_1) + imark_x;
	    ay = 0.5 * cos(theta_1) + imark_y;
	}
       	glVertex3f(ax,ay, 0.2);
    }
    glEnd();
    /* outer circle that points to color positions(outer marker)*/

    GColor(1.0, 1.0, 1.0).execGL();
    glBegin(GL_TRIANGLE_STRIP);
    for (a = 0; a < 1000; a++) {
	theta_1 = (4 * PI) * ((a / (float) 1000 ) / 2);
	/* if even, inner radius */
	if (a % 2) {
	    ax = 1.0 * sin(theta_1) + omark_x;
	    ay = 1.0 * cos(theta_1) + omark_y;
	}
	/* else odd, outer radius */
	else {
	    ax = 0.5 * sin(theta_1) + omark_x;
	    ay = 0.5 * cos(theta_1) + omark_y;
	}
	glVertex3f(ax,ay, 0.2);
    }
    glEnd();
    
    GColor(1.0, 1.0, 1.0).execGL();
    glBegin(GL_TRIANGLE_STRIP);
    for (a = 0; a < 1000; a++) {
	theta_1 = (4 * PI) * ((a / (float) 1000 ) / 2);
	/* if even, inner radius */
	if (a % 2) {
	    ax = 1.0 * sin(theta_1) + fmark_x;
	    ay = 1.0 * cos(theta_1) + fmark_y;
	}
	/* else odd, outer radius */
	else {
	    ax = 0.5 * sin(theta_1) + fmark_x;
	    ay = 0.5 * cos(theta_1) + fmark_y;
	}
	glVertex3f(ax,ay, 0.2);
    }
    glEnd();

    HSVtoRGB(ourhue, s_val, v_val, &red, &green, &blue);
    GColor(red / 255.0, green / 255.0, blue / 255.0).execGL();
    exportRed = red / 255.0;
    exportGreen = green / 255.0;
    exportBlue = blue / 255.0;
    
    return 1;
}

bool IColorSelect::clicked(GPoint pt, int ClickType) {

    float theta;
    bool hit = false;
    bool didhit = false;

    if (ClickType == GLUT_DOWN)
    {


    /* need this? (fy = -fy;) */
    //pt.setX(-pt.getX());

    GLfloat fx = pt.getX();
    GLfloat fy = pt.getY();


    GLdouble click_radius = sqrt(((fx - OUTER_R - location.getX())*(fx - OUTER_R - location.getX())) +
    				((fy + OUTER_R - location.getY())*(fy + OUTER_R - location.getY())));

    /* we have clickable ! */
    if ((click_radius > INNER_R)
	&& (click_radius < OUTER_R)) {

	if ((fy + OUTER_R - location.getY()) < 0) {
	    theta = PI - atan(-((fx - OUTER_R - location.getX())/
	    			(fy + OUTER_R - location.getY())));
	} else {
	    theta = atan((fx - OUTER_R - location.getX())/
	    		(fy + OUTER_R - location.getY()));
	}

	ourhue = (theta/20*PI) * 360;
	omark_x = fx;
	omark_y = fy;

	didhit = true;
    }/*clickable S and V box */
    hit = S_block->PtinRect(GPoint(fx,fy,0.0));
    float temp_y = fy;
	if (temp_y < 0.0)
		temp_y = temp_y * -1.0;
    if(hit)
    {
        imark_x = fx;
	imark_y = fy;
	//s_val = ((fy - location.getY())/((location.getY() + OUTER_R) - (location.getY())));
	s_val = 1.0 - ((fy - (location.getY() - (2*OUTER_R)))/((location.getY()) - (location.getY() - (2*OUTER_R))));
	//v_val = 1.0;
	didhit = true;
    }
    hit = V_block->PtinRect(GPoint(fx,fy,0.0));
    if(hit)
    {
	fmark_x = fx;
	fmark_y = fy;
	v_val = 1.0 - ((fy - (location.getY() - (2*OUTER_R)))/((location.getY()) - (location.getY() - (2*OUTER_R))));

	didhit = true;
    }
    }

    return didhit;
}

void IColorSelect::CreateClickable(void)
{

    S_block = new GRect(GPoint((2 * OUTER_R) + 5.0 + location.getX(), location.getY(), 0.0),
    			GPoint((2 * OUTER_R) + 10.0 + location.getX(), -(2* OUTER_R) + location.getY(),0.0));
    V_block = new GRect(GPoint((2 * OUTER_R) + 15.0 + location.getX(), location.getY(), 0.0),
    			GPoint((2 * OUTER_R)+ 20.0 + location.getX(), -(2 * OUTER_R) + location.getY(), 0.0));
}
void IColorSelect::dragged(GPoint pt)
{

    clicked(pt, GLUT_DOWN);
}

void IColorSelect::setcolor(GColor preview)
{
	unsigned char red, green, blue;

	red = (unsigned char) (preview.getRed() / 255);
	green = (unsigned char) (preview.getGreen() / 255);
	blue = (unsigned char) (preview.getBlue() / 255);


	GColor tempcolor;
	tempcolor = gethue(preview.getRed(), preview.getGreen(), preview.getBlue());

	ourhue = tempcolor.getRed();
	s_val = tempcolor.getGreen();
	v_val = tempcolor.getBlue();
	float theta;
	theta = (20/PI)*(ourhue/360);
	GLfloat length;
	GLfloat height;
	length = sin(theta) * (INNER_R + 2.5);
	height = cos(theta) * (INNER_R + 2.5);
	omark_x = length + location.getX() + OUTER_R;
	omark_y = height + location.getY() - OUTER_R;


		imark_y = location.getY() - ((2*OUTER_R)* tempcolor.getGreen());
		imark_x = (2 * OUTER_R) + 7.5 + location.getX();

		fmark_y = location.getY() - ((2*OUTER_R)* tempcolor.getBlue());
		fmark_x = (2 * OUTER_R) + 17.5 + location.getX();

	if(imark_y < (location.getY() - (2*OUTER_R)))
		imark_y = location.getY() - (2*OUTER_R);
		
	if(fmark_y < (location.getY() - (2*OUTER_R)))
		fmark_y = location.getY() - (2*OUTER_R);

	HSVtoRGB(ourhue, s_val, v_val, &red, &green, &blue);
}

GColor IColorSelect::getcolor()
{

	return (GColor(exportRed, exportGreen, exportBlue));
}
