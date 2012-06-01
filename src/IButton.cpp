/*
 * 3dfm - 3D File Manager
 * $Id: IButton.cpp,v 1.6 2003/04/17 21:41:13 kristopher Exp $
 */

/*
 * IButton
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <IButton.h>

/* design definitions */
#define ZDIR 0.1
#define CHAR_WIDTH 2
#define CHAR_HEIGHT 3


/* Creates the IButton object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the slider
 *   size - the demensions of the button
 *   paneColor - the color of the button
 *   Caption - what the button's label is
 *
 */
IButton::IButton(GPoint location, GPoint size, GColor paneColor, char* Caption)
{
    //Pull in all values...
    this->size = size;
    this->location = location;
    this->Caption = (char*)malloc(sizeof (char) * strlen(Caption) + 1);
    this->Caption = Caption;
    this->paneColor = paneColor;

    Capture = false;

    CreateClickable();
}

IButton::~IButton()
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
void IButton::execGL(void) {

    //Set the panecolor of the object to the active color
    paneColor.execGL();

    //Creating the button
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY(), ZDIR);
    glEnd();

    //If we are clicking on the button create the white glaze
    if (Capture)
    {
       glColor4f(1.0, 1.0, 1.0, 0.3);
       glBegin(GL_QUADS);
       glVertex3f(location.getX() + 1.0, location.getY() - 1.0, ZDIR);
       glVertex3f(location.getX() + 1.0, location.getY() - size.getY(), ZDIR);
       glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(), ZDIR);
       glVertex3f(location.getX() + size.getX(), location.getY() - 1.0, ZDIR);
       glEnd();
    }

    //Calculate how many characters can fit on the button and if the string
    //if too big for the button do not display anything at all
    float AvailableRoom;
    int Characters;
    int length;
    char* tempstring;
    length = strlen(Caption);
    AvailableRoom = size.getX() / CHAR_WIDTH;
    Characters = (int)AvailableRoom - 1;
    tempstring = (char*)malloc(sizeof (char) * Characters + 1);
    if (length > Characters)
    {
       Caption = (char*)malloc(sizeof(char) + 1);
       Caption[0] = 0;
    }

    free (tempstring);

    //Center the text inside of the button
    GLfloat CenterHorz, CenterVert;
    CenterHorz = size.getX() / 2.0;
    CenterVert = size.getY() / 2.0;

    //Now calculate the shift distance for the string
    //Note:  Mod (%) produces the remainder
    int EvenOdd, Shift;
    EvenOdd = length % 2;
    Shift = length / 2;
    //If there is an odd number...
    if (EvenOdd > 0)
        //Shift half plus half of a letter
    	CenterHorz = CenterHorz - (CHAR_WIDTH * Shift)- 1.0;
	//Otherwise...
	else
	    //Shift half of the letters
	    CenterHorz = CenterHorz - (CHAR_WIDTH * Shift);

    CenterVert = CenterVert + (CHAR_HEIGHT / 2);

    //Need to pust matrix to draw text
    glPushMatrix();
    glTranslatef(location.getX(), location.getY(), ZDIR);
    //If we are capture, make text black
    if (Capture)
        glColor4f(0.0, 0.0, 0.0, 0.75);
	else
	//Otherwise use the standard white
	glColor4f(1.0, 1.0, 1.0, 0.75);

    //Set position for text
    glRasterPos3f(CenterHorz, -CenterVert, ZDIR);
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

void IButton::CreateClickable()
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
void IButton::clicked(GPoint pt, int ClickType)
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

/* Change the caption of the button
 *
 * PARAMS
 *   NewCaption - the new caption for the button
 *
 */
void IButton::SetCaption(char* NewCaption)
{
	int length = strlen(NewCaption);
	Caption = (char*)malloc(sizeof(char) * length + 1);
	strcpy (Caption, NewCaption);
}
