/*
 * 3dfm - 3D File Manager
 * $Id: GRect.cpp,v 1.5 2003/04/08 17:54:08 adam Exp $
 */

/*
 * GRect
 */

/* system includes */
#include <GL/glut.h>

/* 3dfm includes */
#include <GRect.h>

/* Default constructor for the GRect object.  This will not usually
 * be called but satisfies the compiler, or if the user would like
 * to make an empty GRect object this constuctor can be used.
 *
 * PARAMS
 *   none
 *
 */
GRect::GRect(void)
{
   topleft = GPoint(0.0, 0.0, 0.0);
   topright = GPoint(0.0, 0.0, 0.0);
   bottomleft = GPoint(0.0, 0.0, 0.0);
   bottomright = GPoint(0.0, 0.0, 0.0);
}

/* Constructs a GRect object based on the upper left and bottom right
 * coordinates passed to it.  The constructor does check for the validity
 * of the rectangle, but only outputs and error message.
 *
 * PARAMS
 *   none
 *
 */
GRect::GRect(GPoint upperleft, GPoint lowerright)
{
    //Check to make sure the rectangle coordinates are valid
    if ((upperleft.getX() >= lowerright.getX()) && (upperleft.getY() <= lowerright.getY()))
    	fprintf (stderr, "Incorrectly designed rectangle!\t %f, %f, %f, %f\n",
	upperleft.getX(), upperleft.getY(), lowerright.getX(), lowerright.getY());

    //Set the four corners of the rectangle
    topleft = upperleft;

    topright = upperleft;
    topright.setX(lowerright.getX());

    bottomleft = upperleft;
    bottomleft.setY(lowerright.getY());
    bottomright = lowerright;
}

/* Default destructor of the GRect object
 *
 * PARAMS
 *   none
 *
 */
GRect::~GRect(void)
{
    // nothing here
}

/* Returns the top left corner of the rectangle
 *
 * PARAMS
 *   none
 *
 */
GPoint GRect::Gettopleft()
{
    return topleft;
}

/* Returns the top right corner of the rectangle
 *
 * PARAMS
 *   none
 *
 */
GPoint GRect::Gettopright()
{
    return topright;
}

/* Returns the bottom left corner of the rectangle
 *
 * PARAMS
 *   none
 *
 */
GPoint GRect::Getbottomleft()
{
    return bottomleft;
}

/* Returns the bottom right corner of the rectangle
 *
 * PARAMS
 *   none
 *
 */
GPoint GRect::Getbottomright()
{
    return bottomright;
}

/* Returns the height of the rectangle
 *
 * PARAMS
 *   none
 *
 */
GLfloat GRect::Getheight()
{
     GLfloat height;
     height = bottomleft.getY() - topleft.getY();
     return height;
}

/* Returns the width of the rectangle
 *
 * PARAMS
 *   none
 *
 */
GLfloat GRect::Getwidth()
{
     GLfloat width;
     width = bottomright.getX() - topleft.getX();
     return width;
}

/* Determines whether a point in inside the rectangle or not
 *
 * PARAMS
 *   point - the point to be checked
 *
 * RETURNS
 *   bool - true if the point is in, otherwise false
 */
bool GRect::PtinRect(GPoint point)
{
     //Check if point in is the Rectangle
     if ((point.getX() <= bottomright.getX()) && (point.getX() >= topleft.getX()) &&
        (point.getY() >= bottomright.getY()) && (point.getY() <= topleft.getY()))
	return true;
	else
	return false;
}

void GRect::ReSize(GPoint upperleft, GPoint lowerright)
{
    if ((upperleft.getX() >= lowerright.getX()) && (upperleft.getY() <= lowerright.getY()))
    	fprintf (stderr, "Incorrectly designed rectangle!\t %f, %f, %f, %f\n",
	upperleft.getX(), upperleft.getY(), lowerright.getX(), lowerright.getY());

    //Set the four corners of the rectangle
    topleft = upperleft;

    topright = upperleft;
    topright.setX(lowerright.getX());

    bottomleft = upperleft;
    bottomleft.setY(lowerright.getY());
    bottomright = lowerright;
}

