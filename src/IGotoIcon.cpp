/*
 * 3dfm - 3D File Manager
 * $Id: IGotoIcon.cpp,v 1.6 2003/04/10 03:26:19 kristopher Exp $
 */

/*
 * IGotoIcon
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <IGotoIcon.h>

/* design definitions */
#define ZDIR 0.1
#define CHAR_WIDTH 2
#define CHAR_HEIGHT 3


/* Creates the IGotoIcon object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the slider
 *   size - the demensions of the button
 *   paneColor - the color of the button
 *
 */
IGotoIcon::IGotoIcon(GPoint location, GPoint size, GColor paneColor)
{
    //Pull in all values...
    this->size = size;
    this->location = location;
    this->paneColor = paneColor;
    arrowColor = GColor(1.0, 0.1, 0.1, 0.8);

    //Set moues over and capture flags
    Capture = false;
    Over = false;

    //Setup animation variables
    direction = true;
    aalpha = 0.0;

    CreateClickable();
}

/* Take the passed in string and prints it out to the screen
 *
 * PARAMS
 *   *font - the font that text is to be displayed in
 *   *str - the string to be displayed
 *
 */
static void ourPrintString(void *font, char *str)
{
   int i,l=strlen(str);

   for(i=0;i<l;i++)
      glutBitmapCharacter(font,*str++);
}

/* Draw the physical icon object
 *
 * PARAMS
 *
 *
 */
void IGotoIcon::execGL(void) {

    //Set the panecolor of the object to the active color
    paneColor.execGL();


    if (Capture)
    {
    //If we are clicking on the box make the background a transparent white
    glColor4f(1.0, 1.0, 1.0, 0.4);
    }
    //Creating the back pane
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY(), ZDIR);
    glEnd();


    //If mouse over make it pulsate
    if (Over)
    {
    if ((aalpha < (arrowColor.getAlpha())) && direction)
    		{
    		aalpha = aalpha + 0.015;
		}
			else if ((aalpha <= (arrowColor.getAlpha()) && (!direction)))
				{
					aalpha = aalpha - 0.015;
					if (aalpha < 0.0)
						{
						direction = true;
						aalpha = 0.0;
						}
				}
				else if ((aalpha >= (arrowColor.getAlpha()) && (direction)))
					{
					aalpha = arrowColor.getAlpha();
					direction = false;
					}
	glColor4f(arrowColor.getRed(), arrowColor.getGreen(), arrowColor.getBlue(), aalpha);
    }
    else
    {
    	//If not over it then return to normal red arrow color
        glColor4f(1.0, 0.1, 0.1, 0.8);
	aalpha = arrowColor.getAlpha();
    }

    //Draw the arrow
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 3.0, ZDIR);
    glVertex3f(location.getX(), location.getY() - 7.0, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 7.0, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 3.0, ZDIR);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 3.0, ZDIR - 3.0);
    glVertex3f(location.getX(), location.getY() - 7.0, ZDIR - 3.0);
    glVertex3f(location.getX() + 6.0, location.getY() - 7.0, ZDIR - 3.0);
    glVertex3f(location.getX() + 6.0, location.getY() - 3.0, ZDIR - 3.0);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 3.0, ZDIR - 3.0);
    glVertex3f(location.getX(), location.getY() - 3.0, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 3.0, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 3.0, ZDIR - 3.0);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 7.0, ZDIR - 3.0);
    glVertex3f(location.getX(), location.getY() - 7.0, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 7.0, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 7.0, ZDIR - 3.0);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 3.0, ZDIR - 3.0);
    glVertex3f(location.getX(), location.getY() - 7.0, ZDIR - 3.0);
    glVertex3f(location.getX(), location.getY() - 7.0, ZDIR);
    glVertex3f(location.getX(), location.getY() - 3.0, ZDIR);
    glEnd();


    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 6.0, location.getY() - 3.0, ZDIR - 3.0);
    glVertex3f(location.getX() + 6.0, location.getY() - 7.0, ZDIR - 3.0);
    glVertex3f(location.getX() + 6.0, location.getY() - 7.0, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 3.0, ZDIR);
    glEnd();


    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 6.0, location.getY() - 0.5, ZDIR - 3.0);
    glVertex3f(location.getX() + 6.0, location.getY() - 9.5, ZDIR - 3.0);
    glVertex3f(location.getX() + 6.0, location.getY() - 9.5, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 0.5, ZDIR);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 6.0, location.getY() - 9.5, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 9.5, ZDIR - 3.0);
    glVertex3f(location.getX() + 9.5, location.getY() - 5.0, ZDIR - 3.0);
    glVertex3f(location.getX() + 9.5, location.getY() - 5.0, ZDIR);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 6.0, location.getY() - 0.5, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 0.5, ZDIR - 3.0);
    glVertex3f(location.getX() + 9.5, location.getY() - 5.0, ZDIR - 3.0);
    glVertex3f(location.getX() + 9.5, location.getY() - 5.0, ZDIR);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(location.getX() + 6.0, location.getY() - 0.5, ZDIR);
    glVertex3f(location.getX() + 6.0, location.getY() - 9.5, ZDIR);
    glVertex3f(location.getX() + 9.5, location.getY() - 5.0, ZDIR);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(location.getX() + 6.0, location.getY() - 0.5, ZDIR - 3.0);
    glVertex3f(location.getX() + 6.0, location.getY() - 9.5, ZDIR - 3.0);
    glVertex3f(location.getX() + 9.5, location.getY() - 5.0, ZDIR - 3.0);
    glEnd();

    //Push matrix to draw text
    glPushMatrix();
    //Move the main coordinates to the top left corner or the box
    glTranslatef(location.getX(), location.getY(), ZDIR);

    //Create white text color
    glColor4f(1.0, 1.0, 1.0, 0.8);

    //Set text printing location and print the number
    glRasterPos3f(8.0, -9.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "g");

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();
}

/* Create a GRect to cover the button
 *
 * PARAMS
 *   none
 *
 */
void IGotoIcon::CreateClickable()
{
	Button = new GRect(GPoint(location.getX(), location.getY(), ZDIR),
			   GPoint(location.getX() + size.getX(),
			          location.getY() - size.getY() -1,
			          ZDIR));
}

/* Check if the button was hit and if so was it down or up
 *
 * PARAMS
 *   pt - the point where the mouse is
 *   ClickType - what type of click (up or down)
 *
 */
void IGotoIcon::clicked(GPoint pt, int ClickType)
{
    bool hitButton;
    //Check for a hit
    hitButton = Button->PtinRect(pt);

    //Have we hit the button
    if (hitButton)
    {

      	   //Check what type of message (up or down)
  	   //The mouse is clicked
	   if (ClickType == GLUT_DOWN)
	   {
	   	Capture = true;
		Clicked = false;
           }
	       //Was the mouse button lifted?
		else if (Capture && (ClickType == GLUT_UP))
			{
			Capture = false;
			Clicked = true;
			}
   }else
      {  //If we did not hit the button be sure to remove
         //the capture
         Capture = false;
         Clicked = false;
      }
}

void IGotoIcon::moved(GPoint pt)
{
    bool hitButton;
    //Check for a hit
    hitButton = Button->PtinRect(pt);
    //If we are over it perform animation
    if (hitButton)
    {
    	Over = true;
    }else
    	Over = false;
}
