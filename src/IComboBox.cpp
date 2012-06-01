/*
 * 3dfm - 3D File Manager
 * $Id: IComboBox.cpp,v 1.10 2003/04/17 21:41:13 kristopher Exp $
 */

/*
 * IComboBox
 */

/* system includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <IComboBox.h>
#include <IListItem.h>
#include <Prefs.h>

#define OVERHEAD 6
#define CHAR_WIDTH 2
#define CHAR_CNT(x) (int) ((x - OVERHEAD) / CHAR_WIDTH)
#define HEADER 5
#define ZDIR 0.1

/* Construct a new IComboBox from the passed in values.
 * constructor does not do any parameter checking at this time.
 *
 * PARAMS
 *   location - the top left corner of the box
 *   size - the size in the X, Y and Z directions
 *   paneColor - the color of the pane
 */
IComboBox::IComboBox(GPoint location, GPoint size, GColor paneColor, int itemNumber)
{
    this->location = location;
    this->size = size;
    this->paneColor = paneColor;
    line_max = itemNumber;
    if (line_max < 5)
	line_max = 5;
    //this->size.setY(line_max * 4);
    if (CHAR_CNT(size.getX()) < 5) {
	this->size.setX(5 * CHAR_WIDTH + OVERHEAD);
    }
    char_max = CHAR_CNT(this->size.getX());
    item_cnt = 0;
    head = NULL;
    top = head;
    next_idx = 0;

    Text = (char*)malloc(sizeof (char) * 2 + 1);
    Text = "";

    TextLongName = (char*)malloc(sizeof(char) * 2 + 1);
    TextLongName = "";

    //Set ListBox drawing variables
    ListBoxLocation = GPoint(location.getX(), location.getY() - size.getY() - 0.5, location.getZ());
    ListBoxSize = GPoint(size.getX() - 0.5, itemNumber * 4, size.getZ());

    //Set external flags to default
    CloseList = false;
    OpenList = false;
    TitleChange = false;
    FocusLock = false;

    //Set animation flags
    Animate = true;
    aa = 0.0;
    dohighlight = false;
    highlight = 0;

    //Set reading flags
    ReRead = false;

    Items = NULL;
}

/* Destroys the IComboBox item, and data structure
 *
 * PARAMS
 *   none
 *
 */
IComboBox::~IComboBox(void) {
    IListItem *curr = head;
    while (curr != NULL) {
	if (curr->getChecked()) {
	    Prefs::addCheck(curr->getLongName());
	} else {
	    Prefs::removeCheck(curr->getLongName());
	}
#ifdef _DEBUG_FILTER
	Prefs::dumpChecked();
#endif
	curr = curr->getNext();
    }
    int array_size = (line_max * 2) + 4;
    for (int i = 0; i < array_size; i++) {
 	delete Items[i];
 	Items[i] = NULL;
    }
    delete head;
    
    free (Text);
    free (TextLongName);

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
   
   //fprintf (stderr, "Called:  %s\n", str);

   for(i=0;i<l;i++)
      glutBitmapCharacter(font,*str++);


}

/* Draw the actual IComboBox-box object on the screen
 *
 * PARAMS
 *   none
 *
 */
int IComboBox::execGL(void) {

    //Creates the clickable GRects
    CreateClickable();

    //Set the paneColor to the active color
    paneColor.execGL();

    //Draw the main pane
    glBegin(GL_QUADS);
    glVertex3f(location.getX(), location.getY(), location.getZ());
    glVertex3f(location.getX() + size.getX(),
	       location.getY(), ZDIR);
    glVertex3f(location.getX() + size.getX(),
	       location.getY() - size.getY(),
	       ZDIR);
    glVertex3f(location.getX(), location.getY() - size.getY(),
	       ZDIR);

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

    //Perform the color contrasting algorithm so the button can be seen
    //on top of the main pane

    GColor previous = paneColor;
    (previous + (GLfloat) -0.4).execGL();

    glBegin(GL_QUADS);
    //Creating drop down list button
    glVertex3f(location.getX() + size.getX() - 3.5, location.getY() - 0.5,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 3.5, location.getY() - size.getY() + 0.5,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 0.5, location.getY() - size.getY() + 0.5,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 0.5, location.getY() - 0.5,
	       location.getZ() + 0.1);
    glEnd();

    glColor4f(1.0, 1.0, 1.0, 0.6);
    //Creating down triangle even though the chevron looks better
    glBegin(GL_TRIANGLES);
    glVertex3f(location.getX() + size.getX() - 3.0, location.getY() - 1.0,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 2.0, location.getY() - size.getY() + .75,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 1.0 , location.getY() - 1.0,
	       location.getZ() + 0.2);
    glEnd();


    //Create the semi-transparent white text color
    glColor4f(1.0, 1.0, 1.0, 0.7);

    //Need to pust matrix to draw text
    glPushMatrix();
    //Move the main coordinates to the top left corner or the box
    glTranslatef(location.getX(), location.getY(), 0.1);

    //Initialize settings for the first line

    glRasterPos3f(1.0, -4.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, Text);

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();

    if (OpenList)
    {

    paneColor.execGL();

    //Run animation
    if (Animate)
    {
    	Animate = AnimatePane();
	return Animate;
    }

    glBegin(GL_QUADS);
    //Creating ListBox pane
    glVertex3f(ListBoxLocation.getX(), ListBoxLocation.getY(), ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX(), ListBoxLocation.getY() - ListBoxSize.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY() - ListBoxSize.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glEnd();

    //Perform the color contrasting algorithm so the buttons can be see
    //on top of the main pane
    GColor previous = paneColor;
    (paneColor + (GLfloat) -0.4).execGL();


    glBegin(GL_QUADS);
    //Creating scroll down button
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 3, ListBoxLocation.getY() - ListBoxSize.getY() + 4,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 3, ListBoxLocation.getY() - ListBoxSize.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY() - ListBoxSize.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY() - ListBoxSize.getY() + 4,
	       ListBoxLocation.getZ() + 0.1);
    glEnd();

    glBegin(GL_QUADS);
    //Creating scroll up button
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 3, ListBoxLocation.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 3, ListBoxLocation.getY() - 4,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY()  - 4,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glEnd();

    glBegin(GL_QUADS);
    //Creating page down button
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 3, ListBoxLocation.getY() - ListBoxSize.getY() + 8,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 3, ListBoxLocation.getY() - ListBoxSize.getY() + 4,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY() - ListBoxSize.getY() + 4,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY() - ListBoxSize.getY() + 8,
	       ListBoxLocation.getZ() + 0.1);
    glEnd();

    glBegin(GL_QUADS);
    //Creating page up button
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 3, ListBoxLocation.getY() - 4,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 3, ListBoxLocation.getY() - 8,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY()  - 8,
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY() - 4,
	       ListBoxLocation.getZ() + 0.1);
    glEnd();

    glColor4f(1.0, 1.0, 1.0, 0.6);
    //Creating scroll down triangle even though the chevron looks better
    glBegin(GL_TRIANGLES);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 2.75, ListBoxLocation.getY() - ListBoxSize.getY() + 3.5,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 1.5, ListBoxLocation.getY() - ListBoxSize.getY() + .5,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 0.25 , ListBoxLocation.getY() - ListBoxSize.getY() + 3.5,
	       ListBoxLocation.getZ() + 0.2);
    glEnd();

    //Creating scroll up triangle
    glBegin(GL_TRIANGLES);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 2.75, ListBoxLocation.getY() - 3.5,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 1.5, ListBoxLocation.getY() - .5,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 0.25 , ListBoxLocation.getY() - 3.5,
	       ListBoxLocation.getZ() + 0.2);
    glEnd();

    //Creating page down double chevron
    glBegin(GL_LINE_STRIP);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 2.75, ListBoxLocation.getY() - ListBoxSize.getY() + 5.75,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 1.5, ListBoxLocation.getY() - ListBoxSize.getY() + 4.25,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 0.25 , ListBoxLocation.getY() - ListBoxSize.getY() + 5.75,
	       ListBoxLocation.getZ() + 0.2);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 2.75, ListBoxLocation.getY() - ListBoxSize.getY() + 7.75,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 1.5, ListBoxLocation.getY() - ListBoxSize.getY() + 6.25,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 0.25 , ListBoxLocation.getY() - ListBoxSize.getY() + 7.75,
	       ListBoxLocation.getZ() + 0.2);
    glEnd();

    //Creating page up double chevron
    glBegin(GL_LINE_STRIP);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 2.75, ListBoxLocation.getY() - 5.75,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 1.5, ListBoxLocation.getY() - 4.25,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 0.25 , ListBoxLocation.getY() - 5.75,
	       ListBoxLocation.getZ() + 0.2);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 2.75, ListBoxLocation.getY() - 7.75,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 1.5, ListBoxLocation.getY() - 6.25,
	       ListBoxLocation.getZ() + 0.2);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX() - 0.25 , ListBoxLocation.getY() - 7.75,
	       ListBoxLocation.getZ() + 0.2);
    glEnd();

    //Set the color back to the pane color
    paneColor = previous;

    //Create the semi-transparent white text color
    glColor4f(1.0, 1.0, 1.0, 0.5);

    //Need to pust matrix to draw text
    glPushMatrix();
    //Move the main coordinates to the top left corner or the box
    glTranslatef(ListBoxLocation.getX(), ListBoxLocation.getY(), ZDIR);

    //Initialize settings for the first line
    int y = -3;
    int line = line_max;
    IListItem *cur = top;
    int count = 1;

    //Create the text and the check-boxes from the top down
    while ((cur != NULL) && (line > 0)) {

	if ((count == highlight) && (dohighlight))
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
	}
	else
	{
		glColor4f(1.0, 1.0, 1.0, 0.5);
	}

	glRasterPos3f(0.5, y, 0.1);
	ourPrintString(GLUT_BITMAP_8_BY_13, cur->getName());


	//Creating checked check box

	//Go to next line and next item in the list
	y -= 4;
	cur = cur->getNext();
	--line;
	++count;
    }

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();
    }

    return 0;
}

/* Add the item to the data structure list and the viewable list
 *
 * PARAMS
 *   *name - the short name of the item to be added to the visible list
 *   *name_l - the long name to be added to the data structure
 *   checked - mark whether the item added is checked or not
 *
 */
void IComboBox::add(char *name) {

    //If this is the first item
    if (head == NULL) {
	//Create the head node...
	head = new IListItem(name, name, char_max, next_idx++,0);
	top = head;
    } else {
        //Otherwise add to the end of the list and add the previous pointer
	//Note:  this is a doubly linked list chosen for performance
	IListItem *cur = head;
	IListItem *pcur = head;
	while (cur != NULL) {
	    pcur = cur;
	    cur = cur->getNext();
	}
	pcur->setNext(new IListItem(name, name, char_max,
				    next_idx++, 0));
	pcur->getNext()->setPrev(pcur);
    }
    item_cnt++;
}

/* Performs the scroll operation based on the passed in argument
 *
 * PARAMS
 *   offset - which direction and how far to scroll
 *
 */
void IComboBox::scroll(int offset) {
    /* offset configuration:
     *   -> A negative sign means to scroll UP
     *   -> A positive number means to scroll DOWN
     *   -> A number greater than +/- 1 results in a PAGE
     */

     if (head == NULL)
     	return;

    int change;
    if (offset < -1)
	change = -line_max;
    else if (offset > 1)
	change = line_max;
    else
	change = offset;

    //Set to the top of the viewable list
    IListItem *cur = top;
    IListItem *pcur = top;

    //If we are at the bottom of the list then do not go down
    if ((offset > 0) && (top->getIdx() > item_cnt - line_max - 1)) {
	change = 0;
    }
    //Perform paging and check to not go past the end of the list
    while ((cur != NULL) && (change != 0))  {
	if (offset < 0) {
	    //Page up based on the ListBoxSize of offset
	    pcur = cur;
	    cur = cur->getPrev();
	    ++change;
	}
	if (offset > 0) {
	    //Page down based on the ListBoxSize of offset
	    pcur = cur;
	    cur = cur->getNext();
	    --change;
	}
    }

    //Check for end of list and set back to top
    if (cur == NULL) {
	top = pcur;
    } else {
	top = cur;
    }
}


/* Figures out if a point was clicked on the list box and if
 * it was then we determine which object and call the appropriate
 * handler.
 *
 * PARAMS
 *   pt - the point clicked on the screen
 *
 */
bool IComboBox::clicked(GPoint pt, int ClickType) {

    if (ClickType == GLUT_DOWN)
    {

    /* is this us? */
    TitleChange = false;
    bool hitDown;
    bool hit;
    bool hitDevice;
    int count;

    hitDown = DownButton->PtinRect(pt);
    hitDevice = Device->PtinRect(pt);
    if (hitDown && !OpenList)
    {
    	OpenList = true;
	FocusLock = true;
	return hitDown;
    }

    if (hitDevice)
    {
    	FocusLock = true;
	return hitDevice;
    }


    if (OpenList)
    {
    for (count = 0; count <= (line_max + 3/*4*/); count++)
    {
        //Run through the list of GRects to see if we have a hit
    	hit = Items[count]->PtinRect(pt);
	if (hit)
	{
	   //We have a hit now go see who is was
	   FocusLock = true;
	   break;
	}
    }

    /* Setup for master controls
     *   0 = Scroll down
     *   1 = Scroll up
     *   2 = Page down
     *   3 = Page up
     */

     if (hit)
     {
     if (count == 0)
	scroll(1);
	else if (count == 1)
	     scroll(-1);
	     else if (count == 2)
	          scroll(5);
		  else if (count == 3)
		  	scroll(-5);
				else if ((count >= 4) && (count <= line_max + 3/*4*/))
				{
					/*
					 *If we hit something that is not a control button we need to
					 *fine out which list item it was.
					 *Get the top index, add count, minus the control buttons.
			 		 */
					int idx = top->getIdx();
					idx = idx + (count - 3);
					SetAsText(idx);
				}}
					else
					{
						OpenList = false;
						Animate = true;
						aa = 0.0;
						FocusLock = false;
					}
	return hit;
	}
   OpenList = false;
   Animate = true;
   aa = 0.0;
   FocusLock = false;
   return hit;
   }

    return false;
}

/* Draw the clickable GRects, and put them into the data structure
 *
 * PARAMS
 *   none
 *
 */
void IComboBox::CreateClickable()
{

    Device = new GRect(GPoint(location.getX(), location.getY(), location.getZ() + 0.1),
 			   GPoint(location.getX() + size.getX(),
			   	  location.getY() - size.getY(),
				  location.getZ() + 0.1));

    DownButton = new GRect(GPoint(location.getX() + size.getX() - 3.5,
    				  location.getY() - 0.5,
				  location.getZ() + 0.1),
 			   GPoint(location.getX() + size.getX() - 0.5,
			   	  location.getY() - size.getY() + 0.5,
				  location.getZ() + 0.1));

    if (OpenList)
    {
    	//Create the ListBoxSize of the array based on the ListBoxSize of the Listbox
    	int array_ListBoxSize = line_max + 4;

    	//Create the dynamic array based on the calculated array ListBoxSize
	Items = (GRect **) new GRect[array_ListBoxSize];

    	//Creating scroll down button
    	Items[0] = new GRect(GPoint(ListBoxLocation.getX() + ListBoxSize.getX() - 3,
					ListBoxLocation.getY() - ListBoxSize.getY() + 4,
					ListBoxLocation.getZ() + 0.1),
				 GPoint(ListBoxLocation.getX() + ListBoxSize.getX(),
					ListBoxLocation.getY() - ListBoxSize.getY(),
					ListBoxLocation.getZ() + 0.1));

    	//Creating scroll up button
    	Items[1] = new GRect(GPoint(ListBoxLocation.getX() + ListBoxSize.getX() - 3,
					ListBoxLocation.getY(),
					ListBoxLocation.getZ() + 0.1),
				 GPoint(ListBoxLocation.getX() + ListBoxSize.getX(),
					ListBoxLocation.getY()  - 4,
					ListBoxLocation.getZ() + 0.1));

    	//Creating page down button
    	Items[2] = new GRect(GPoint(ListBoxLocation.getX() + ListBoxSize.getX() - 3,
					ListBoxLocation.getY() - ListBoxSize.getY() + 8,
					ListBoxLocation.getZ() + 0.1),
				 GPoint(ListBoxLocation.getX() + ListBoxSize.getX(),
					ListBoxLocation.getY()	- ListBoxSize.getY() + 4,
					ListBoxLocation.getZ() + 0.1));

    	//Creating page up button
    	Items[3] = new GRect(GPoint(ListBoxLocation.getX() + ListBoxSize.getX() - 3,
					ListBoxLocation.getY() - 4,
					ListBoxLocation.getZ() + 0.1),
				 GPoint(ListBoxLocation.getX() + ListBoxSize.getX(),
					ListBoxLocation.getY()  - 8,
					ListBoxLocation.getZ() + 0.1));


    	for (int count = 4, y = 0; count <= (line_max + 3/*4*/); count += 1, y -= 4)
    		{
			//Creating the Listitem GRect
			Items[count] = new GRect(GPoint(ListBoxLocation.getX() + 0.5,
							ListBoxLocation.getY()+ y,
							0.1),
							GPoint(ListBoxLocation.getX() + ListBoxSize.getX() - 3,
							ListBoxLocation.getY() + y - 4,
							0.1));
    		}
    }
}


void IComboBox::SetAsText(int idx)
{
    TitleChange = false;
    //Check for an invalid item number
    if ((idx < 1) || (idx > item_cnt)) {
	return;
    }

    //Set to head of list and beginning of indexes
    //Note: the indexes begin at 1
    IListItem *cur = head;
    int i = 1;
    while (cur != NULL)
    {
	//Once we find the matching index...
	if (i++ == idx)
	{
            //Get the long name of that item

	    Text = (char*) malloc(sizeof (char) * strlen(cur->getName()) + 1);
	    Text = cur->getName();
	    TextLongName = (char*) malloc(sizeof (char) * strlen(cur->getLongName()) + 1);
	    TextLongName = cur->getLongName();
	    OpenList = false;
	    Animate = true;
	    aa = 0.0;
	    TitleChange = true;
	}
	//Traverse through the list
	cur = cur->getNext();
    }
}

/* Performs the animation "fade-in" when the list is originally
 * displayed
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   bool - returns true if the pane is still animating otherwise
 *        - return false
 */
bool IComboBox::AnimatePane()
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
    	aa = aa + 0.1;
	StillAnimate = true;
    }

    //Pull in colors slected by the user
    GLfloat ar = paneColor.getRed();
    GLfloat ag = paneColor.getGreen();
    GLfloat ab = paneColor.getBlue();

    //Set to the temporary color
    glColor4f(ar, ag, ab, aa);

    //Draw the main pane with temporary colors
    glBegin(GL_QUADS);
    glVertex3f(ListBoxLocation.getX(), ListBoxLocation.getY(), ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX(), ListBoxLocation.getY() - ListBoxSize.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY() - ListBoxSize.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glVertex3f(ListBoxLocation.getX() + ListBoxSize.getX(), ListBoxLocation.getY(),
	       ListBoxLocation.getZ() + 0.1);
    glEnd();

    return StillAnimate;
}

void IComboBox::mouseMove(GPoint pt)
{
    //fprintf(stderr, "Point:%f\t%f\n", pt.getX(), pt.getY());
    /* is this us? */
    TitleChange = false;
    if(Items == NULL)
    	return;

    bool hit;
    int count;
    for (count = 0; count <= (line_max + 3/*4*/); count++)
    {
        //Run through the list of GRects to see if we have a hit
    	hit = Items[count]->PtinRect(pt);
	if (hit)
	{
	   //We have a hit now go see who is was
	   break;
	}
    }

    /* Setup for master controls
     *   0 = Scroll down
     *   1 = Scroll up
     *   2 = Page down
     *   3 = Page up
     */

     if ((count >= 4) && (count <= (line_max)+3 /*4*/))
	{
		/*
		 *If we hit something that is not a control button we need to
		 *fine out which list item it was.
		 *Get the top index, add count, minus the control buttons.
			 */
		dohighlight = true;
		highlight = count - 3;
		glutPostRedisplay();
		return;
	}

	dohighlight = false;
	return;
}

char* IComboBox::GetText()
{
	return TextLongName;
}

void IComboBox::SetText(char* NewText)
{
   Text = (char*) malloc(sizeof (char) * strlen(NewText) + 1);
   TextLongName = (char*) malloc(sizeof (char) * strlen(NewText) + 1);
   strcpy(Text, NewText);
   strcpy(TextLongName, NewText);
}

/* Add the appropriate keys to the string
 * Note: This only accepts special keys not any others
 *
 * PARAMS
 *   key - the key hit
 *
 */
void IComboBox::SendKeys(int key)
{
   //Check is any of the special keys are being held down

   //If we have focus, and Ctrl and Alt are not being pressed continue
   if (FocusLock)
   {
	switch (key)
		{
		case (GLUT_KEY_UP):
		    scroll(-1);
	    	break;

		case (GLUT_KEY_DOWN):
	    	    if (!OpenList)
		       OpenList = true;
		       else
		       scroll(1);
	    	break;

		case (GLUT_KEY_PAGE_UP):
            	    scroll(-5);
	    	break;

		case (GLUT_KEY_PAGE_DOWN):
            	    if (!OpenList)
		       OpenList = true;
		       else
		       scroll(5);
	    	break;

		default:
	    	break;
		}

   }
}
