/*
 * 3dfm - 3D File Manager
 * $Id: ISlider.cpp,v 1.9 2003/04/05 21:44:43 kristopher Exp $
 */

/*
 * ISlider
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <ISlider.h>

/* design definitions */
#define HEIGHT 3
#define ZDIR 0.1
#define CHAR_WIDTH 2


/* Creates the ISlider object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the slider
 *   length - how long the slider is going to be
 *   paneColor - the color of the slider AND the slider pane
 *   labels - do you want to show labels or not
 *   min - the minimum value of the slider
 *   max - the maximum value of the slider
 *   start - the initial position of the slider
 *
 */
ISlider::ISlider(GPoint location, GLfloat length, GColor paneColor,
		 bool labels, int min, int max, float start) {
    //Pull in all values...
    this->labels = labels;
    this->location = location;
    this->length = length;
    this->paneColor = paneColor;
    this->min=min;
    this->max=max;

    //Setup our mouse control variables
    redraw = false;
    Capture = false;
    Focus = false;

    //Verify that our starting value is within limits
    if ((int)start > max)
	start = max;
    if ((int)start < min)
	start = min;

    this->start = CalculateLocation(start);
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

/* Draw the physical slider object
 *
 * PARAMS
 *
 *
 */
bool ISlider::execGL(void) {

    //Set the panecolor of the object to the active color
    paneColor.execGL();

    /*
     *The minimum length of the slider is 20, if it is less
     *than that set it to the minimum
     */
    if (length < 20.0) {
	length = 20.0;
    }

    //Creating slide box
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), ZDIR);
    glVertex3f(location.getX(), location.getY() - HEIGHT, ZDIR);
    glVertex3f(location.getX() + length, location.getY() - HEIGHT, ZDIR);
    glVertex3f(location.getX() + length, location.getY(), ZDIR);
    glEnd();

    //Creating slide slit
    GColor previous = paneColor;
    //The slide slit is ALWAYS black
    glColor4f(0.0, 0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glVertex3f(location.getX() + 2, location.getY() - 1.25, ZDIR);
    glVertex3f(location.getX() + 2, location.getY() - 1.75, ZDIR);
    glVertex3f(location.getX() + length - 2, location.getY() - 1.75, ZDIR);
    glVertex3f(location.getX() + length - 2, location.getY() - 1.25, ZDIR);
    glEnd();

    //Creating slider
    paneColor = previous;
    paneColor.execGL();
    GLfloat Slider_start;



    Slider_start = start;
    //fprintf (stderr, "ExecGL start:  %f\n", start);


    /* Slider design:
     *   Since the slider is 2.5 units wide and we want the pointed end to point on the
     *   current value a bit of moving as to be done.  CalculateLocation() returns where
     *   the value is, so we slide the entire slider 1.25 to the left to ensure we are
     *   pointed at the correct value.
     */

    glBegin(GL_POLYGON);
    glVertex3f(Slider_start - 1.25, location.getY() + 1.0, ZDIR + 0.1);
    glVertex3f(Slider_start - 1.25, location.getY() - HEIGHT - 1.0, ZDIR + 0.1);
    glVertex3f(Slider_start, location.getY() - HEIGHT - 2.5, ZDIR + 0.1);
    glVertex3f(Slider_start + 1.25, location.getY() - HEIGHT - 1, ZDIR + 0.1);
    glVertex3f(Slider_start + 1.25, location.getY() + 1.0, ZDIR + 0.1);
    glEnd();

    //Creating Labels
    if (labels)
    {
	glColor4f(1.0, 1.0, 1.0, 0.5);
	char label1[1000];
	char label2[1000];
	sprintf(label1, "%d", (int)min);
	sprintf(label2, "%d", (int)max);
	int l_label1 = strlen(label1);
	int l_label2 = strlen(label2);

        /*
	 *To prevent overlap of large value on a small slider we calculate where the first
	 *label ends and the last label begins.
	 */
	float end_label1 = location.getX() + (l_label1 * CHAR_WIDTH);
	float begin_label2 = location.getX() + length - (l_label2 * CHAR_WIDTH) - CHAR_WIDTH;

	//If the two labels do not overlap then set and display them
	if (begin_label2 > end_label1)
	{
	    //Set the first label to begin at the beginning of the slider
	    glRasterPos3f(location.getX(), location.getY() - 8.0, ZDIR + 0.1);
	    ourPrintString(GLUT_BITMAP_8_BY_13, label1);
	    //Set the last label to end at the end of the slider
	    glRasterPos3f(location.getX() + length - (l_label2 * CHAR_WIDTH),
			  location.getY() - 8.0, ZDIR + 0.1);
	    ourPrintString(GLUT_BITMAP_8_BY_13, label2);
	}
	paneColor = previous;
    }

    return redraw;
}

/* Determine where the slider should be draw according to its value
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   GLfloat - the location where the center of the slider is to be drawn
 */
GLfloat ISlider::CalculateLocation(float new_start)
{
    //Calculate percentage
    float percent = (new_start - min)/(max - min);

    //Calculate location
    GLfloat drawLocation = ((location.getX() + length - 2.0) - (location.getX() + 2.0)) *
	percent;
    if(percent == 0.0)
	drawLocation = 2.0;
    else
	if (percent > 0.99)
	    drawLocation = length - 2.0;
	    else
	    	drawLocation += 2.3;

    //Added 2.3 because i say so
    return (location.getX() + drawLocation);
}

/* Create the clickable objects
 *
 * PARAMS
 *   none
 *
 */
void ISlider::CreateClickable()
{
	SliderTab = new GRect(GPoint(location.getX() + start - 1.25,
			location.getY() + 1.0,
			ZDIR + 0.1),
			GPoint(location.getX() + start + 1.25,
			location.getY() - HEIGHT - 1.0,
			ZDIR + 0.1));

	SliderDevice = new GRect(GPoint(location.getX(), location.getY(), ZDIR),
			GPoint(location.getX() + length, location.getY() - HEIGHT - 1, ZDIR));
}

/* Check if the object was hit and if so was it down or up
 *
 * PARAMS
 *   pt - the point where the mouse is
 *   ClickType - what type of click (up or down)
 *
 */
void ISlider::clicked(GPoint pt, int ClickType)
{

    bool hitDevice, hitTab;

    //Check for a hit
    hitDevice = SliderDevice->PtinRect(pt);
    hitTab = SliderTab->PtinRect(pt);

    //Have we hit the slider tab
    if (hitTab)
    {
       	   //Check what type of message (up or down)
  	   //The mouse is clicked
	   if (ClickType == GLUT_DOWN)
	   {
	   	//Check if we are onthe end bounds
		if (pt.getX() <= (location.getX() + 2))
			start = location.getX() + 2;
			else if (pt.getX() >= (location.getX() + length - 2))
				start = (location.getX() + length - 2);
				else
					start = pt.getX();

		//Move the GRect to the appropriate place
		SliderTab->ReSize(GPoint(start - 1.25,
			location.getY() + 1.0,
			ZDIR + 0.1),
			GPoint(start + 1.25,
			location.getY() - HEIGHT - 1.0,
			ZDIR + 0.1));

		//Set our flags
		Capture = true;
		redraw = true;
		Focus = true;
	    }

    }else
    //Are we in the slider
    if (hitDevice)
    {
    	//Be sure it is a button down so we don't move on an undragged object
	if (ClickType == GLUT_DOWN)
	{
	   //Are we between the beginning of the slit and the beginning of the slider
     	   if ((pt.getX() <= (location.getX() + 2)) && (pt.getX() >= location.getX()))
    		{
		//Move to end of slit
		start = location.getX() + 2;
		SliderTab->ReSize(GPoint(start - 1.25,
			location.getY() + 1.0,
			ZDIR + 0.1),
			GPoint(start + 1.25,
			location.getY() - HEIGHT - 1.0,
			ZDIR + 0.1));
		redraw = true;
 		}
		//Are we between the end of the slit and the end of the slider
		else if ((pt.getX() >= (location.getX() + length - 2) && pt.getX() <= location.getX() + length))
			{
			start = location.getX() + length - 2;
			SliderTab->ReSize(GPoint(start - 1.25,
			location.getY() + 1.0,
			ZDIR + 0.1),
			GPoint(location.getX() + start + 1.25,
			location.getY() - HEIGHT - 1.0,
			ZDIR + 0.1));
			redraw = true;
			}
			//Otherwise we are along the slit somewhere so move to that location
			else
				{
				start = pt.getX();
				SliderTab->ReSize(GPoint(start - 1.25,
					location.getY() + 1.0,
					ZDIR + 0.1),
					GPoint(start + 1.25,
					location.getY() - HEIGHT - 1.0,
					ZDIR + 0.1));
				redraw = true;
				}
	}
	//Set the flags
	Capture = false;
	Focus = true;
    }

    //Was the mouse button lifted and were we captured?
    if (Capture && (ClickType == GLUT_UP))
    {
    	//Perform boundary checking
    	if (pt.getX() <= (location.getX() + 2))
			start = location.getX() + 2;
			else if (pt.getX() >= (location.getX() + length - 2))
				start = location.getX() + length - 2;
				else
					start = pt.getX();

	//Move the GRect as well
	SliderTab->ReSize(GPoint(start - 1.25,
					location.getY() + 1.0,
					ZDIR + 0.1),
					GPoint(start + 1.25,
					location.getY() - HEIGHT - 1.0,
					ZDIR + 0.1));
	//Set the flags
	Capture = false;
	redraw = true;
	Focus = false;
    }
}

/* Perform object dragging
 *
 * PARAMS
 *   pt - the point where the mouse is
 *
 */
void ISlider::dragged(GPoint pt)
{
   //Are we captured?
   if (Capture)
   {
   	//Perform boundary checking so we don't go off the slider slit
	if (pt.getX() <= (location.getX() + 2))
		start = location.getX() + 2;
		else if (pt.getX() >= (location.getX() + length - 2))
			start = (location.getX() + length - 2);
			else
				start = pt.getX();

	//Move the GRect as well
	SliderTab->ReSize(GPoint(start - 1.25,
			location.getY() + 1.0,
			ZDIR + 0.1),
			GPoint(start + 1.25,
			location.getY() - HEIGHT - 1.0,
			ZDIR + 0.1));

	//Set flags
	redraw = true;
	Focus = true;
    }
    else
    	{
	//We are not captured, reset flags
    	Focus = false;
    	}
}

/* Obtain the current value of the slider based on its position
 * Note:  This function is accurate, but due to the movement of the
 *        mouse, when a large slider is created, for instance between
 *        0 and 5000 and it is only two inches long it impossible
 *        for the mouse to touch all 5000 points.  Therefore when
 *        the ISlider is linked with ITextBox, one can witness that
 *        not all values will be hit.  In inorder to get around this
 *        it is recommended to "buddy" the ITextBox and ISlider so that
 *        a desired number can be set.  In addition, in the situation
 *        it is recommended that the ITextBox is used to retrieve values.
 *        Please keep in mind that the ISlider is mearly a representation
 *        of limits.  Use the ITextBox to pin point values.
 * PARAMS
 *   none
 *
 */
int ISlider::GetValue()
{
    //Calculate percentage
    float percent = ((start - (location.getX() + 2.0))/((location.getX() + length - 2.0)
    - (location.getX() + 2.0)));

    //"Just in case" bounds checking
    if (percent < 0.01)
	percent = 0.0;
    if (percent > 1.0)
	percent = 1.00;

    //Calculate location
    float value;
    int CurrentValue;
    value = (max - min) * percent;
    CurrentValue = int(min) + int(value);

    return CurrentValue;
}

/* Set the current value of the slider
 *
 * PARAMS
 *   NewValue - value slider is to be changed to
 *
 */
void ISlider::SetValue(int NewValue)
{
	/* 
	 * Note: Only is the value is within bounds is
	 * it accepted, otherwise the command is ignored.
	 * This was chosen rather than moving to one of the
	 * extremes incase there was an error in the 
	 * translation code.
	 */
	if  ((NewValue >= min) && (NewValue <= max))
	{
	start = CalculateLocation(NewValue);
	//Move the GRect as well
	SliderTab->ReSize(GPoint(start - 1.25,
			location.getY() + 1.0,
			ZDIR + 0.1),
			GPoint(start + 1.25,
			location.getY() - HEIGHT - 1.0,
			ZDIR + 0.1));
	redraw = true;
	}
}

/* Get the minimum value of the slider
 *
 * PARAMS
 *   int - the minimum value of the slider
 *
 */
int ISlider::GetMin()
{
	return min;
}

/* Get the maximum value of the slider
 *
 * PARAMS
 *   int - the maximum value of the slider
 *
 */
int ISlider::GetMax()
{
	return max;
}
