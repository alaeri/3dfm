/*
 * 3dfm - 3D File Manager
 * $Id: PrefsControl.cpp,v 1.16 2003/04/18 21:43:45 kristopher Exp $
 */

/*
 * PrefsControl
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <PrefsControl.h>

#define ZDIR 0.1
/* Creates the PrefsControl object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the slider
 *   size - how long and high the dialog is going to be
 *   paneColor - the color of the main pane
 *   titlebarColor - the color of the title bar
 *   titleText - text that goes in the title bar
 *
 */
PrefsControl::PrefsControl(GPoint location, GPoint size, GColor paneColor,
		 GColor titlebarColor, char* titleText)
{
    //Pull in all values...

    this->location = location;
    this->size = size;
    this->paneColor = paneColor;
    this->titlebarColor = titlebarColor;
    this->titleText = titleText;

    //Setup our mouse control variables
    redraw = true;

    //Set active tab
    tab = 1;

    //Set to stay open
    exitStatus = 0;

    //Set animation flags
    Animate = true;
    aa = 0.0;
    
    //set locks
    lockio = true;

    CreateObjects();
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

/* Draw the physical main dialog pane, the title bar
 * and the active tab
 *
 * PARAMS
 *  none
 *
 */
bool PrefsControl::execGL(void) {

    //Run animation
    if (Animate)
    {
    	Animate = AnimatePane();
	return Animate;
	lockio = true;
    }
    else
    	lockio = false;


    //Set the panecolor of the object to the active color
    paneColor.execGL();

    //Creating Main Panel
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY(), ZDIR);
    glEnd();

    //Creating the titlebar
    titlebarColor.execGL();
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), ZDIR + 0.1);
    glVertex3f(location.getX(), location.getY() - 6.0, ZDIR + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY() - 6.0, ZDIR + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY(), ZDIR + 0.1);
    glEnd();

    paneColor.execGL();

    //Draw the buttons
    okButton->execGL();
    cancelButton->execGL();

    if (tab == 1)
    {
    	//Draw the look and feel tab
	lf->execGL();
	glColor4f(0.2, 0.4, 0.8, 0.6);
	//Draw the title tab for the inactive tab
	glBegin(GL_QUADS);
	glVertex3f(location.getX() + 29.0, location.getY() - 11.0, ZDIR + 0.1);
        glVertex3f(location.getX() + 29.0, location.getY() - 16.0, ZDIR + 0.1);
    	glVertex3f(location.getX() + 50.0, location.getY() - 16.0, ZDIR + 0.1);
    	glVertex3f(location.getX() + 50.0, location.getY() - 11.0, ZDIR + 0.1);
	glEnd();

	//Draw the text for the inactive title tabs
	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 0.8);
    	glTranslatef(location.getX(), location.getY(), ZDIR);
    	glRasterPos3f(31.0, -15.0, 0.2);
    	ourPrintString(GLUT_BITMAP_8_BY_13, "Filtering");
        //Done drawing text, go back into normal coordinate system
    	glPopMatrix();
    } else if (tab == 2)
    	{
		//Draw the filter tab
	    	filter->execGL();
		glColor4f(0.2, 0.4, 0.8, 0.6);
		//Draw the title tab for the inactive tab
		glBegin(GL_QUADS);
		glVertex3f(location.getX() + 1.0, location.getY() - 11.0, ZDIR + 0.1);
        	glVertex3f(location.getX() + 1.0, location.getY() - 16.0, ZDIR + 0.1);
    		glVertex3f(location.getX() + 29.0, location.getY() - 16.0, ZDIR + 0.1);
    		glVertex3f(location.getX() + 29.0, location.getY() - 11.0, ZDIR + 0.1);
		glEnd();

		//Draw the text for the inactive title tabs
		glPushMatrix();
		glColor4f(1.0, 1.0, 1.0, 0.8);
    		glTranslatef(location.getX(), location.getY(), ZDIR);
    		glRasterPos3f(2.0, -15.0, 0.2);
    		ourPrintString(GLUT_BITMAP_8_BY_13, "Look and Feel");
        	//Done drawing text, go back into normal coordinate system
    		glPopMatrix();
    	}


    //Draw the title text
    glPushMatrix();
    glColor4f(1.0, 1.0, 1.0, 0.9);
    glTranslatef(location.getX(), location.getY(), ZDIR);
    glRasterPos3f(1.0, -4.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, titleText);
    //Done drawing text, go back into normal coordinate system
    glPopMatrix();

    return redraw;
}

/* Create the IFC widgets to be used in the main prefs dialog
 *
 *
 * PARAMS
 *  none
 *
 */
void PrefsControl::CreateObjects()
{
	okButton = new IButton(GPoint(location.getX() + size.getX() - 15.0,
	                                location.getY() - size.getY() + 7.0,
					ZDIR + 0.1),
			       GPoint(14, 6, 0.1), paneColor, "OK");

	cancelButton = new IButton(GPoint(location.getX() + size.getX() - 31.0,
	                                location.getY() - size.getY() + 7.0,
					ZDIR + 0.1),
			       GPoint(14, 6, 0.1), paneColor, "Cancel");

	lf = new PrefsLFtab(GPoint(location.getX() + 1.0, location.getY() - 10.0, ZDIR + 0.2),
    			GPoint(size.getX() - 2.0, size.getY() - 18, 0.0),
			GColor(0.1, 0.1, 0.8, 0.6),
			"Look and Feel");

	filter = new PrefsFiltertab(GPoint(location.getX() + 1.0, location.getY() - 10.0, ZDIR + 0.2),
    			GPoint(size.getX() - 2.0, size.getY() - 18, 0.0),
			GColor(0.1, 0.1, 0.8, 0.6),
			"Filtering");
}

/* Create the clickable title tabs, because they are not
 * IFC widgets
 *
 * PARAMS
 *  none
 *
 */
void PrefsControl::CreateClickable()
{
	filterTab = new GRect(GPoint(location.getX() + 29.0, location.getY() - 11.0, ZDIR + 0.1),
			  GPoint(location.getX() + 50.0, location.getY() - 16.0, ZDIR + 0.1));

	lfTab = new GRect(GPoint(location.getX() + 1.0, location.getY() - 11.0, ZDIR + 0.1),
			  GPoint(location.getX() + 29.0, location.getY() - 16.0, ZDIR + 0.1));

}

/* Handles the mouse clicks
 *
 *
 * PARAMS
 *  pt - the point clicked on the screen
 *  ClickType - where the click was up, down, or hold
 *
 */
void PrefsControl::clicked(GPoint pt, int ClickType)
{
	if (lockio)
		return;
	/*
	 * The prefs dialog has three different status stages
	 * they are checked by the calling program to know if
	 * the dialog needs to be closed and if the data should
	 * be updated.  The stages are:
	 *
	 * 0  - Dialog is open and being used; do not close
	 * 1  - OK was pressed save the data
	 * -1 - Cancel was pressed ignore changes
	 */

	//If the OK button was clicked change the status
	okButton->clicked(pt, ClickType);
	if (okButton->Clicked)
		exitStatus = 1;

	//If CANCEL button was clicked ignore changes
	cancelButton->clicked(pt, ClickType);
	if (cancelButton->Clicked)
	  exitStatus = -1;

	//Hit testing for the title tabs
	bool hit = false;

	//If we hit when the first tab is active...
	if (tab == 1)
	{
	   //Did we get the inactive title tab?
	   hit = filterTab->PtinRect(pt);
	   if (hit && ClickType == GLUT_DOWN)
	   	//If we did change the active tab
		tab = 2;
	   //Set the animation flags
	   //Pass the click on to the Look and Feel controller
	   lf->clicked(pt, ClickType);
	   filter->Animate = true;
	   filter->aa = 0.0;
	}
	//If we hit when the second tab is active...
	else if (tab == 2)
	{
	   //Did we hit the inactive title tab?
	   hit = lfTab->PtinRect(pt);
	   if (hit && ClickType == GLUT_DOWN)
	   	//If we did change the active tab
		tab = 1;
	   //Pass the click on to the Filter controller
	   filter->clicked(pt, ClickType);
	   //Set the animation flags
	   lf->Animate = true;
	   lf->aa = 0.0;
	}

	redraw = true;
}

/* Perform object dragging
 *
 * PARAMS
 *   pt - the point where the mouse is
 *
 */
void PrefsControl::dragged(GPoint pt)
{
	if (lockio)
		return;
	/*
	 *Send the dragged point to the appropriate
	 *controller based on the active tab
	 */
    	if (tab == 1)
	   lf->dragged(pt);
	   else if (tab == 2)
	   	filter->dragged(pt);
}

/* Monitor mouse movement
 *
 * PARAMS
 *   pt - the point where the mouse is
 *
 */
void PrefsControl::moved(GPoint pt)
{
	if (lockio)
		return;
	/*
	 *Send the dragged point to the appropriate
	 *controller based on the active tab
	 */
	if (tab == 1)
	   lf->moved(pt);
	   else if (tab == 2)
	   	filter->moved(pt);
}

/* Sends keypresses to appropriate controller
 *
 * PARAMS
 *   key - the code for the key hit
 *   x   - the x coordinate of the mouse when the key was hit
 *   y   - the y coordinate of the mouse when the key was hit
 *
 */
void PrefsControl::keypress(unsigned char key, int x, int y)
{
	if (lockio)
		return;
	//If the ESC key is hit send the signal the same as if the
	//CANEL button was hit
	if (key == 27)
		exitStatus = -1;
		else if (key == 13)
			{

			exitStatus = 1;
			}
			else if (tab == 1)
	   			lf->keypress(key, x, y);
	   			else if (tab == 2)
	   				filter->keypress(key, x, y);
}

/* Sends special keypresses to appropriate controller
 *
 * PARAMS
 *   key - the code for the key hit
 *   x   - the x coordinate of the mouse when the key was hit
 *   y   - the y coordinate of the mouse when the key was hit
 *
 */
void PrefsControl::specialkeypress(int key, int x, int y)
{
	if (lockio)
		return;
	if (tab == 1)
	   lf->specialkeypress(key, x, y);
	   else if (tab == 2)
	   	filter->specialkeypress(key, x, y);
}

/* Converts the int depth to a character and sends
 * it to the Look and Feel tab controller
 *
 * PARAMS
 *   depth - the depth of the current system
 *
 */
void PrefsControl::setdepth(int depth)
{
	/*
	 * The reason that this conversion needs to be
	 * done is because the textbox takes a string.
	 * There for we must convert the int.  We do it
	 * here so that the called function does not need
	 * to worry about it
	 */
	char* stringdepth;
	stringdepth = (char*) malloc(sizeof (char) * 19 + 1);
	sprintf (stringdepth, "%d", depth);

	lf->setdepth(stringdepth);

	//Prevent memory leak! eek!
	free (stringdepth);
}

/* Converts the int breath to a character and sends
 * it to the Look and Feel tab controller
 *
 * PARAMS
 *   breadth - the depth of the current system
 *
 */
void PrefsControl::setbreadth(int breadth)
{
	/*
	 * The reason that this conversion needs to be
	 * done is because the textbox takes a string.
	 * There for we must convert the int.  We do it
	 * here so that the called function does not need
	 * to worry about it
	 */
	char* stringbreadth;
	stringbreadth = (char*) malloc(sizeof (char) * 19 + 1);
	sprintf (stringbreadth, "%d", breadth);

	lf->setbreadth(stringbreadth);

	//Prevent memory leak! eek!
	free (stringbreadth);
}

/* Gets the depth from the Look and Feel tab controller
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   the depth
 */
int PrefsControl::getdepth ()
{
	return lf->getdepth();
}

/* Gets the breadth from the Look and Feel tab controller
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   the breadth
 */
int PrefsControl::getbreadth()
{
	return lf->getbreadth();
}

/* Sets the color packet in the Look and Feel controller
 *
 * PARAMS
 *  colorPacket - the color packet from the calling program
 *
 * RETURNS
 *   none
 */
void PrefsControl::setcolorpacket(s_colorpacket colorPacket)
{
	lf->setcolorpacket(colorPacket);
}

/* Gets the color packet from the Look and Feel controller
 *
 * PARAMS
 *  none
 *
 * RETURNS
 *   the color packet from the Look and Feel controller
 */
s_colorpacket PrefsControl::getcolorpacket()
{
	return lf->getcolorpacket();
}

/* Sets the check packet in the Filter controller
 *
 * PARAMS
 *  checkPacket - the check packet from the calling program
 *
 * RETURNS
 *   none
 */
void PrefsControl::setcheckpacket(s_checkpacket checkPacket)
{
	filter->setcheckpacket(checkPacket);
}

/* Gets the check packet from the Filter controller
 *
 * PARAMS
 *  none
 *
 * RETURNS
 *   the check packet from the Filter controller
 */
s_checkpacket PrefsControl::getcheckpacket()
{
	return filter->getcheckpacket();
}

/* Performs the animation "fade-in" when the dialog is originally
 * displayed
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   bool - returns true if the pane is still animating otherwise
 *        - return false
 */
bool PrefsControl::AnimatePane()
{
    /*
     * Set the original value to not animating, if the alpha
     * value has to be changed it will change the value
     */
    bool StillAnimate = false;

    GLfloat compare_value;

    /*
     * Check to see if we have reached the set alpha value, if not
     * increment and set to not animating
     */

    compare_value = paneColor.getAlpha();

    /*
     *The -0.1 prevents overshoot, so that the pane doesn't get less and
     *less transparent and the all of a sudden go back to more transparent
     */
    if (aa < (compare_value - 0.1))
    {
    	aa = aa + 0.05;
	StillAnimate = true;
    }

    //Pull in colors slected by the user
    GLfloat ar = paneColor.getRed();
    GLfloat ag = paneColor.getGreen();
    GLfloat ab = paneColor.getBlue();

    //Set to the temporary color
    glColor4f(ar, ag, ab, aa);

    //Creating Main Panel
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(), location.getY(), ZDIR);
    glEnd();

    return StillAnimate;
}
