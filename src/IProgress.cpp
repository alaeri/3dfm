/*
 * 3dfm - 3D File Manager
 * $Id: IProgress.cpp,v 1.5 2003/04/08 17:54:10 adam Exp $
 */

/*
 * IList
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <IProgress.h>

/* design definitions */
#define HEIGHT 10
#define SLOPE 3
#define HEADER 4
#define ZDIR 0.1
#define CHAR_WIDTH 2
#define MIN 0
#define MAX 100

/* Create the progress bar object based on the passed in arguments
 *
 * PARAMS
 *   location - the top left corner of the progress bar
 *   length - the length of the progress bar
 *   paneColor - the color of the back pane
 *   progressColor - the color of the progress indicator
 *   current - the current location of the progress indicator
 *   title - the title for the progress bar
 *
 */
IProgress::IProgress(GPoint location, GLfloat length, GColor paneColor, GColor
	    progressColor, float current, char* title) {
    this->title = title;
    this->location = location;
    this->length = length;
    this->paneColor = paneColor;
    this->progressColor = progressColor;
    this->current=current;

    redraw = false;
    direction = true;
    aalpha = 0.0;
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

/* Draw the progress bar objects
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   int - tells the calling program that the object is animating
 *         needs to be redrawn
 */
void IProgress::execGL(void)
{
//     glDisable(GL_DEPTH_TEST);
//     glDisable(GL_FOG);
//     glDisable(GL_LIGHTING);
//     glEnable(GL_BLEND);

    //Set the panecolor to the active color
    paneColor.execGL();

    //Creating progress panel
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY()  - HEADER, ZDIR);
    glVertex3f(location.getX(), location.getY() - HEIGHT - HEADER, ZDIR);
    glVertex3f(location.getX() + length, location.getY() - HEIGHT - HEADER, ZDIR);
    glVertex3f(location.getX() + length, location.getY()  - HEADER, ZDIR);
    glEnd();

    //Creating title panel
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY() - HEADER, ZDIR);
    glVertex3f(location.getX() + (length/2), location.getY() - HEADER, ZDIR);
    glVertex3f(location.getX() + (length/2) - SLOPE, location.getY(), ZDIR);
    glEnd();

    //Creating percentage panel
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + (length/2), location.getY() - HEIGHT - HEADER, ZDIR);
    glVertex3f(location.getX() + (length/2) + SLOPE, location.getY() - (2*HEADER) -
	       HEIGHT, ZDIR);
    glVertex3f(location.getX() + length, location.getY() - (2*HEADER) - HEIGHT, ZDIR);
    glVertex3f(location.getX() + length, location.getY() - HEADER - HEIGHT, ZDIR);
    glEnd();

    //Creating progress bar
    progressColor.execGL();
    if ((aalpha < (progressColor.getAlpha())) && direction)
    		{
    		aalpha = aalpha + 0.015;
		}
			else if ((aalpha <= (progressColor.getAlpha()) && (!direction)))
				{
					aalpha = aalpha - 0.015;
					if (aalpha < 0.0)
						{
						direction = true;
						aalpha = 0.0;
						}
				}
				else if ((aalpha >= (progressColor.getAlpha()) && (direction)))
					{
					aalpha = progressColor.getAlpha();
					direction = false;
					}


    glColor4f(progressColor.getRed(), progressColor.getGreen(), progressColor.getBlue(), aalpha);
    GLfloat percent_end = CalculateLocation();
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 2, location.getY() - HEADER - 2.5, ZDIR + 0.1);
    glVertex3f(location.getX() + 2, location.getY() - HEADER - 7.5, ZDIR + 0.1);
    glVertex3f(location.getX() + percent_end, location.getY() - HEADER - 7.5, ZDIR + 0.1);
    glVertex3f(location.getX() + percent_end, location.getY() - HEADER - 2.5, ZDIR + 0.1);
    glEnd();


    //Creating Labels
    //Set color to font color
    glColor4f(1.0, 1.0, 1.0, 0.5);
    char label[4];
    char complete[11] = "% Complete";  //Percent complete is always displayed
    char* new_title;
    sprintf(label, "%d", (int)current);
    int l_label = strlen(label);
    int l_title = strlen(title);

    /*
     * If the label entered will not fit in the allotted space, cut it back so that it fits
     * and add 3 dots to the end.
     * Note:  3 dots are added to all labels reguardless
     */

    int available = ((((int)length) / 2) / 2) - 4;
    new_title = (char*)malloc(available);

    if (l_title > available)
    {
    	strncpy(new_title, title, (size_t)available);
	new_title[available] = 0;
	strcat (new_title, "...");
	new_title[available + 3] = 0;
    }
    else
    {
    	strcpy(new_title, title);
	strcat (new_title, "...");
    }

//    float percent_label = location.getX() + (length/2) + 2;

    //Calculate printing location of title

    glPushMatrix();

    glRasterPos3f(location.getX() + 1, location.getY() - 2.75, ZDIR + 0.1);
    ourPrintString(GLUT_BITMAP_8_BY_13, new_title);
    //Calculate printing location of the percent complete phrase
    glRasterPos3f(location.getX() + length - 20, location.getY() - HEADER - HEIGHT - 2.75, ZDIR + 0.1);
    ourPrintString(GLUT_BITMAP_8_BY_13, complete);

    /* Calculate the location of the percent complete number based on its size
     * For example:
     *   5 percent is displayed as -> 5% Complete
     *   10 percent is displayed as -> 10% Complete
     *   Notice that the number shifts to the left based on its size
     */

    glRasterPos3f(location.getX() + length - 20 - (l_label * CHAR_WIDTH),
		  location.getY() - HEADER - HEIGHT - 2.75, ZDIR + 0.1);
    ourPrintString(GLUT_BITMAP_8_BY_13, label);

    glPopMatrix();
    if (current == 100)
        redraw = false;
	else
	redraw = true;
}

void IProgress::UpdateCurrent(int new_current)
{
	current = new_current;
	redraw = true;
}

GLfloat IProgress::CalculateLocation(void)
{
    //Calculate percentage
    float percent = (current - MIN)/(MAX - MIN);
    if (percent < 0.01)
	percent = 0.01;

    //Calculate location
    GLfloat drawLocation = ((location.getX() + length - 2.0) - (location.getX() + 2.0)) *
	percent;
    if (percent < 0.02)
	drawLocation = 2.0;
    else
	if (percent > 0.98)
	    drawLocation = length - 2.0;

    return drawLocation;
}
