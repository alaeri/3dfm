/*
 * 3dfm - 3D File Manager
 * $Id: IPrefsIcon.cpp,v 1.6 2003/04/10 03:26:20 kristopher Exp $
 */

/*
 * IPrefsIcon
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <IPrefsIcon.h>

/* design definitions */
#define ZDIR 0.1
#define CHAR_WIDTH 2
#define CHAR_HEIGHT 3


/* Creates the IPrefsIcon object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the slider
 *   size - the demensions of the button
 *   paneColor - the color of the button
 *
 */
IPrefsIcon::IPrefsIcon(GPoint location, GPoint size, GColor paneColor)
{
    //Pull in all values...
    this->size = size;
    this->location = location;
    this->paneColor = paneColor;

    //Set capture flags
    Capture = false;
    Over = false;
    
    //Set animation flags
    slidertop = 1.0;
    sliderbottom = 3.0;
    slidertopdir = 1;
    sliderbotdir = 1;

    //Create clickable objects
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
void IPrefsIcon::execGL(void) {

    //Set the panecolor of the object to the active color
    paneColor.execGL();

    //If we are over the object make the sliders slide...
    if (Over)
    {
    	if (slidertopdir == 1)
	{
		slidertop += 0.05;
		if (slidertop >= 4.0)
		{
		slidertopdir = -1;
		}
	}
	if (slidertopdir == -1)
	{
		slidertop -= 0.05;
		if (slidertop <= 0.0)
		{
		slidertopdir = 1;
		}
	}
	if (sliderbotdir == 1)
	{
		sliderbottom += 0.05;
		if (sliderbottom >= 4.0)
		{
		sliderbotdir = -1;
		}
	}
	if (sliderbotdir == -1)
	{
		sliderbottom -= 0.05;
		if (sliderbottom <= 0.0)
		{
		sliderbotdir = 1;
		}
	}


    }

    //If we are captured then make the background white and transparent
    if (Capture)
    {
    glColor4f(1.0, 1.0, 1.0, 0.4);
    }
    //Creating the back pane
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY(), ZDIR);
    glEnd();

    //Draw top mini slider device
    glColor4f(0.2, 0.1, 1.0, 0.8);
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 1.0, ZDIR + 0.1);
    glVertex3f(location.getX(), location.getY() - 2.5, ZDIR + 0.1);
    glVertex3f(location.getX() + 5.0, location.getY() - 2.5, ZDIR + 0.1);
    glVertex3f(location.getX() + 5.0, location.getY() - 1.0, ZDIR + 0.1);
    glEnd();

    //Draw top mini slider
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + slidertop, location.getY() - 0.5, ZDIR + 0.2);
    glVertex3f(location.getX() + slidertop, location.getY() - 3.0, ZDIR + 0.2);
    glVertex3f(location.getX() + slidertop + 1.0, location.getY() - 3.0, ZDIR + 0.2);
    glVertex3f(location.getX() + slidertop + 1.0, location.getY() - 0.5, ZDIR + 0.2);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(location.getX() + slidertop, location.getY() - 3.0, ZDIR + 0.2);
    glVertex3f(location.getX() + slidertop + 0.5, location.getY() - 3.5, ZDIR + 0.2);
    glVertex3f(location.getX() + slidertop + 1.0, location.getY() - 3.0, ZDIR + 0.2);
    glEnd();

    //Draw bottom mini slider device
    glColor4f(1.0, 0.1, 0.2, 0.8);
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 6.5, ZDIR + 0.1);
    glVertex3f(location.getX(), location.getY() - 8.0, ZDIR + 0.1);
    glVertex3f(location.getX() + 5.0, location.getY() - 8.0, ZDIR + 0.1);
    glVertex3f(location.getX() + 5.0, location.getY() - 6.5, ZDIR + 0.1);
    glEnd();

    //Draw bottom mini slider
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + sliderbottom, location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + sliderbottom, location.getY() - 8.5, ZDIR + 0.2);
    glVertex3f(location.getX() + sliderbottom + 1.0, location.getY() - 8.5, ZDIR + 0.2);
    glVertex3f(location.getX() + sliderbottom + 1.0, location.getY() - 6.0, ZDIR + 0.2);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(location.getX() + sliderbottom, location.getY() - 8.5, ZDIR + 0.2);
    glVertex3f(location.getX() + sliderbottom + 0.5, location.getY() - 9.0, ZDIR + 0.2);
    glVertex3f(location.getX() + sliderbottom + 1.0, location.getY() - 8.5, ZDIR + 0.2);
    glEnd();

    //Draw screw driver thingy
    glColor4f(0.3, 1.0, 0.2, 0.6);
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 6.0, location.getY() - 0.5, ZDIR + 0.05);
    glVertex3f(location.getX() + 6.0, location.getY() - 5.0, ZDIR + 0.05);
    glVertex3f(location.getX() + 8.0, location.getY() - 5.0, ZDIR + 0.05);
    glVertex3f(location.getX() + 8.0, location.getY() - 0.5, ZDIR + 0.05);
    glEnd();

    glColor4f(0.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(location.getX() + 6.5, location.getY() - 0.5, ZDIR + 0.05);
    glVertex3f(location.getX() + 6.5, location.getY() - 5.0, ZDIR + 0.05);
    glEnd();

    glColor4f(0.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(location.getX() + 7.5, location.getY() - 0.5, ZDIR + 0.05);
    glVertex3f(location.getX() + 7.5, location.getY() - 5.0, ZDIR + 0.05);
    glEnd();

    glColor4f(0.486275, 0.533333, 0.533333, 0.8);
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 6.5, location.getY() - 5.0, ZDIR + 0.05);
    glVertex3f(location.getX() + 6.5, location.getY() - size.getY() + 1.5, ZDIR + 0.05);
    glVertex3f(location.getX() + 7.5, location.getY() - size.getY() + 1.5, ZDIR + 0.05);
    glVertex3f(location.getX() + 7.5, location.getY() - 5.0, ZDIR + 0.05);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(location.getX() + 6.5, location.getY() - size.getY() + 1.5, ZDIR + 0.2);
    glVertex3f(location.getX() + 7.0, location.getY() - size.getY() + 0.5, ZDIR + 0.2);
    glVertex3f(location.getX() + 7.5, location.getY() - size.getY() + 1.5, ZDIR + 0.2);
    glEnd();

    //Push matrix to draw text
    glPushMatrix();
    //Move the main coordinates to the top left corner or the box
    glTranslatef(location.getX(), location.getY(), ZDIR);

    //Create white text color
    glColor4f(1.0, 1.0, 1.0, 0.8);

    //Set text printing location and print the number
    glRasterPos3f(8.0, -9.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "p");

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();
}

/* Create a GRect to cover the button
 *
 * PARAMS
 *   none
 *
 */

void IPrefsIcon::CreateClickable()
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
void IPrefsIcon::clicked(GPoint pt, int ClickType)
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

/* Checks if the mouse is over the button
 *
 * PARAMS
 *   pt - the point where the mouse is
 *
 */
void IPrefsIcon::moved(GPoint pt)
{
    bool hitButton;
    //Check for a hit
    hitButton = Button->PtinRect(pt);
    //If we are over the button animate
    if (hitButton)
    {
    	Over = true;
    }
    else
    	Over = false;
}
