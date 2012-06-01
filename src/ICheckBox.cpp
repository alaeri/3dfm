/*
 * 3dfm - 3D File Manager
 * $Id: ICheckBox.cpp,v 1.5 2003/04/17 21:15:20 kristopher Exp $
 */

/*
 * ICheckBox
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <ICheckBox.h>

/* design definitions */
#define ZDIR 0.1
#define CHAR_WIDTH 2
#define CHAR_HEIGHT 3


/* Creates the ICheckBox object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the slider
 *   paneColor - the color of the check box border
 *   Caption - the label next to the checkbox
 *   Checked - whether the checkbox is checked or not
 *
 */
ICheckBox::ICheckBox(GPoint location, GColor paneColor, char* Caption, bool Checked)
{
    //Pull in all values...
    this->location = location;
    this->Caption = (char*) malloc(sizeof (char) * strlen(Caption) + 1);
    this->Caption = Caption;
    this->paneColor = paneColor;
    this->Checked = Checked;

    //Set initial capture
    Capture = false;

    CreateClickable();
}


ICheckBox::~ICheckBox(void)
{
	free (Caption);
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

/* Draw the physical slider object
 *
 * PARAMS
 *
 *
 */
void ICheckBox::execGL(void) {
    //Set the panecolor of the object to the active color
    paneColor.execGL();

    //Creating the empty checkbox
    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY() - 3.0, ZDIR);
    glVertex3f(location.getX() + 3.0, location.getY() - 3.0, ZDIR);
    glVertex3f(location.getX() + 3.0, location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glEnd();

    //If we are clicking on the button create the white glaze
    if (Checked)
    {
       glColor4f(1.0, 1.0, 1.0, 0.6);
       glBegin(GL_QUADS);
       glVertex3f(location.getX() + 0.2, location.getY() - 0.2, ZDIR);
       glVertex3f(location.getX() + 0.2, location.getY() - 2.8, ZDIR);
       glVertex3f(location.getX() + 2.8, location.getY() - 2.8, ZDIR);
       glVertex3f(location.getX() + 2.8, location.getY() - 0.2, ZDIR);
       glEnd();
    }

    //Need to pust matrix to draw text
    glPushMatrix();
    glTranslatef(location.getX(), location.getY(), ZDIR);
    //If we are capture, make text black

    glColor4f(1.0, 1.0, 1.0, 0.75);

    //Set position for text
    glRasterPos3f(4.0, -2.5, ZDIR);
    ourPrintString(GLUT_BITMAP_8_BY_13, Caption);

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();
}

/* Create a GRect to cover the button
 *
 * PARAMS
 *   none
 *
 */
void ICheckBox::CreateClickable()
{
	box = new GRect(GPoint(location.getX(), location.getY(), ZDIR),
			GPoint(location.getX() + 3.0, location.getY() - 3.0,
			          ZDIR));
}

/* Check if the button was hit and if so was it down or up
 *
 * PARAMS
 *   pt - the point where the mouse is
 *   ClickType - what type of click (up or down)
 *
 */
void ICheckBox::clicked(GPoint pt, int ClickType)
{
    bool hitbox;
    //Check for a hit
    hitbox = box->PtinRect(pt);

    //Have we hit the button
    if (hitbox)
    {

      	   //Check what type of message (up or down)
  	   //The mouse is clicked
	   if (ClickType == GLUT_DOWN)
	   {
	   	Capture = true;
	   	Checked = !Checked;
           }
	       //Was the mouse button lifted?
		else if (Capture && (ClickType == GLUT_UP))
			{
				Capture = false;
				//Checked = !Checked;
			}
   }else
      {  //If we did not hit the button be sure to remove
         //the capture
	     if(Capture)
	     {
	     Capture = false;
             Checked = !Checked;
	     }
	     Capture = false;
      }
}

/* Change the caption of the button
 *
 * PARAMS
 *   NewCaption - the new caption for the button
 *
 */
void ICheckBox::SetCaption(char* NewCaption)
{
	int length = strlen(NewCaption);
	Caption = (char*) malloc(sizeof (char) * length + 1);
	strcpy (Caption, NewCaption);
}
