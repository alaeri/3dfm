/*
 * 3dfm - 3D File Manager
 * $Id: ISearchBox.cpp,v 1.10 2003/04/08 01:51:43 kristopher Exp $
 */

/*
 * ISearchBox
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <ISearchBox.h>

#define ZDIR 0.1
/*"Prefs.h" Creates the ISearchBox object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the search box
 *   paneColor - the color of the search box
 *
 */
ISearchBox::ISearchBox(GPoint location, GColor paneColor)
{
    //Pull in all values...

    this->location = location;
    this->paneColor = paneColor;

    //Setup our mouse control variables
    redraw = true;
    notFound = false;
    haveData = false;
    closeBox = false;

    //Set animation flags
    Animate = true;
    aa = 0.0;

    CreateObjects();

    searchText->SetFocus(true);
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
bool ISearchBox::execGL(void) {

    //Run animation
    if (Animate)
    {
    	Animate = AnimatePane();
	return Animate;
    }

    //Set the panecolor of the object to the active color
    paneColor.execGL();

    //Creating title tab
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 2.0 , ZDIR + 0.2);
    glVertex3f(location.getX(), location.getY() - 8.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 15.0, location.getY() - 8.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 9.0, location.getY() - 2.0, ZDIR + 0.2);
    glEnd();

    //Creating the main tab
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 8.0, ZDIR + 0.2);
    glVertex3f(location.getX(), location.getY() - 18.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 64.0, location.getY() - 18.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 64.0, location.getY() - 8.0, ZDIR + 0.2);
    glEnd();

    //Creating the button tab
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 17.0, location.getY() - 18.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 23.0, location.getY() - 26.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 64.0, location.getY() - 26.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 64.0, location.getY() - 18.0, ZDIR + 0.2);
    glEnd();

    glPushMatrix();
    glColor4f(1.0, 1.0, 1.0, 0.9);
    glTranslatef(location.getX(), location.getY(), ZDIR);
    glRasterPos3f(2.0, -6.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "GoTo");
    if (notFound)
    {
    	glRasterPos3f(18.0, -6.0, 0.2);
    	ourPrintString(GLUT_BITMAP_8_BY_13, "* Folder Not Found!");
    }
    //Done drawing text, go back into normal coordinate system
    glPopMatrix();



    findButton->execGL();
    clearButton->execGL();
    closeButton->execGL();
    searchText->execGL();

    return redraw;
}

/* Create the IFC widgets to be used in the main prefs dialog
 *
 *
 * PARAMS
 *  none
 *
 */
void ISearchBox::CreateObjects()
{
   findButton = new IButton(GPoint(location.getX() + 52.0,
   				     location.getY() - 18.0,
				     ZDIR + 0.2),
			      GPoint(10.0, 6.0, 0.0),
			      GColor(0.2, 0.3, 0.9, 0.8),
			      "Go");
   clearButton = new IButton(GPoint(location.getX() + 38.0,
   				     location.getY() - 18.0,
				     ZDIR + 0.2),
			      GPoint(12.0, 6.0, 0.0),
			      GColor(0.2, 0.3, 0.9, 0.8),
			      "Clear");

   closeButton = new IButton(GPoint(location.getX() + 24.0,
   				     location.getY() - 18.0,
				     ZDIR + 0.2),
			      GPoint(12.0, 6.0, 0.0),
			      GColor(0.2, 0.3, 0.9, 0.8),
			      "Close");

   searchText = new ITextBox(GPoint(location.getX() +2.0,
   				    location.getY() - 10.0,
				    ZDIR + 0.2),
			     GPoint(60.0, 6.0, 0.0),
			     paneColor, //GColor(0.7, 0.5, 0.3, 0.6),
			     0);
}

/* Create the clickable title tabs, because they are not
 * IFC widgets
 *
 * PARAMS
 *  none
 *
 */
void ISearchBox::clicked(GPoint pt, int ClickType)
{
	findButton->clicked(pt, ClickType);
	if (findButton->Clicked)
	{
		haveData = true;
	}

	clearButton->clicked(pt, ClickType);
	if (clearButton->Clicked)
	{
		searchText->Clear();
		searchText->SetFocus(true);
	}

	closeButton->clicked(pt, ClickType);
	if (closeButton->Clicked)
	{
		closeBox = true;
	}

	searchText->clicked(pt, ClickType);

	redraw = true;
}

/* Sends keypresses to appropriate controller
 *
 * PARAMS
 *   key - the code for the key hit
 *   x   - the x coordinate of the mouse when the key was hit
 *   y   - the y coordinate of the mouse when the key was hit
 *
 */
void ISearchBox::keypress(unsigned char key, int x, int y)
{
	//If the not found banner is up and we hit a key,
	//we make it go away
	if (notFound)
		notFound = false;
	//Simulate clicking ok when pressing enger
	if (key == 13)
		haveData = true;
	//Simulate clicking clear when pressing delete
	else if (key == 127)
	{
		searchText->Clear();
		searchText->SetFocus(true);
	}
	//Close the box when escape is closed
	else if (key == 27)
		closeBox = true;
	else
	searchText->SendKeys(key);
}

/* Returns the text in the search box
 *
 * PARAMS
 *  none
 *
 * RETURNS
 *   the current string
 *
 */
char* ISearchBox::gettext()
{
	return searchText->string;
}

/* Function that tells the user the object was not found
 *
 * PARAMS
 *  none
 *
 * RETURNS
 *   none
 *
 */
void ISearchBox::notfound()
{
	//Show the not found flag
	//set the focus to the textbox
	//and change the data flag
	notFound = true;
	searchText->SetFocus(true);
	haveData = false;
}

/* Performs the animation "fade-in" when the gotoBox is originally
 * displayed
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   bool - returns true if the pane is still animating otherwise
 *        - return false
 */
bool ISearchBox::AnimatePane()
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
    	aa = aa + 0.025;
	StillAnimate = true;
    }

    //Pull in colors slected by the user
    GLfloat ar = paneColor.getRed();
    GLfloat ag = paneColor.getGreen();
    GLfloat ab = paneColor.getBlue();

    //Set to the temporary color
    glColor4f(ar, ag, ab, aa);

    //Creating title tab
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 2.0 , ZDIR + 0.2);
    glVertex3f(location.getX(), location.getY() - 8.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 15.0, location.getY() - 8.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 9.0, location.getY() - 2.0, ZDIR + 0.2);
    glEnd();

    //Creating the main tab
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 8.0, ZDIR + 0.2);
    glVertex3f(location.getX(), location.getY() - 18.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 64.0, location.getY() - 18.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 64.0, location.getY() - 8.0, ZDIR + 0.2);
    glEnd();

    //Creating the button tab
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 17.0, location.getY() - 18.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 23.0, location.getY() - 26.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 64.0, location.getY() - 26.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 64.0, location.getY() - 18.0, ZDIR + 0.2);
    glEnd();

    return StillAnimate;
}
