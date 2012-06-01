/*
 * 3dfm - 3D File Manager
 * $Id: GColor.cpp,v 1.9 2003/04/09 03:39:09 adam Exp $
 */

/* system includes */
#include <math.h>

/* 3dfm includes */
#include <GColor.h>

float MIN(float x, float y, float z) {
    if ((x <= y) && (x <= z))
	return x;
    if ((y <= x) && (y <= z))
	return y;
    if ((z <= x) && (z <= y))
	return z;
}

float MAX(float x, float y, float z) {
    if ((x >= y) && (x >= z))
	return x;
    if ((y >= x) && (y >= z))
	return y;
    if ((z >= x) && (z >= y))
	return z;
}

/* Construct a new GColor with the default color (black).
 *
 * PARAMS
 *   none
 */
GColor::GColor(void) {
    this->red = 0.0;
    this->green = 0.0;
    this->blue = 0.0;
    this->alpha = 1.0;
}
// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//              if s == 0, then h = -1 (undefined)
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
    float min, max, delta;
    min = MIN( r, g, b );
    max = MAX( r, g, b );
    *v = max;                               // v
    delta = max - min;
    if( max != 0 )
	*s = delta / max;               // s
    else {
	// r = g = b = 0                // s = 0, v is undefined
	*s = 0;
	*h = -1;
	return;
    }
    if( r == max )
	*h = ( g - b ) / delta;         // between yellow & magenta
    else if( g == max )
	*h = 2 + ( b - r ) / delta;     // between cyan & yellow
    else
	*h = 4 + ( r - g ) / delta;     // between magenta & cyan
    *h *= 60;                               // degrees
    if( *h < 0 )
	*h += 360;
}
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
    int i;
    float f, p, q, t;
    if( s == 0 ) {
	// achromatic (grey)
	*r = *g = *b = v;
	return;
    }
    h /= 60;                        // sector 0 to 5
    //i = floor( h );
    i = (int) h;
    f = h - i;                      // factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );
    switch( i ) {
	case 0:
	    *r = v;
	    *g = t;
	    *b = p;
	    break;
	case 1:
	    *r = q;
	    *g = v;
	    *b = p;
	    break;
	case 2:
		*r = p;
	    *g = v;
	    *b = t;
	    break;
	case 3:
	    *r = p;
	    *g = q;
	    *b = v;
	    break;
	case 4:
	    *r = t;
	    *g = p;
	    *b = v;
	    break;
	default:                // case 5:
	    *r = v;
	    *g = p;
	    *b = q;
	    break;
    }
}

/* Construct a new GColor with the specified color components.  This
 * constructor does not do any parameter checking at this time.
 *
 * PARAMS
 *   red   - the red component
 *   green - the green component
 *   blue  - the blue component
 */
GColor::GColor(GLfloat red, GLfloat green, GLfloat blue) {
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = 1.0;
}

/* Construct a new GColor with the specified color components.  This
 * constructor does not do any parameter checking at this time.
 *
 * PARAMS
 *   red   - the red component
 *   green - the green component
 *   blue  - the blue component
 *   alpha - the alpha component
 */
GColor::GColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
}

/* Returns the red component of this color
 *
 * RETURN
 *   the red component
 */
GLfloat GColor::getRed(void) {
    return red;
}

/* Returns the green component of this color
 *
 * RETURN
 *   the green component
 */
GLfloat GColor::getGreen(void) {
    return green;
}

/* Returns the blue component of this color
 *
 * RETURN
 *   the blue component
 */
GLfloat GColor::getBlue(void) {
    return blue;
}

/* Returns the alpha component of this color
 *
 * RETURN
 *   the alpha component
 */
GLfloat GColor::getAlpha(void) {
    return alpha;
}

/* Sets the alpha blending factor for this color
 *
 * PARAMS
 *   a - the new alpha value
 */
void GColor::setAlpha(GLfloat a) {
    alpha = a;
}

/* Execute the GL for this object.  This will set the foreground color
 * that of the OpenGL engine to color this object represents.
 *
 * POST-CONDITION
 *   The GL engine state will have this color set after this function.
 */
void GColor::execGL(void) {
    glColor4f(red, green, blue, alpha);
}

/* Exectue the GL for this object and add the same color for the material
 * properties. 
 */
void GColor::execFullGL(void) {
    execGL();
    GLfloat matAmb[] = {red, green, blue, alpha};
    GLfloat matSpec[] = {0.5, 0.5, 0.5, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmb);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
}

GColor GColor::operator+(GLfloat amt) {
    
    GLfloat h, s, v;
    GLfloat change = 0;
    RGBtoHSV(red, green, blue, &h, &s, &v);
    if (v > 0.75)
    {
	if (alpha < 0.4)
	{
	    alpha = 0.7;
	    change = 0.5;
	}
	if (alpha > 0.4)
	{
	    //alpha;
	    change = -0.5;
	}
    }
    else if (v > 0.50)
	{ 
	    if (alpha < 0.4)
	    {
		alpha = 0.7;
		//change = 0.50;
	    }		
	    else
	    {
		change = 0.25;
		//alpha = 0.75;
	    }
	}
    else
	if (alpha < 0.4)
	{
	    alpha = 0.7;
	    change = 0.5;
	}
	else 
	    change = 0.5;
    
    HSVtoRGB(&red, &green, &blue, h, s, v);

    red = red + change;
    green = green + change;
    blue = blue + change;

    if (red > 1.0)
	red = 1.0;
    if (blue > 1.0)
	blue = 1.0;
    if (green > 1.0)
	green = 1.0;

    if (red < 0.0)
	red = 0.0;
    if (blue < 0.0)
	blue = 0.0;
    if (green < 0.0)
	green = 0.0;

 	    
    return GColor(red, green, blue, alpha);
}
