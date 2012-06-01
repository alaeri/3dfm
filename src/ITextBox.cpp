/*
 * 3dfm - 3D File Manager
 * $Id: ITextBox.cpp,v 1.11 2003/04/17 21:41:14 kristopher Exp $
 */

/*
 * ITextBox
 */

/* system includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <ITextBox.h>
#include <GRect.h>

#define ZDIR 0.1
#define CHAR_WIDTH 2
#define CHAR_HEIGHT 4

/* Construct a new ITextBox from the passed in values.
 * constructor does not do any parameter checking at this time.
 *
 * PARAMS
 *   location - the top left corner of the box
 *   size - the size in the X, Y and Z directions
 *   paneColor - the color of the pane
 *   number - should the textbox only accept numbers?
 */
ITextBox::ITextBox(GPoint location, GPoint size, GColor paneColor, bool number)
{
    this->location = location;
    this->size = size;
    this->paneColor = paneColor;
    this->number = number;

    //Determine how many characters can fit in the box
    //Also set the current string to blank
    CharacterMax = ((int)size.getX() / 2) - 1;
    string = (char *) malloc(sizeof(char)*CharacterMax + 1);
    string[0] = 0;

    //Set animation flags
    FocusLock = false;
    direction = true;
    aalpha = 0.0;
    redraw = false;

    CreateClickable();
}

/* Destroys the ITextBox item, and data structure
 *
 * PARAMS
 *   none
 *
 */
ITextBox::~ITextBox(void)
{
   free (string);
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

/* Draw the actual ITextBox-box object on the screen
 *
 * PARAMS
 *   none
 *
 */
void ITextBox::execGL(void) {

    //Set the paneColor to the active color
    paneColor.execGL();

    //Draw the main pane
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), location.getZ());
    glVertex3f(location.getX() + size.getX(),
	       location.getY(), location.getZ());
    glVertex3f(location.getX() + size.getX(),
	       location.getY() - size.getY(),
	       location.getZ());
    glVertex3f(location.getX(), location.getY() - size.getY(),
	       location.getZ());

    glEnd();

    glColor4f(0.0, 0.0, 0.0, 1.0);
    //Draw text pane
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 0.5, location.getY() - 0.5, location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 0.5,
	       location.getY() - 0.5, location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 0.5,
	       location.getY() - size.getY() + 0.5,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + 0.5, location.getY() - size.getY() + 0.5,
	       location.getZ() + 0.1);

    glEnd();



    //Drawing the text

    //Need to pust matrix to draw text
    glPushMatrix();
    //Move the main coordinates to the top left corner or the box
    glTranslatef(location.getX(), location.getY(), ZDIR);

    //Create white text color
    glColor4f(1.0, 1.0, 1.0, 0.5);

    //Set text printing location and print the number
    glRasterPos3f(1.0, -4.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, string);

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();

    //If we are focued on the TextBox draw the carot
    if (CreateCarot)
    {
    glColor4f(1.0, 1.0, 1.0, aalpha);
    if (aalpha < 0.7 && direction)
    		{
    		aalpha = aalpha + 0.015;
		}
			else if ((aalpha <= 0.7 && (!direction)))
				{
					aalpha = aalpha - 0.015;
					if (aalpha < 0.0)
						{
						direction = true;
						aalpha = 0.0;
						}
				}
				else if ((aalpha >= 0.7 && (direction)))
					{
					aalpha = 0.7;
					direction = false;
					}

    	int length = strlen(string);
	glBegin(GL_QUADS);
    	glVertex3f(location.getX() + 1.0 + (length * CHAR_WIDTH),
        	   location.getY() - 1.0,
	           location.getZ() + 0.1);
    	glVertex3f(location.getX() + 1.0 + (length * CHAR_WIDTH),
	           location.getY() - 1.0 - CHAR_HEIGHT,
	           location.getZ() + 0.1);
    	glVertex3f(location.getX() + 1.5 + (length * CHAR_WIDTH),
	           location.getY() - 1.0 - CHAR_HEIGHT,
	           location.getZ() + 0.1);
    	glVertex3f(location.getX() + 1.5 + (length * CHAR_WIDTH),
                   location.getY() - 1.0,
	           location.getZ() + 0.1);
    	glEnd();

	redraw = true;
    }
}

/* Set the text within the box
 *
 * PARAMS
 *   *NewString - the string to be put into the textbox
 *
 */
void ITextBox::SetText(char* NewString)
{
	Clear();
	int length = strlen(NewString);
	string = (char*)malloc(length);
	strcpy (string, NewString);
}

/* Create the clickable objects
 *
 * PARAMS
 *   none
 *
 */
void ITextBox::CreateClickable()
{
	Box = new GRect(GPoint(location.getX() + 0.5, 
	                       location.getY() - 0.5, 
			       location.getZ() + 0.1),
		        GPoint(location.getX() + size.getX() - 0.5, 
			       location.getY() - size.getY() + 0.0, 
			       location.getZ() + 0.1));
}

/* Check if the button was hit and if so was it down or up
 *
 * PARAMS
 *   pt - the point where the mouse is
 *   ClickType - what type of click (up or down)
 *
 */
void ITextBox::clicked(GPoint pt, int ClickType)
{
   //Only down clicks matter
   if (ClickType == GLUT_DOWN)
   {
   	bool hit;

	//Did we hit the box
	hit = Box->PtinRect(pt);
	if (hit)
		{
		//If we hit it set the locks
		FocusLock = true;
		CreateCarot = true;
		}else
		{
			//If not release the locks
			FocusLock = false;
	    		CreateCarot = false;
		}
   }
}

/* Add the appropriate keys to the string
 *
 * PARAMS
 *   key - the key hit
 *
 */
void ITextBox::SendKeys(unsigned char key)
{
   //Check is any of the special keys are being held down
   int specialKey = glutGetModifiers();

   //If we have focus, and Ctrl and Alt are not being pressed continue
   if (FocusLock && (specialKey != GLUT_ACTIVE_CTRL) && (specialKey != GLUT_ACTIVE_ALT))
   {
	/*
	 * To easily add the character to the string, we need to convert
	 * it from a char to a char* and just worry about the first
	 * character of the new string
	 */
	char* keycode;

	//keycode = (char*)malloc(2);
	keycode = (char*)malloc(sizeof(char)*2 + 1);
	keycode[0] = key;

	//If we are only accepting numbers then only add when the following
	//keys are hit
	if (number)
	{
		switch (key) {
		case '1':
		    AddCharacter(keycode);
		    redraw = 1;
	    	break;
		case '2':
	    	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case '3':
            	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case '4':
            	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case '5':
            	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case '6':
            	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case '7':
            	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case '8':
            	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case '9':
            	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case '0':
            	  AddCharacter(keycode);
	    	  redraw = 1;
	    	break;
		case 8:
	    	  RemoveCharacter();
	    	  redraw = 1;
	    	break;

		default:
	    	break;
		}
    	}else
	   {
	   	/*
		 * Otherwise we are accepting everyting except the following
		 * keys
		 * Note:  We ignore backspace (8) here because we do not want
		 *        to add its ASCII code
		 */
		if ((key != 127) && (key != 8) && (key != 27) &&
		   (key != 9) && (key != 13))
   		{
   		AddCharacter(keycode);
		redraw = 1;
   		} else if (key == 8)
			{
			//If we do hit backspace remove a character
   			RemoveCharacter();
			redraw = 1;
   			}
	   }
   }
}

/* Add the character to the string
 *
 * PARAMS
 *   NewCharacter* - the Character to be added
 *
 */
void ITextBox::AddCharacter(char* NewCharacter)
{
	char* tempstring;
	/*
	 * Get the string length and verify that we
	 * are not over our maximum
	 */
	int length = strlen(string);
	if (length < CharacterMax)
	{
	//Allocate more memory for out new string
	//tempstring = (char*)malloc(sizeof(char)*length + 2);
	//strcpy (tempstring, string);

	//Copy to temporary string
	strncat (string, NewCharacter, (size_t)1);
	//length = strlen(tempstring);

	//Reallocate our original string and copy to it
	//string = (char*)realloc(string, sizeof(char)*length + 1);
	//strcpy (string, tempstring);
	//Prevent memory leaks
	//free (tempstring);
	}
}

/* Remove the last character from the string
 *
 * PARAMS
 *   none
 *
 */
void ITextBox::RemoveCharacter()
{
	//Get the length
	int length = strlen(string);
	//Do we have more than one character left
	if (length >= 1)
		//If so take the last character and set it as
		//the null terminator
		string[length-1] = 0;
	//	else if (length == 1)
	//		{
			//If it is out last charactor, re-allocate
			//the memory and set it to zero
	//		string = (char*)realloc(string, sizeof(char));
	//		string = "";
	//		}
}

/* Check if the string is empty
 *
 * RETURNS
 *   bool - if the string is empty
 *
 */
bool ITextBox::IsEmpty()
{
	if(string[0] == 0)
		return true;
		else
			return false;
}

/* Clear the string
 *
 * PARAMS
 *   none
 *
 */
void ITextBox::Clear()
{
	//string = (char *) realloc(string, sizeof(char));
	//string = "";
	//memset(string, 0CharacterMax+1
	string[0] = 0;
}

void ITextBox::SetFocus(bool focus)
{
    FocusLock = focus;
    CreateCarot = focus;
}
