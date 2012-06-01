/*
 * 3dfm - 3D File Manager
 * $Id: PrefsFiltertab.cpp,v 1.11 2003/04/18 21:43:45 kristopher Exp $
 */

/*
 * PrefsFiltertab
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <PrefsFiltertab.h>

#define ZDIR 0.1

/* Creates the PrefsFiltertab object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the tab
 *   size - the size of the Filter tab
 *   paneColor - the color of the tab
 *   tabTitle - the title of the tab
 *
 */
PrefsFiltertab::PrefsFiltertab(GPoint location, GPoint size, GColor paneColor,
		 char* tabTitle)
{
    //Pull in all values...

    this->location = location;
    this->size = size;
    this->paneColor = paneColor;
    this->tabTitle = tabTitle;

    //Setup our mouse control variables
    redraw = true;

    CreateObjects();

    //Set animation flags
    Animate = true;
    aa = 0.0;
    
    //input locks
    lockio = true;
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

/* Draw the Filter tab
 *
 * PARAMS
 *
 *
 */
bool PrefsFiltertab::execGL(void) {

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

    //Creating main tab
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX(), location.getY() - size.getY(), ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(), ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX(), location.getY() - 6.0, ZDIR + 0.2);
    glEnd();

    //Creating the titletab
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 28.0, location.getY(), ZDIR + 0.2);
    glVertex3f(location.getX() + 28.0, location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 48.0, location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 48.0, location.getY(), ZDIR + 0.2);
    glEnd();

    glPushMatrix();
    glColor4f(1.0, 1.0, 1.0, 0.9);
    glTranslatef(location.getX(), location.getY(), ZDIR);
    glRasterPos3f(29.0, -4.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, tabTitle);
    glRasterPos3f(4.0, -12.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "Viewable Folders");
    glRasterPos3f(4.0, -35.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "Filtering Settings");
    //Done drawing text, go back into normal coordinate system
    glPopMatrix();

    glColor4f(0.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX() + 38.0, location.getY() - 11.0, ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX() - 2.0 , location.getY() - 11.0, ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX() - 2.0, location.getY() - 31.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 2.0, location.getY() - 31.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 2.0, location.getY() - 11.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 3.0, location.getY() - 11.0, ZDIR + 0.2);
    glEnd();

    glColor4f(0.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX() + 42.0, location.getY() - 34.0, ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX() - 2.0 , location.getY() - 34.0, ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX() - 2.0, location.getY() - 47.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 2.0, location.getY() - 47.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 2.0, location.getY() - 34.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 3.0, location.getY() - 34.0, ZDIR + 0.2);
    glEnd();

    paneColor.execGL();

    //Draw all IFC widgets

    hiddenCheckBox->execGL();
    linkedCheckBox->execGL();
    systemCheckBox->execGL();
    labelsCheckBox->execGL();
    checksCheckBox->execGL();
    fontsCheckBox->execGL();


    return redraw;
}

/* Create the IFC widgets to be used in the main prefs dialog
 *
 *
 * PARAMS
 *  none
 *
 */
void PrefsFiltertab::CreateObjects()
{
	systemCheckBox = new ICheckBox(GPoint(location.getX() + size.getX() - 40.0, location.getY() - 17.0,
				             ZDIR),
				      GColor(0.0, 1.0, 1.0, 1.0),
				      "System Folders",
				      false);

	hiddenCheckBox = new ICheckBox(GPoint(location.getX() + 4.0, location.getY() - 17.0,
				             ZDIR),
				      GColor(0.0, 1.0, 0.0, 1.0),
				      "Hidden Folders",
				      false);

	linkedCheckBox = new ICheckBox(GPoint(location.getX() + 4.0, location.getY() - 25.0,
				             ZDIR),
				      GColor(1.0, 1.0, 0.0, 1.0),
				      "Linked Folders",
				      false);

	labelsCheckBox = new ICheckBox(GPoint(location.getX() + 4.0, location.getY() - 40.0,
				             ZDIR),
				      GColor(0.2, 0.8, 0.4, 1.0),
				      "View Labels (L)",
				      false);

	checksCheckBox = new ICheckBox(GPoint(location.getX() + size.getX() - 47.0, location.getY() - 40.0,
				             ZDIR),
				      GColor(0.2, 0.8, 0.4, 1.0),
				      "View Checks only (%)",
				      false);

	fontsCheckBox = new ICheckBox(GPoint(location.getX() + 4.0, location.getY() - 50.0,
				             ZDIR),
				      GColor(0.2, 0.8, 0.4, 1.0),
				      "Use Stroke Fonts (K)",
				      false);


}

/* Handles the mouse clicks
 *
 *
 * PARAMS
 *  pt - the point clicked on the screen
 *  ClickType - where the click was up, down, or hold
 *
 */
void PrefsFiltertab::clicked(GPoint pt, int ClickType)
{

   if (lockio)
	return;

   hiddenCheckBox->clicked(pt, ClickType);
   linkedCheckBox->clicked(pt, ClickType);
   systemCheckBox->clicked(pt, ClickType);
   labelsCheckBox->clicked(pt, ClickType);
   checksCheckBox->clicked(pt, ClickType);
   fontsCheckBox->clicked(pt, ClickType);
}

/* Perform object dragging
 *
 * PARAMS
 *   pt - the point where the mouse is
 *
 */
void PrefsFiltertab::dragged(GPoint pt)
{
	if (lockio)
		return;
}

/* Monitor mouse movement
 *
 * PARAMS
 *   pt - the point where the mouse is
 *
 */
void PrefsFiltertab::moved(GPoint pt)
{
	if (lockio)
		return;
}

/* Sends keypresses to appropriate controller
 *
 * PARAMS
 *   key - the code for the key hit
 *   x   - the x coordinate of the mouse when the key was hit
 *   y   - the y coordinate of the mouse when the key was hit
 *
 */
void PrefsFiltertab::keypress(unsigned char key, int x, int y)
{
	if (lockio)
		return;
}

/* Sends special keypresses to appropriate controller
 *
 * PARAMS
 *   key - the code for the key hit
 *   x   - the x coordinate of the mouse when the key was hit
 *   y   - the y coordinate of the mouse when the key was hit
 *
 */
void PrefsFiltertab::specialkeypress(int key, int x, int y)
{
	if (lockio)
		return;

}

/* Sets the current check packet
 *
 * PARAMS
 *   checkPacket - the new color packet
 *
 * RETURNS
 *   none
 */
void PrefsFiltertab::setcheckpacket(s_checkpacket checkPacket)
{
	this->checkPacket = checkPacket;

	hiddenCheckBox->Checked = this->checkPacket.hiddenfolders;
    	linkedCheckBox->Checked = this->checkPacket.linkedfolders;
    	systemCheckBox->Checked = this->checkPacket.systemfolders;
	labelsCheckBox->Checked = this->checkPacket.showlabels;
	checksCheckBox->Checked = this->checkPacket.enablechecks;
	fontsCheckBox->Checked = this->checkPacket.strokefonts;

}

/* Returns the current check packet
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   the current check packet
 */
s_checkpacket PrefsFiltertab::getcheckpacket()
{
    //Build the check packet for export
    checkPacket.hiddenfolders = hiddenCheckBox->Checked;
    checkPacket.linkedfolders = linkedCheckBox->Checked;
    checkPacket.systemfolders = systemCheckBox->Checked;
    checkPacket.showlabels = labelsCheckBox->Checked;
    checkPacket.enablechecks = checksCheckBox->Checked;
    checkPacket.strokefonts = fontsCheckBox->Checked;

    return checkPacket;
}

/* Performs the animation "fade-in" when the tab is originally
 * displayed
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   bool - returns true if the pane is still animating otherwise
 *        - return false
 */
bool PrefsFiltertab::AnimatePane()
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

    //Creating main tab
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX(), location.getY() - size.getY(), ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(), ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX(), location.getY() - 6.0, ZDIR + 0.2);
    glEnd();

    //Creating the titletab
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 28.0, location.getY(), ZDIR + 0.2);
    glVertex3f(location.getX() + 28.0, location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 48.0, location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 48.0, location.getY(), ZDIR + 0.2);
    glEnd();

    return StillAnimate;
}
