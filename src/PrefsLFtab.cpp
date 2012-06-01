/*
 * 3dfm - 3D File Manager
 * $Id: PrefsLFtab.cpp,v 1.16 2003/04/18 21:43:45 kristopher Exp $
 */

/*
 * PrefsLFtab
 */

/* system includes */
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <PrefsLFtab.h>

#define ZDIR 0.1

/* Creates the PrefsLFtab object according to the specified parameters
 *
 * PARAMS
 *   location - the top left corner of the tab
 *   size - the size of the Look and Feel tab
 *   paneColor - the color of the tab
 *   tabTitle - the title of the tab
 *
 */
PrefsLFtab::PrefsLFtab(GPoint location, GPoint size, GColor paneColor,
		 char* tabTitle)
{
    //Pull in all values...

    this->location = location;
    this->size = size;
    this->paneColor = paneColor;
    this->tabTitle = tabTitle;

    //Setup our mouse control variables
    redraw = true;

    //Initialize all colors until changed
    colorPacket.background = GColor(0.0, 0.0, 0.0, 1.0);
    colorPacket.filelistbox = GColor(0.0, 0.0, 0.0, 1.0);
    colorPacket.folderlistbox = GColor(0.0, 0.0, 0.0, 1.0);
    colorPacket.hiddenfolders = GColor(1.0, 0.4, 0.2, 1.0);
    colorPacket.linkedfolders = GColor(0.0, 0.0, 0.0, 1.0);
    colorPacket.normalfolders = GColor(0.0, 0.0, 0.0, 1.0);
    colorPacket.parentfolder = GColor(0.0, 0.0, 0.0, 1.0);
    colorPacket.statusbar = GColor(0.0, 0.0, 0.0, 1.0);
    colorPacket.statusbartext = GColor(0.0, 0.0, 0.0, 1.0);
    colorPacket.systemfolders = GColor(0.0, 0.0, 0.0, 1.0);

    //Create the IFC objects
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

/* Draw the Look and Feel tab
 *
 * PARAMS
 *
 *
 */
bool PrefsLFtab::execGL(void) {

    //Run animation
    if (Animate)
    {
    	Animate = AnimatePane();
	//Draw all of the IFC widgets for this tab
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
    glVertex3f(location.getX(), location.getY(), ZDIR + 0.2);
    glVertex3f(location.getX(), location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 28.0, location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 28.0, location.getY(), ZDIR + 0.2);
    glEnd();

    //Push the matrix to draw all of the static text
    glPushMatrix();
    glColor4f(1.0, 1.0, 1.0, 0.9);
    glTranslatef(location.getX(), location.getY(), ZDIR);
    glRasterPos3f(1.0, -4.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, tabTitle);
    glRasterPos3f(2.0, -17.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "Breadth:");
    glRasterPos3f(2.0, -32.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "Depth:");
    glRasterPos3f(4.0, -47.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "Color Control");
    glRasterPos3f(4.0, -55.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "Select Item:");
    glRasterPos3f(5.0, -119.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "R:");
    glRasterPos3f(25.0, -119.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "G:");
    glRasterPos3f(45.0, -119.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "B:");
    glRasterPos3f(80.0, -65.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "Stored:");
    glRasterPos3f(80.0, -90.0, 0.2);
    ourPrintString(GLUT_BITMAP_8_BY_13, "Current:");
    //Done drawing text, go back into normal coordinate system
    glPopMatrix();

    //Draw the grouping line for the Color Control title
    glColor4f(0.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(location.getX() + 30.0, location.getY() - 46.0, ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX() - 2.0 , location.getY() - 46.0, ZDIR + 0.2);
    glVertex3f(location.getX() + size.getX() - 2.0, location.getY() - size.getY() + 2.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 2.0, location.getY() - size.getY() + 2.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 2.0, location.getY() - 46.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 3.0, location.getY() - 46.0, ZDIR + 0.2);
    glEnd();

    paneColor.execGL();

    //Create a new matrix so we can draw the folder examples with their
    //own environmental settings
    glPushMatrix();

    // lighting/fog
    glEnable(GL_LIGHTING);
    GLfloat ambLight[4] = {0.4, 0.4, 0.4, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambLight);
    glEnable(GL_LIGHT0);
    glEnable(GL_FOG);

    // fog
    GLfloat color[4] = {0.0, 0.2, 0.2, 1.0};
    glFogfv(GL_FOG_COLOR, color);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 10.0); //5
    glFogf(GL_FOG_END, 250.0); //150

    // draw folder preview with lighting and fog
    FFolder f1("/", 0, NULL, 4.0, GColor(0.734375, 0.539062, 0.25),
    GColor(savedColor.getRed(), savedColor.getGreen(), savedColor.getBlue()));
    f1.setLocation(GPoint(location.getX() + 95.0, location.getY() - 75.0, ZDIR + 0.1));
    f1.execGL(NULL);

    //current view
    FFolder f2("/", 0, NULL, 4.0, GColor(0.734375, 0.539062, 0.25),
    GColor(colorSelect->getcolor()));
    f2.setLocation(GPoint(location.getX() + 95.0, location.getY() - 100.0, ZDIR + 0.1));
    f2.execGL(NULL);

    // restore the matricies
    glDisable(GL_LIGHTING);
    glPopMatrix();

    //Draw all of the IFC widgets for this tab
    colorSelect->execGL();
    depthSlider->execGL();
    breadthSlider->execGL();
    depthText->execGL();
    breadthText->execGL();
    colorCombo->execGL();
    applyButton->execGL();
    redText->execGL();
    greenText->execGL();
    blueText->execGL();

    return redraw;
}

/* Create the IFC widgets to be used in the main prefs dialog
 *
 *
 * PARAMS
 *  none
 *
 */
void PrefsLFtab::CreateObjects()
{
   breadthSlider = new ISlider(GPoint(location.getX() + 20.0, location.getY() - 13.0,
   				      ZDIR + 0.2),
			       size.getX() - 35.0,
			       GColor(0.2, 0.3, 0.9, 0.8),
			       1,
			       1,
			       1000,
			       15 /*CHANGE!!! */);

   depthSlider = new ISlider(GPoint(location.getX() + 20.0, location.getY() - 28.0,
   				      ZDIR + 0.2),
			       size.getX() - 35.0,
			       GColor(0.2, 0.9, 0.3, 0.8),
			       1,
			       1,
			       3,
			       2 /*CHANGE!!! */);

   breadthText = new ITextBox(GPoint(location.getX() + size.getX() - 11.0,
   				     location.getY() - 13.0,
				     ZDIR + 0.2),
			      GPoint(10.0, 6.0, 0.0),
			      GColor(0.2, 0.3, 0.9, 0.8),
			      1);
   depthText = new ITextBox(GPoint(location.getX() + size.getX() - 11.0,
   				     location.getY() - 28.0,
				     ZDIR + 0.2),
			      GPoint(6.0, 6.0, 0.0),
			      GColor(0.2, 0.9, 0.3, 0.8),
			      1);

   redText = new ITextBox(GPoint(location.getX()  + 10.0,
   				 location.getY() - 115.0,
				 ZDIR + 0.2),
			      	 GPoint(8.0, 6.0, 0.0),
			      	 GColor(0.9, 0.2, 0.2, 0.8),
			      	 1);

   greenText = new ITextBox(GPoint(location.getX() + 30.0,
			  	   location.getY() - 115.0,
			           ZDIR + 0.2),
			           GPoint(8.0, 6.0, 0.0),
			           GColor(0.2, 0.9, 0.2, 0.8),
			           1);

   blueText = new ITextBox(GPoint(location.getX() + 50.0,
   	 		          location.getY() - 115.0,
				  ZDIR + 0.2),
			          GPoint(8.0, 6.0, 0.0),
			          GColor(0.2, 0.2, 0.9, 0.8),
			          1);

   colorCombo = new IComboBox(GPoint(location.getX() + 30.0, location.getY() - 51.0, ZDIR + 0.2),
   			      GPoint(size.getX() / 3.0, 6.0, 0.0),
			      GColor(0.7, 0.5, 0.3, 0.6),
			      5);

   applyButton = new IButton(GPoint(location.getX() + size.getX() - 35.0,
   				    location.getY() - size.getY() + 9.0,
				    ZDIR + 0.2),
			     GPoint(32.0, 6.0, 0.0),
			     GColor(0.7, 0.5, 0.3, 0.6),
			     "Set Selection");

   colorSelect = new IColorSelect(GPoint(location.getX() + 5.0, location.getY() - 65.0, ZDIR));

   //Populate the combo box with the approprate items
   colorCombo->add("Background");
   colorCombo->add("File ListBox");
   colorCombo->add("Folder ListBox");
   colorCombo->add("Hidden Folders");
   colorCombo->add("Linked Folders");
   colorCombo->add("Normal Folders");
   colorCombo->add("Parent Folder");
   colorCombo->add("Status Bar");
   colorCombo->add("Status Bar Text");
   colorCombo->add("System Folders");

   //To initially start-up set hidden folders to the main text title
   colorCombo->SetText("Hidden Folders");
   setRGB(colorPacket.hiddenfolders);
   colorSelect->setcolor(colorPacket.hiddenfolders);
   savedColor = colorPacket.hiddenfolders;
}

/* Handles the mouse clicks
 *
 *
 * PARAMS
 *  pt - the point clicked on the screen
 *  ClickType - where the click was up, down, or hold
 *
 */
void PrefsLFtab::clicked(GPoint pt, int ClickType)
{
	if (lockio)
		return;

	char* value;
	value = (char*) malloc(sizeof (char) * 19 + 1);
	bool locked = false;

	applyButton->clicked(pt, ClickType);
	if (applyButton->Clicked)
	{
		setcolor();
	}

	depthSlider->clicked(pt, ClickType);
	if (depthSlider->Focus)
	{
		sprintf (value, "%d", depthSlider->GetValue());
		depthText->SetText(value);
	}

	breadthSlider->clicked(pt, ClickType);
	if (breadthSlider->Focus)
	{
		sprintf (value, "%d", breadthSlider->GetValue());
		breadthText->SetText(value);
	}

	/* 
	 * To keep track if the textbox has lost focus so that we know to
	 * upadate the slider we must do the following:
	 * 1 - see if we had focus during the previous mouse click
	 * 2 - send the new click and see if it is in the textbox or not
	 * 3 - of the previous click had focus and the current one did not
	 *     we update the slider
	 */

	if (depthText->FocusLock)
		locked = true;

	depthText->clicked(pt, ClickType);

	if ((!depthText->FocusLock) && locked)
	{
		int NewValue = atoi(depthText->string);
		if ((NewValue < depthSlider->GetMin()) || (depthText->IsEmpty()))
		{
			NewValue = depthSlider->GetMin();
			sprintf (value, "%d", NewValue);
    			depthText->SetText(value);
		}
		else if (NewValue > depthSlider->GetMax())
			{
				NewValue = depthSlider->GetMax();
				sprintf (value, "%d", NewValue);
    				depthText->SetText(value);
			}
			else
			{
				//This is done to prevent the possibilty of
				//preceeding zeros
				sprintf (value, "%d", NewValue);
				depthText->SetText(value);
			}
		depthSlider->SetValue(NewValue);
	}

	if (breadthText->FocusLock)
		locked = true;

	breadthText->clicked(pt, ClickType);

	if ((!breadthText->FocusLock) && locked)
	{
		int NewValue = atoi(breadthText->string);
		if ((NewValue < breadthSlider->GetMin()) || (breadthText->IsEmpty()))
		{
			NewValue = breadthSlider->GetMin();
			sprintf (value, "%d", NewValue);
    			breadthText->SetText(value);
		}
		else if (NewValue > breadthSlider->GetMax())
			{
				NewValue = breadthSlider->GetMax();
				sprintf (value, "%d", NewValue);
    				breadthText->SetText(value);
			}
			else
			{
				//This is done to prevent the possibilty of
				//preceeding zeros
				sprintf (value, "%d", NewValue);
				breadthText->SetText(value);
			}
		breadthSlider->SetValue(NewValue);
	}

	//Perform the same testing for the R, G, and B boxes to see if they lost focus
	//and if they did change the color selector
	if ((redText->FocusLock) || (greenText->FocusLock) || (blueText->FocusLock))
		locked = true;

	redText->clicked(pt, ClickType);
	blueText->clicked(pt, ClickType);
	greenText->clicked(pt, ClickType);

	if ((!redText->FocusLock) && (!greenText->FocusLock) && (!blueText->FocusLock) && locked)
	{
		//The value of the R, G, or B has changed so now we will
		//update all of the fields
		int red = atoi(redText->string);
		int green = atoi(greenText->string);
		int blue = atoi(blueText->string);

		//Error check for over 255
		if (red > 255)
			red = 255;
		if (green > 255)
			green = 255;
		if (blue > 255)
			blue = 255;

		//Convert to 0 to 1 scale because that is what the color swatch takes
		GLfloat NewRedValue = red / 255.0;
		GLfloat NewGreenValue = green / 255.0;
		GLfloat NewBlueValue = blue / 255.0;

		//Set the color swatch to the new values
		colorSelect->setcolor(GColor(NewRedValue, NewGreenValue, NewBlueValue, 1.0));

		char* redvalue;
		char* bluevalue;
		char* greenvalue;

		redvalue = (char*) malloc(sizeof (char) * 19 + 1);
		bluevalue = (char*) malloc(sizeof (char) * 19 + 1);
		greenvalue = (char*) malloc(sizeof (char) * 19 + 1);

		//Re-write the values so that we can perform boundary checking
		//and also prevent leading zeros
		sprintf (redvalue, "%d", red);
		sprintf (greenvalue, "%d", green);
		sprintf (bluevalue, "%d", blue);

		redText->SetText(redvalue);
		greenText->SetText(greenvalue);
		blueText->SetText(bluevalue);

		free (redvalue);
		free (greenvalue);
		free (bluevalue);
	}

	/*
	 * Note: the combobox pulls down on top of the color swatch
	 *       and we do not want clicks to propagate through, so we
	 *       check if the combo box is clicked.  If it is we do not
	 *       allow the colorsector to recieve and clicks
	 */
	if (colorCombo->clicked(pt, ClickType))
	{
		if (colorCombo->TitleChange)
			changecolorview();
	}
	else if (colorSelect->clicked(pt, ClickType))
	{
		char* redvalue;
		char* bluevalue;
		char* greenvalue;

		redvalue = (char*) malloc(sizeof (char) * 19 + 1);
		bluevalue = (char*) malloc(sizeof (char) * 19 + 1);
		greenvalue = (char*) malloc(sizeof (char) * 19 + 1);

		//Pull the new values out of the color swatch
		int NewRedValue = (int) (255.0 * colorSelect->getcolor().getRed());
		int NewGreenValue = (int) (255.0 * colorSelect->getcolor().getGreen());
		int NewBlueValue = (int) (255.0 * colorSelect->getcolor().getBlue());
		//Run bounds checking and do not allow the max to be sure 
		//that the indicators do not fall off the the rectangles
		if (NewRedValue > 255)
			NewRedValue = 254;
		if (NewGreenValue > 255)
			NewGreenValue = 254;
		if (NewBlueValue > 255)
			NewBlueValue = 254;
		//Print the new values to the textbox
		sprintf (redvalue, "%d", NewRedValue);
		sprintf (greenvalue, "%d", NewGreenValue);
		sprintf (bluevalue, "%d", NewBlueValue);
		redText->SetText(redvalue);
		greenText->SetText(greenvalue);
		blueText->SetText(bluevalue);

		free (redvalue);
		free (greenvalue);
		free (bluevalue);
	}


	free (value);

	redraw = true;
}

/* Perform object dragging
 *
 * PARAMS
 *   pt - the point where the mouse is
 *
 */
void PrefsLFtab::dragged(GPoint pt)
{

	if (lockio)
		return;

	char* value;
	value = (char*) malloc(sizeof (char) * 19 + 1);

	depthSlider->dragged(pt);
	if (depthSlider->Focus)
	{	sprintf (value, "%d", depthSlider->GetValue());
    		depthText->SetText(value);
	}

	breadthSlider->dragged(pt);
	if (breadthSlider->Focus)
	{	sprintf (value, "%d", breadthSlider->GetValue());
    		breadthText->SetText(value);
	}

	colorSelect->dragged(pt);

	free (value);

}

/* Monitor mouse movement
 *
 * PARAMS
 *   pt - the point where the mouse is
 *
 */
void PrefsLFtab::moved(GPoint pt)
{
	if (lockio)
		return;

	colorCombo->mouseMove(pt);
}

/* Sends keypresses to appropriate controller
 *
 * PARAMS
 *   key - the code for the key hit
 *   x   - the x coordinate of the mouse when the key was hit
 *   y   - the y coordinate of the mouse when the key was hit
 *
 */
void PrefsLFtab::keypress(unsigned char key, int x, int y)
{
	if (lockio)
		return;

	depthText->SendKeys(key);
	breadthText->SendKeys(key);
	redText->SendKeys(key);
	greenText->SendKeys(key);
	blueText->SendKeys(key);

}

/* Sends special keypresses to appropriate controller
 *
 * PARAMS
 *   key - the code for the key hit
 *   x   - the x coordinate of the mouse when the key was hit
 *   y   - the y coordinate of the mouse when the key was hit
 *
 */
void PrefsLFtab::specialkeypress(int key, int x, int y)
{
	if (lockio)
		return;

	colorCombo->SendKeys(key);
}

/* Puts the new depth in to the textbox and sets the slider
 * to that value
 *
 * PARAMS
 *   depth - the depth of the current system
 *
 */
void PrefsLFtab::setdepth(char* depth)
{
	int value = atoi(depth);
	depthText->SetText(depth);
	depthSlider->SetValue(value);
}

/* Puts the new breadth in to the textbox and sets the slider
 * to that value
 *
 * PARAMS
 *   depth - the breadth of the current system
 *
 */
void PrefsLFtab::setbreadth(char* breadth)
{
	int value = atoi(breadth);
	breadthText->SetText(breadth);
	breadthSlider->SetValue(value);
}

/* Retrieves the depth from the textbox and sends it out
 * The reason that we obtain the value form the textbox
 * is because it is more accurate then the slider
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   the current depth
 */
int PrefsLFtab::getdepth ()
{
	char* value;
	value = (char*) malloc(sizeof (char) * 19 + 1);

	int NewValue = atoi(depthText->string);
	if ((NewValue < depthSlider->GetMin()) || (depthText->IsEmpty()))
	{
		NewValue = depthSlider->GetMin();
		sprintf (value, "%d", NewValue);
    		depthText->SetText(value);
	}
	else if (NewValue > depthSlider->GetMax())
		{
			NewValue = depthSlider->GetMax();
			sprintf (value, "%d", NewValue);
    			depthText->SetText(value);
		}
		else
		{
			//This is done to prevent the possibilty of
			//preceeding zeros
			sprintf (value, "%d", NewValue);
			depthText->SetText(value);
		}
	depthSlider->SetValue(NewValue);

	free (value);

	return atoi(depthText->string);
}

/* Retrieves the breadth from the textbox and sends it out
 * The reason that we obtain the value form the textbox
 * is because it is more accurate then the slider
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   the current breadth
 */
int PrefsLFtab::getbreadth()
{
	char* value;
	value = (char*) malloc(sizeof (char) * 19 + 1);

	int NewValue = atoi(breadthText->string);
	if ((NewValue < breadthSlider->GetMin()) || (breadthText->IsEmpty()))
	{
		NewValue = breadthSlider->GetMin();
		sprintf (value, "%d", NewValue);
		breadthText->SetText(value);
	}
	else if (NewValue > breadthSlider->GetMax())
		{
			NewValue = breadthSlider->GetMax();
			sprintf (value, "%d", NewValue);
			breadthText->SetText(value);
		}
		else
		{
			//This is done to prevent the possibilty of
			//preceeding zeros
			sprintf (value, "%d", NewValue);
			breadthText->SetText(value);
		}
	breadthSlider->SetValue(NewValue);

	free (value);
	
	return atoi(breadthText->string);
}

/* Sets the current color packet
 *
 * PARAMS
 *   colorPacket - the new color packet
 *
 * RETURNS
 *   none
 */
void PrefsLFtab::setcolorpacket(s_colorpacket colorPacket)
{
   this->colorPacket = colorPacket;

   //To initially start-up
   colorCombo->SetText("Hidden Folders");
   setRGB(this->colorPacket.hiddenfolders);
   colorSelect->setcolor(this->colorPacket.hiddenfolders);
   savedColor = this->colorPacket.hiddenfolders;
}

/* Returns the current color packet
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   the current color packet
 */
s_colorpacket PrefsLFtab::getcolorpacket()
{
	return colorPacket;
}

/* Sets the current color to the correct variable depending
 * on the active name of the combo box
 *
 * PARAMS
 *   none
 *
 * RETURNS
 *   none
 */
void PrefsLFtab::changecolorview()
{
	char* value;
	value = (char*)malloc(sizeof(char) * strlen(colorCombo->GetText()) + 1);
	strcpy(value, colorCombo->GetText());

	if (strcmp("Background", value) == 0)
	{
		savedColor = colorPacket.background;
	}else if (strcmp("File ListBox", value) == 0)
	{
		savedColor = colorPacket.filelistbox;
	}else if  (strcmp("Folder ListBox", value) == 0)
	{
		savedColor = colorPacket.folderlistbox;
	}else if (strcmp("Hidden Folders", value) == 0)
	{
		savedColor = colorPacket.hiddenfolders;
	}else if (strcmp("Linked Folders", value) == 0)
	{
		savedColor = colorPacket.linkedfolders;
	}else if (strcmp("Normal Folders", value) == 0)
	{
		savedColor = colorPacket.normalfolders;
	}else if (strcmp("Parent Folder", value) == 0)
	{
		savedColor = colorPacket.parentfolder;
	}else if (strcmp("Status Bar", value) == 0)
	{
		savedColor = colorPacket.statusbar;
	}else if (strcmp("Status Bar Text", value) == 0)
	{
		savedColor = colorPacket.statusbartext;
	}else if (strcmp("System Folders", value) == 0)
	{
		savedColor = colorPacket.systemfolders;
	}

	free (value);
}

/* Fills in the R, G, and B text fields based on the color swatch
 *
 * PARAMS
 *   newColor - the color from the color swatch
 *
 * RETURNS
 *   none
 */
void PrefsLFtab::setRGB(GColor newColor)
{
	char* redvalue;
	char* bluevalue;
	char* greenvalue;
	redvalue = (char*) malloc(sizeof (char) * 19 + 1);
	bluevalue = (char*) malloc(sizeof (char) * 19 + 1);
	greenvalue = (char*) malloc(sizeof (char) * 19 + 1);

	//Get the values form the color swatch and convert to the
	//255 scale
	int NewRedValue = (int) (255.0 * newColor.getRed());
	int NewGreenValue = (int) (255.0 * newColor.getGreen());
	int NewBlueValue = (int) (255.0 * newColor.getBlue());

	sprintf (redvalue, "%d", NewRedValue);
	sprintf (greenvalue, "%d", NewGreenValue);
	sprintf (bluevalue, "%d", NewBlueValue);

	redText->SetText(redvalue);
	greenText->SetText(greenvalue);
	blueText->SetText(bluevalue);

	free (redvalue);
	free (greenvalue);
	free (bluevalue);
}

/* Fills in the R, G, and B text fields based on the color swatch
 *
 * PARAMS
 *   newColor - the color from the color swatch
 *
 * RETURNS
 *   none
 */
void PrefsLFtab::setcolor()
{
	char* value;
	value = (char*)malloc(sizeof(char) * strlen(colorCombo->GetText()) + 1);
	strcpy(value, colorCombo->GetText());

	if (strcmp("Background", value) == 0)
	{
		colorPacket.background = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();
	}else if (strcmp("File ListBox", value) == 0)
	{
		colorPacket.filelistbox = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}else if  (strcmp("Folder ListBox", value) == 0)
	{
		colorPacket.folderlistbox = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}else if (strcmp("Hidden Folders", value) == 0)
	{
		colorPacket.hiddenfolders = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}else if (strcmp("Linked Folders", value) == 0)
	{
		colorPacket.linkedfolders = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}else if (strcmp("Normal Folders", value) == 0)
	{
		colorPacket.normalfolders = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}else if (strcmp("Parent Folder", value) == 0)
	{
		colorPacket.parentfolder = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}else if (strcmp("Status Bar", value) == 0)
	{
		colorPacket.statusbar = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}else if (strcmp("Status Bar Text", value) == 0)
	{
		colorPacket.statusbartext = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}else if (strcmp("System Folders", value) == 0)
	{
		colorPacket.systemfolders = colorSelect->getcolor();
		savedColor = colorSelect->getcolor();

	}

	free (value);

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
bool PrefsLFtab::AnimatePane()
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
    glVertex3f(location.getX(), location.getY(), ZDIR + 0.2);
    glVertex3f(location.getX(), location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 28.0, location.getY() - 6.0, ZDIR + 0.2);
    glVertex3f(location.getX() + 28.0, location.getY(), ZDIR + 0.2);
    glEnd();

    return StillAnimate;
}
