/*
 * 3dfm - 3D File Manager
 * $Id: IList.cpp,v 1.39 2003/04/17 21:41:13 kristopher Exp $
 */

/*
 * IList
 */

/* system includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <IList.h>
#include <IListItem.h>
#include <Prefs.h>

#define OVERHEAD 6
#define CHAR_WIDTH 2
#define CHAR_CNT(x) (int) ((x - OVERHEAD) / CHAR_WIDTH)
#define HEADER 5

/* Construct a new IList from the passed in values.
 * constructor does not do any parameter checking at this time.
 *
 * PARAMS
 *   location - the top left corner of the box
 *   size - the size in the X, Y and Z directions
 *   paneColor - the color of the pane
 */
IList::IList(GPoint location, GPoint size, GColor paneColor, bool Checkbox) {
    this->location = location;
    this->size = size;
    this->paneColor = paneColor;
    line_max = (int) (((size.getY() - 3.0) / 4.0) + 1.0);
    if (line_max < 5)
	line_max = 5;
    this->size.setY((line_max * 4) + 0.5);
    if (CHAR_CNT(size.getX()) < 5) {
	this->size.setX(5 * CHAR_WIDTH + OVERHEAD);
    }
    char_max = CHAR_CNT(this->size.getX());
    item_cnt = 0;
    head = NULL;
    top = head;
    next_idx = 0;
    remaining_checks = 0;

    Checkboxes = Checkbox;

    //Set external flags to default
    CloseList = false;
    DoNavigate = false;

    //Set animation flags
    Animate = true;
    aa = 0.0;
    dohighlight = false;
    highlight = 0;

    //Set reading flags
    ReRead = false;

    Title = (char*)malloc(sizeof (char) * 2 + 1);
    Title = "";

    Items = NULL;
}

/* Destroys the IList item, and data structure
 *
 * PARAMS
 *   none
 *
 */
IList::~IList(void) {
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
    int array_size = (line_max * 2) + 1;
    for (int i = 0; i < array_size; i++) {
 	delete Items[i];
 	Items[i] = NULL;
    }
    delete head;
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

/* Draw the actual IList-box object on the screen
 *
 * PARAMS
 *   none
 *
 */
int IList::execGL(void) {

    //Customizable flags
   // if (item_cnt <= Prefs::breadth)
    //	Checkboxes = false;

    //Creates the clickable GRects
    CreateClickable();

    //Are we displaying check boxes?
    if (Checkboxes)
    	shift_distance = 3.0;
	else
	shift_distance = 0.5;


    if (Checkboxes)
    	CheckControl();

    //Run animation
    if (Animate)
    {
    	Animate = AnimatePane();
	return Animate;
    }

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


    //Perform the color contrasting algorithm so the buttons can be see
    //on top of the main pane
    GColor previous = paneColor;
    (paneColor + (GLfloat) -0.4).execGL();


    //Creating title bar and check control
    //Creating the divider between the title bar and list
    //Get the current line width
    GLfloat old_line_width;
    glGetFloatv(GL_LINE_WIDTH, &old_line_width);
    //Set the new line width for the title bar border
    glLineWidth((GLfloat) 3.0);
    //Create the border line
    glBegin(GL_LINES);
    glVertex3f(location.getX(), location.getY() - HEADER,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY() - HEADER,
	       location.getZ() + 0.1);
    glEnd();

    if (Checkboxes)
    {
    //Adjust line width for the check mark graphic
    glLineWidth((GLfloat) 2.0);
    //Creating the check mark graphic
    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX() + 0.5, location.getY() - 2.0,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + 2.0, location.getY() - 4.0,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + 3.75, location.getY() - 0.5,
	       location.getZ() + 0.1);
    glEnd();
    }
    //Restore the old line width
    glLineWidth(old_line_width);

    //Drawing the checks remaining number

    if (Checkboxes)
    {
    //Need to pust matrix to draw text
    glPushMatrix();
    //Move the main coordinates to the top left corner or the box
    glTranslatef(location.getX(), location.getY(), 0.0);

    //Create white text color
    glColor4f(1.0, 1.0, 1.0, 1.0);
    char numplace[20];

    //Write the integer into a string
    sprintf (numplace, "%lld", remaining_checks);


    /*
     * If the label entered will not fit in the allotted space, cut it back so that it fits
     * and add 3 dots to the end.
     */

    //Count how long the string is and create another one which we will use for printing later
    int l_checks = strlen(numplace);
    char* new_numplace;
    //Allocation enough for a 19 digit number, as large as we can get
    new_numplace = (char*)malloc(sizeof (char) * 19 + 1);

    //How much room is there?
    int available = (int)((int)(size.getX() - 7.75) / 2);
    if (l_checks > available)
	{
	//If we don't have room perform the edit...
	available = available - 3;
	strncpy(new_numplace, numplace, (size_t)available);
	new_numplace[available+1] = 0;
	strcpy (new_numplace, new_numplace);
	strcat (new_numplace, "...");
	}
	else
	{
	    //Otherwise print the entire string
	    new_numplace = (char*)malloc(sizeof (char) * 19 + 1);
	    strcpy(new_numplace, numplace);
	}

    //Set text printing location and print the number
    glRasterPos3f(4.75, -3, 0.1);
    ourPrintString(GLUT_BITMAP_8_BY_13, new_numplace);

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();

    }
    else
    {
    //Need to pust matrix to draw text
    glPushMatrix();
    //Move the main coordinates to the top left corner or the box
    glTranslatef(location.getX(), location.getY(), 0.0);

    //Create white text color
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glRasterPos3f(0.5, -3, 0.1);
    ourPrintString(GLUT_BITMAP_8_BY_13, Title);

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();
    }

    //Set the color back to the pane color
    paneColor = previous;
    (paneColor + (GLfloat) -0.4).execGL();

    glBegin(GL_QUADS);
    //Creating Exit button
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - 0.5,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - 4,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 0.5, location.getY()  - 4,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 0.5, location.getY() - 0.5,
	       location.getZ() + 0.1);
    glEnd();

    //Create the semi-transparent white text color
    glLineWidth((GLfloat) 2.5);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(location.getX() + size.getX() - 2.50, location.getY() - 1.0,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 1.0, location.getY() - 3.50,
	       location.getZ() + 0.1);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(location.getX() + size.getX() - 1.0, location.getY() - 1.0,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 2.50, location.getY() - 3.50,
	       location.getZ() + 0.1);
    glEnd();

    //Set the color back to the pane color
    paneColor = previous;
    (paneColor + (GLfloat) -0.4).execGL();

    //Revert back to the original line width
    glLineWidth(old_line_width);

    glBegin(GL_QUADS);
    //Creating scroll down button
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - size.getY() + 4,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - size.getY(),
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY(),
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY() + 4,
	       location.getZ() + 0.1);
    glEnd();

    glBegin(GL_QUADS);
    //Creating scroll up button
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - HEADER,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - 4 - HEADER,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY()  - 4 - HEADER,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY() - HEADER,
	       location.getZ() + 0.1);
    glEnd();

    glBegin(GL_QUADS);
    //Creating page down button
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - size.getY() + 8,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - size.getY() + 4,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY() + 4,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY() - size.getY() + 8,
	       location.getZ() + 0.1);
    glEnd();

    glBegin(GL_QUADS);
    //Creating page up button
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - 4 - HEADER,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX() - 3, location.getY() - 8 - HEADER,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY()  - 8 - HEADER,
	       location.getZ() + 0.1);
    glVertex3f(location.getX() + size.getX(), location.getY() - 4 - HEADER,
	       location.getZ() + 0.1);
    glEnd();

    glColor4f(1.0, 1.0, 1.0, 0.6);
    //Creating scroll down triangle even though the chevron looks better
    glBegin(GL_TRIANGLES);
    glVertex3f(location.getX() + size.getX() - 2.75, location.getY() - size.getY() + 3.5,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 1.5, location.getY() - size.getY() + .5,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 0.25 , location.getY() - size.getY() + 3.5,
	       location.getZ() + 0.2);
    glEnd();

    //Creating scroll up triangle
    glBegin(GL_TRIANGLES);
    glVertex3f(location.getX() + size.getX() - 2.75, location.getY() - 3.5 - HEADER,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 1.5, location.getY() - .5 - HEADER,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 0.25 , location.getY() - 3.5 - HEADER,
	       location.getZ() + 0.2);
    glEnd();

    //Creating page down double chevron
    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX() + size.getX() - 2.75, location.getY() - size.getY() + 5.75,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 1.5, location.getY() - size.getY() + 4.25,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 0.25 , location.getY() - size.getY() + 5.75,
	       location.getZ() + 0.2);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX() + size.getX() - 2.75, location.getY() - size.getY() + 7.75,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 1.5, location.getY() - size.getY() + 6.25,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 0.25 , location.getY() - size.getY() + 7.75,
	       location.getZ() + 0.2);
    glEnd();

    //Creating page up double chevron
    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX() + size.getX() - 2.75, location.getY() - 5.75 - HEADER,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 1.5, location.getY() - 4.25 - HEADER,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 0.25 , location.getY() - 5.75 - HEADER,
	       location.getZ() + 0.2);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX() + size.getX() - 2.75, location.getY() - 7.75 - HEADER,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 1.5, location.getY() - 6.25- HEADER,
	       location.getZ() + 0.2);
    glVertex3f(location.getX() + size.getX() - 0.25 , location.getY() - 7.75 - HEADER,
	       location.getZ() + 0.2);
    glEnd();

    //Set the color back to the pane color
    paneColor = previous;

    //Create the semi-transparent white text color
    glColor4f(1.0, 1.0, 1.0, 0.5);

    //Need to pust matrix to draw text
    glPushMatrix();
    //Move the main coordinates to the top left corner or the box
    glTranslatef(location.getX(), location.getY(), 0.0);

    //Initialize settings for the first line
    int y = -3 - HEADER;
    int line = line_max;
    IListItem *cur = top;
    int count = 1;

    //Create the text and the check-boxes from the top down
    while ((cur != NULL) && (line > 1)) {

	if ((count == highlight) && (dohighlight))
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
	}
	else
	{
		glColor4f(1.0, 1.0, 1.0, 0.5);
	}

	glRasterPos3f(shift_distance, y, 0.1);
	ourPrintString(GLUT_BITMAP_8_BY_13, cur->getName());


	//Creating checked check box
	if (Checkboxes)
	{
	glColor4f(1.0, 1.0, 1.0, 0.5);
	if (cur->getChecked()) {
	    glBegin(GL_QUADS);
	    glVertex3f(0.5, y+2 , 0.1);
	    glVertex3f(0.5, y , 0.1);
	    glVertex3f(2.5, y , 0.1);
	    glVertex3f(2.5, y+2 , 0.1);
	    glEnd();
	}
	else
	{
	    //Creating non-checked check box
	    glBegin(GL_LINE_STRIP);
	    glVertex3f(0.5, y+2 , 0.1);
	    glVertex3f(2.5, y+2 , 0.1);
	    glVertex3f(2.5, y , 0.1);
	    glVertex3f(0.5, y , 0.1);
	    glVertex3f(0.5, y+2 , 0.1);
	    glEnd();
	}
	}
	//Go to next line and next item in the list
	y -= 4;
	cur = cur->getNext();
	--line;
	++count;
    }

    //Done drawing text, go back into normal coordinate system
    glPopMatrix();

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
void IList::add(char *name, char *name_l, int checked) {

    //If this is the first item
    if (head == NULL) {
	//Create the head node...
	head = new IListItem(name, name_l, char_max, next_idx++, checked);
	top = head;
	inspos = head;
    } else {
        //Otherwise add to the end of the list and add the previous pointer
	//Note:  this is a doubly linked list chosen for performance
	IListItem *cur = inspos;
	IListItem *pcur = inspos;
	while (cur != NULL) {
	    pcur = cur;
	    cur = cur->getNext();
	}
	pcur->setNext(new IListItem(name, name_l, char_max,
				    next_idx++, checked));
	pcur->getNext()->setPrev(pcur);
	inspos = pcur->getNext();
	
    }
    item_cnt++;
}

/* Performs the scroll operation based on the passed in argument
 *
 * PARAMS
 *   offset - which direction and how far to scroll
 *
 */
void IList::scroll(int offset) {
    /* offset configuration:
     *   -> A negative sign means to scroll UP
     *   -> A positive number means to scroll DOWN
     *   -> A number greater than +/- 1 results in a PAGE
     */


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
	    //Page up based on the size of offset
	    pcur = cur;
	    cur = cur->getPrev();
	    ++change;
	}
	if (offset > 0) {
	    //Page down based on the size of offset
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


/* Checks the item who index matches the passed in argument
 *
 * PARAMS
 *   idx - the number of the item to be checked
 *
 */
void IList::check(int idx) {

    //Check for an invalid item number
    if ((idx < 1) || (idx > item_cnt)) {
	return;
    }

    //Set to head of list and beginning of indexes
    //Note: the indexes begin at 1
    ReRead = true;
    IListItem *cur = head;
    int i = 1;
    while (cur != NULL) {
	//Once we find the matching index...
	if (i++ == idx) {
            //Check and see if it is checked...
	    int ischecked = cur->getChecked();
	    if (ischecked == 1)
	    {
	            //If it is checked, then uncheck it
	    	    cur->setCheck(0);
		    remaining_checks++;
            }
	    else
	    {
	    //Otherwise check it
	    if (remaining_checks > 0)
	    	    cur->setCheck(1);
	    continue;
	    }
	}
	//Traverse through the list
	cur = cur->getNext();
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
void IList::clicked(GPoint pt) {

    if (Items[4]->PtinRect(pt)) {
	CloseList = true;
    }

    if (item_cnt == 0) {
	return;
    }

    /* is this us? */
    bool hit;
    int count;
    for (count = 0; count <= ((line_max * 2) + 1); count++)
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
			else if (count == 4)
				CloseList = true;
			else if ((Checkboxes) && (count >= 5) && (count <= (line_max*2) + 1))
				{
					/*
			 		*If we hit something that is not a control button we need to
			 		*fine out which list item it was.
			 		*/
					int temp;
					temp = count % 2;  //If it is odd it is a list item
					if (temp != 0)
					{
						/*
					 	*Get the top index, add count, mines the control buttons,
					 	*and divided by two (checkboxes and listitems)
					 	*/
						int idx = top->getIdx();
						idx = idx + ((count - 5) / 2);
						Navigate(idx+1);
					}
					else
					{
						/*
					 	*Otherwise perform the same operation to find the check
					 	*Note: the minus five because in the order of the data
					 	*      structure, ListItems come before the checkboxes
					 	*/
						int idx2 = top->getIdx();
						idx2 = idx2 + ((count - 5) / 2);
						//Add one because indexes are based on 1 - count is based on 0
						check(idx2+1);
						//Redraw the screen so we can see the check
						glutPostRedisplay();
					}
				}
				else if ((!Checkboxes) && (count >= 5) && (count <= (line_max*2)+1))
					{
						/*
						 *If we hit something that is not a control button we need to
						 *fine out which list item it was.
						 *Get the top index, add count, minus the control buttons.
			 			 */
						int idx = top->getIdx();
						idx = idx + (count - 5);
						Navigate(idx+1);
					}
	}
}

/* Draw the clickable GRects, and put them into the data structure
 *
 * PARAMS
 *   none
 *
 */
void IList::CreateClickable()
{
    //Create the size of the array based on the size of the Listbox
    int array_size;
    if (Checkboxes)
    	array_size = (line_max * 2) + 5;
	else
	array_size = line_max + 5;

    //Create the dynamic array based on the calculated array size
    Items = (GRect **) new GRect[array_size];

    //Creating scroll down button
    Items[0] = new GRect(GPoint(location.getX() + size.getX() - 3,
				location.getY() - size.getY() + 4,
				location.getZ() + 0.1),
			 GPoint(location.getX() + size.getX(),
				location.getY() - size.getY(),
				location.getZ() + 0.1));

    //Creating scroll up button
    Items[1] = new GRect(GPoint(location.getX() + size.getX() - 3,
				location.getY() - HEADER,
				location.getZ() + 0.1),
			 GPoint(location.getX() + size.getX(),
				location.getY()  - 4 - HEADER,
				location.getZ() + 0.1));

    //Creating page down button
    Items[2] = new GRect(GPoint(location.getX() + size.getX() - 3,
				location.getY() - size.getY() + 8,
				location.getZ() + 0.1),
			 GPoint(location.getX() + size.getX(),
				location.getY()	- size.getY() + 4,
				location.getZ() + 0.1));

    //Creating page up button
    Items[3] = new GRect(GPoint(location.getX() + size.getX() - 3,
				location.getY() - 4 - HEADER,
				location.getZ() + 0.1),
			 GPoint(location.getX() + size.getX(),
				location.getY()  - 8 - HEADER,
				location.getZ() + 0.1));

    //Creating exit button
    Items[4] = new GRect(GPoint(location.getX() + size.getX() - 3,
    			location.getY() - 0.5,
	       		location.getZ() + 0.1),
			GPoint (location.getX() + size.getX() - 0.5,
			location.getY()  - 4,
			location.getZ() + 0.1));

    if (Checkboxes)
    {
	for (int count = 5, y = 0; count <= ((line_max * 2) + 1); count+=2, y-=4) // + 4
    	{
    		//Creating the Listitem GRect
		Items[count] = new GRect(GPoint(location.getX() + 3.0,
						location.getY()+ y - HEADER,
						0.1),
						GPoint(location.getX() + size.getX() - 3,
						location.getY() + y - 4 - HEADER,
						0.1));
		//Creating non-checked check box
		Items[count+1] = new GRect(GPoint(location.getX() + 0.5,
						  location.getY() + y - 1 - HEADER,
						  0.1),
					   GPoint(location.getX() + 2.5,
						  location.getY() + y - 3 - HEADER,
						  0.1));
    	}
    }
    else
        {
	    for (int count = 5, y = 0; count <= ((line_max * 2) + 1); count+=1, y-=4)
    		{
	    		//Creating the Listitem GRect
			Items[count] = new GRect(GPoint(location.getX() + 0.5,
							location.getY()+ y - HEADER,
							0.1),
							GPoint(location.getX() + size.getX() - 3,
							location.getY() + y - 4 - HEADER,
							0.1));
    		}
    	}
}

/* Calculates how many checks are remaining and how many are to
 * be issued.  It also compensates from breadth resizing outside
 * of the GUI
 *
 * PARAMS
 *   none
 *
 */
void IList::CheckControl()
{
	/*
	 * If there are less items then the specified breadth then the
	 * max number of checks to be allocated is the number of sub-folders
         */
	if (item_cnt < Prefs::breadth)
		remaining_checks = item_cnt;
	else
		remaining_checks = Prefs::breadth;

	int checked;
	long long int temp_checks;
	temp_checks = remaining_checks - 1;

	//Start at the beginning of the list
	IListItem *cur = head;
	IListItem *pcur = head;
	/* Traverse through the list and count how many checks are allocated
	 * everytime a check is found it is subtracted from the total number
	 * available which is caluclated at the beginning of this function.
	 *
	 * In the event that the user would add folders manually to the check
	 * datafile and all the available checks have been allocated.   If this
	 * happens then all of the checks found after the last check has been
	 * allocated will be set as unchecked.
	 */
	while (cur != NULL)
	{
	    pcur = cur;
	    checked = cur->getChecked();
	    if (checked == 1)
	    {
	    	temp_checks--;
		//If we are out of checks the turn the remaining ones to unchecked
		if (temp_checks < 0)
		{
			temp_checks = 0;
			cur->setCheck(0);
		}
	    }
	    //Traverse throught the list
	    cur = cur->getNext();
	}
	//Set how many checks remain
	remaining_checks = temp_checks;
}

void IList::Navigate(int idx)
{
    //Check for an invalid item number
    if ((idx < 1) || (idx > item_cnt)) {
	DoNavigate = false;
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
	    DoNavigate = true;
	    NavigateLocation = cur->getLongName();
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
bool IList::AnimatePane()
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
    glVertex3f(location.getX(), location.getY(), location.getZ());
    glVertex3f(location.getX() + size.getX(),
	       location.getY(), location.getZ());
    glVertex3f(location.getX() + size.getX(),
	       location.getY() - size.getY(),
	       location.getZ());
    glVertex3f(location.getX(), location.getY() - size.getY(),
	       location.getZ());

    glEnd();

    return StillAnimate;
}

void IList::mouseMove(GPoint pt)
{
    //fprintf(stderr, "Point:%f\t%f\n", pt.getX(), pt.getY());
    /* is this us? */
    if(Items == NULL)
    	return;

    bool hit;
    int count;
    for (count = 0; count <= ((line_max * 2) + 1); count++)
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

     if (hit && (Checkboxes) && (count >= 5) && (count <= (line_max*2)+ 5))
     {
	/*
	 *If we hit something that is not a control button we need to
	 *fine out which list item it was.
	 */
	int temp;
	temp = count % 2;  //If it is odd it is a list item
	if (temp != 0)
	{
		/*
	 	*Get the top index, add count, mines the control buttons,
	 	*and divided by two (checkboxes and listitems)
	 	*/

		dohighlight = true;
		highlight = (count/2) - 1;
		glutPostRedisplay();
		return;
	}
     }  else if ((!Checkboxes) && (count >= 5) && (count <= (line_max)+5))
	{
		/*
		 *If we hit something that is not a control button we need to
		 *fine out which list item it was.
		 *Get the top index, add count, minus the control buttons.
			 */
		dohighlight = true;
		highlight = count - 4;
		glutPostRedisplay();
		return;
	}

	dohighlight = false;
	return;
}

void IList::SetTitle(char* NewTitle)
{

    //Count how long the string is and create another one which we will use for printing later
    int l_checks = strlen(NewTitle);
    char* new_title;
    //Allocation enough for a 19 digit number, as large as we can get
    new_title = (char*)malloc(sizeof (char) * l_checks + 1);

    //How much room is there?
    int available = (int)((int)(size.getX() - 7.75) / 2);
    if (l_checks > available)
	{
	//If we don't have room perform the edit...
	available = available - 3;
	strncpy(new_title, NewTitle, (size_t)available);
	new_title[available+1] = 0;
	strcat (new_title, "...");
	int length = strlen(new_title);
	Title = (char*)malloc(sizeof (char) * length + 1);
	strcpy (Title, new_title);
	}
	else
	{
	    //Otherwise print the entire string
	    Title = (char*)malloc(sizeof (char) * l_checks + 1);
	    strcpy (Title, NewTitle);
	}
}
