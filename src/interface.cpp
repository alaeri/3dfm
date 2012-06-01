/*
 * 3dfm - 3D File Manager
 * $Id: interface.cpp,v 1.76 2003/05/04 22:42:10 adam Exp $
 */

#include <FFolder.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <GL/glut.h>
#include <GPoint.h>
#include <GColor.h>
#include <GSelectList.h>
#include <ISlider.h>
#include <IList.h>
#include <Prefs.h>
#include <PrefsControl.h>
#include <ISearchBox.h>
#include <IPrefsIcon.h>
#include <IGotoIcon.h>

/*
 * Snapshot demo with object selection
 *
 * This test program takes a path and depth as parameters and draws a single
 * static snapshot of what our program would display.  It also allows the
 * user to select objects.
 */

#define CHECK_DELTA	  50
#define ACTION_NONE       0
#define ACTION_EYEMOVE    1
#define ACTION_LOOKATMOVE 2
#define MOVE_Z            0
#define MOVE_XY           1

/* navigation types (record MRU or not) */
#define MRU_IGNORE 0  /* ignore this navigation for MRU purposes */
#define MRU_RECORD 1  /* record this navigation for MRU purposes */

#define PI 3.141592654

char status_tooltip[200];
long long int br = 100;
FFolder *ourPath;
GPoint eyeOrig(0.0, 0.0, 50.0); /* where the eye moves to at reset */
//GPoint eye(0.0, 0.0, 50.0);     /* where the eye (camera) is located */
GPoint eye(-60.2, -17.5, -187.2);     /* where the eye (camera) is located */
GPoint lookatOrig(0.0, 0.0, 0.0);  /* where we are looking at reset */
GPoint lookat(73.2, -95.2, -42.5);  /* where we are looking at reset */
//GPoint lookat(0.0, 0.0, 0.0);  /* where the camera is looking */
GPoint up(0.0, 1.0, 0.0);      /* which way is "up" */
int dirty;      /* does the display need to be redrawn */
int resetFolder;      /* are we moving the camera? */
int resetEye;
int resetLookat;
int resetRotate = 0;
GLfloat fstart; /* start and end points for fog, when linear */
GLfloat fend;
int moveAction;  /* what we do when the mouse moves */
int mouseX, mouseY;
int mousePointX, mousePointY;
int moveType;
int deletePane = 0;
int deleteFilePane = 0;
int deletePrefsDlg = 0;
int deleteSearch = 0;
int storedLabels = 0;
GLdouble modelMatrix[16];
GLdouble projMatrix[16];
GLint viewport[4];
IList *testPane;
ISlider *testSlider;
IList *filePane;
PrefsControl *prefsDlg;
ISearchBox *searchBox;
IPrefsIcon *prefsIcon;
IGotoIcon *gotoIcon;

/*
 * reshape
 *
 * This function reshapes the display when the window is resized or moved.
 *
 * PARAM
 *   width - the width of the new window size
 *   height - the height of the new window size
 */
void reshape(int width, int height) {
    /* set the viewport size */
    glViewport(0, 0, (GLint)width, (GLint)height);
}

/*
 * navigate
 *
 * This function navigates to a new location (changes the active
 * folder).  It does not do any checking to see if the new path
 * actually exists or is valid, that falls under the perview of
 * FFolder.
 *
 * PARAM
 *   newPath       - the new path to which to navigate 
 *   activeStartPt - where to place the newly active folder 
 *   recordMRU     - if nonzero, records this navigation for most-recently-
 *                   used purposes
 */
void navigate(char *newPath, GPoint activeStartPt, 
	      int recordMRU = MRU_RECORD) {

    /* this could take a while... */
    glutSetCursor(GLUT_CURSOR_WAIT);

    /* if we are recording this navigation for MRU purposes, do so */
    if (recordMRU) {
	Prefs::setMRU(newPath);
    }

    /* delete the path and recreate it */
    delete ourPath;
    ourPath = new FFolder(newPath, Prefs::depth, NULL, 1.0,
			  GColor(0.734375, 0.539062, 0.25),
			  Prefs::folderColor);
    ourPath->setShowDotDot((Prefs::breadth < ourPath->countFolders(0)) 
			   ? 3 : 2);
    ourPath->setLocation(activeStartPt);

    /* animate the view to it's original position */
    resetLookat = resetEye = resetFolder = 1;

    /* change the cursor back to the default (inherited from parent app) */
    glutSetCursor(GLUT_CURSOR_INHERIT);

}

/*
 * reload
 *
 * This function reloads the current view to allow certain changes to take
 * effect.  It is currently used when preferences are changed but could be
 * used in other circumstances in the future.
 */
void reload(void) {

    /* get the current path */
    char *oldPath = ourPath->getName();

    /* copy it */
    int path_len = strlen(oldPath);
    char *newPath = (char *) malloc(sizeof(char) * (path_len + 1));
    strncpy(newPath, oldPath, path_len + 1);

    /* grab the location (use the same old one) */
    GPoint newLocation = ourPath->getLocation();

    /* call navigate to do the reload */
    navigate(newPath, newLocation, MRU_IGNORE);

}

/* forward declaration */
void CheckClose(int value);
void checkFilePaneOpen(int value);

void special_keypress(int key, int x, int y) {
    if (prefsDlg) {
	prefsDlg->specialkeypress(key, x, y);
    }
//     if (searchBox) {
// 	searchBox->specialkeypress(key, x, y);
//     }
}

/*
 * keypress
 *
 * This is the callback function for when a key is pressed.  It is called by
 * GLUT.  At the moment we do not use the x,y coordinates of the mouse pointer
 * when the key was pressed, but we must take the parameters because that's
 * what the caller expects this function signature to be.
 *
 * PARAM
 *   key - the ASCII value of the key pressed
 *   x   - the X position of the mouse in the window when the key was pressed
 *   y   - the Y position of the mouse in the window when the key was pressed
 */
void keypress(unsigned char key, int x, int y) {
    if (prefsDlg) {
	prefsDlg->keypress(key, x, y);
	return;
    }

    if (searchBox) {
	searchBox->keypress(key, x, y);
	return;
    }
    
    if (testPane)
    {
    	if (key == 27)
		deletePane = 1;
	return;
    }

    if (filePane)
    {
    	if (key == 27)
	{
		ourPath->setState(FOLDER_CLOSE);
		deleteFilePane = 1;
	}
	return;
    }


    switch (key) {

#ifdef _DEBUG_AVL
	case 's':
	    fprintf(stderr, "list: %d tree: %d\n", AVLTree::list_cnt,
		    AVLTree::tree_cnt);
#endif
	case '|':
	case '%':
	    Prefs::showOnlyChecked = (Prefs::showOnlyChecked) ? 0 : 1;
//	    reload();
	    ourPath->positionChildren();
	    break;

	/* turn labels on/off */
	case 'l':
	case 'L':
	    Prefs::showLabels = (Prefs::showLabels) ? 0 : 1;
	    dirty = 1;
	    break;

        /* change the label type */
	case 'k':
	case 'K':
	    Prefs::strokeFont = (Prefs::strokeFont) ? 0 : 1;
	    dirty = 1;
	    break;

	/* reset the view */
	case 'X':
	case 'x':
	    resetLookat = resetEye = resetFolder = resetRotate = 1;
	    break;
	case 'o':
	    if ((!testPane) && (!prefsDlg) && (!searchBox)) {
		storedLabels = Prefs::showLabels;
		Prefs::showLabels = 0;
		ourPath->setState(FOLDER_OPEN);
		resetLookat = resetEye = resetFolder = 1;
		glutTimerFunc(50, checkFilePaneOpen, 1);
	    }
	    break;
	case 'c':
//	    ourPath->setState(FOLDER_CLOSE);
	    break;
	case 'G':
	case 'g':
	{
	if ((!testPane) && (!prefsDlg) && (!filePane))
	    {
	    searchBox = new ISearchBox(GPoint(36.0, -60.0, 0.0),
				       GColor(0.3, 0.5, 1.0, 0.4));
	    glutTimerFunc(50, CheckClose, 1);
	    }
	}
	    break;
	case 'B':
	case 'b':
	{

	    if (testPane) {
		return;
	    }

	    /* grab the location of the active folder.  This is where
	     * we will be placing the new active folder */
	    GPoint active_pt = ourPath->getLocation();

	    /* grab our own copy of the path name, since we will be
	     * mutilating it, not to mention the path will be going
	     * away */
	    char *oldpath_str = ourPath->getName();
	    char *newpath_str = (char *) malloc(strlen(oldpath_str) + 4);
	    strcpy(newpath_str, oldpath_str);

	    /* create the name of the parent */
	    strcat(newpath_str, "/..");
	    FFSObject::stripName(newpath_str);

	    /* navigate to the new location */
	    navigate(newpath_str, active_pt);

	    /* delete our name */
	    free(newpath_str);
	}
	    break;
	case 'p':
	case 'P':
	{
	if ((!testPane) && (!filePane) && (!searchBox))
	    {
	    /* populate the color packet */
	    s_colorpacket colors;
	    colors.normalfolders = Prefs::folderColor;
	    colors.hiddenfolders = Prefs::hiddenColor;
	    colors.parentfolder = Prefs::parentColor;
	    colors.linkedfolders = Prefs::linkColor;
	    colors.systemfolders = Prefs::systemColor;
	    colors.background = Prefs::backgroundColor;
	    colors.filelistbox = Prefs::filelistboxColor;
	    colors.folderlistbox = Prefs::folderlistboxColor;
	    colors.statusbar = Prefs::statusbarColor;
	    colors.statusbartext = Prefs::statusbartextColor;

	    s_checkpacket checks;
	    checks.linkedfolders = Prefs::showLinks;
	    checks.hiddenfolders = Prefs::showHidden;
	    checks.systemfolders = Prefs::showSystem;
	    checks.showlabels = Prefs::showLabels;
	    checks.enablechecks = Prefs::showOnlyChecked;
	    checks.strokefonts = Prefs::strokeFont;

	    prefsDlg = new PrefsControl(GPoint(-60.0, 80.0, 0.0),
					GPoint(125.0, 150.0, 0.0),
					GColor(1.0, 0.5, 0.4, 0.7),
					GColor(0.0, 0.0, 1.0, 0.7),
					"Preferences");

	    prefsDlg->setdepth(Prefs::depth);
	    prefsDlg->setbreadth(Prefs::breadth);
	    prefsDlg->setcolorpacket(colors);
	    prefsDlg->setcheckpacket(checks);
	    glutTimerFunc(50, CheckClose, 1);
	    }
	}
	    break;
	case '[':
	    Prefs::child_rotate -= 0.1;
	    if (Prefs::child_rotate < -PI) {
		Prefs::child_rotate = PI - (-Prefs::child_rotate - PI);
	    }
	    ourPath->positionChildren();
	    dirty = 1;
	    break;
	case ']':
	    Prefs::child_rotate += 0.1;
	    if (Prefs::child_rotate > PI) {
		Prefs::child_rotate = - PI - (Prefs::child_rotate - PI);
	    }
	    ourPath->positionChildren();
	    dirty = 1;
	    break;
	case 'f':
	case 'F': 
	{
	    /** popup folder list **/
	    GColor c = Prefs::folderlistboxColor;
	    c.setAlpha(0.85);
	    bool checks = ((Prefs::breadth < ourPath->countFolders(0))
			   || (Prefs::showOnlyChecked));
	    testPane = new IList(GPoint(50.0,100.0,0.0),
				 GPoint(50.0, 190.0, 0.0),  
				 c, checks);
	    FFolder *f;
	    ourPath->rewindFolders();
//		    int folder_cnt = ourPath->countFolders();
//		    for (int i = 0; i < folder_cnt; i++) {
//			FFolder *f = ourPath->getChildFolder(i);
	    while ((f = ourPath->getNextFolder(0)) != NULL) {
		char *name = f->getShortName();
		char *name_l = f->getName();
		int check = Prefs::isChecked(name_l);
		if (strcmp(name, "") != 0) {
		    testPane->add(f->getShortName(), f->getName(),
				  check);
		    resetLookat = resetEye = resetFolder = 1;
		}
	    }
	    
	    /* schedule checks to see when the pane closes */
	    glutTimerFunc(50, CheckClose, 1);
	    break;
	}
	default:
	    break;
    }

}

static void ourPrintString(
   void *font,
   char *str
)
{
   int i,l=strlen(str);

   for(i=0;i<l;i++)
      glutBitmapCharacter(font,*str++);
}

void checkFilePaneOpen(int value) {
    if (ourPath->getState() == FOLDER_OPEN) {
	GColor c = Prefs::filelistboxColor;
	c.setAlpha(0.85);
	filePane = new IList(GPoint(-70.0,57.0,0.0),
			     GPoint(130.0, 100.0, 0.0),
			     c, false);
	FFile *f;
	ourPath->rewindFiles();
	while ((f = ourPath->getNextFile()) != NULL) {
	    char *name = f->getShortName();
	    if (strcmp(name, "") != 0) {
		filePane->add(f->getShortName(), f->getName(), 0);
	    }
	}

	filePane->SetTitle(ourPath->getName());

	/* schedule checks to see when the pane closes */
	glutTimerFunc(50, CheckClose, 1);
    } else {
	glutTimerFunc(50, checkFilePaneOpen, 1);
    }
}

/*
 * render
 *
 * This function performs the OpenGL drawing into the selected buffer.
 */
void render(GSelectList *selector) {

    /* select our matrix */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    /* are we doing selection? */
    if (selector) {
	glInitNames();
	glPushName(1000);
    }

    /* our background color and fog color */
    GColor bgColor = Prefs::backgroundColor;
    glClearColor(bgColor.getRed(), bgColor.getGreen(), bgColor.getBlue(), 1.0);
    GLfloat color[4] = {bgColor.getRed(), bgColor.getGreen(), 
			bgColor.getBlue(), 1.0};
    glFogfv(GL_FOG_COLOR, color);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(eye.getX(), eye.getY(), eye.getZ(),
	      lookat.getX(), lookat.getY(), lookat.getZ(),
	      up.getX(), up.getY(), up.getZ());

    glDisable(GL_BLEND);
    if (ourPath->execGL(selector)) {
	dirty = 1;
    }
    ourPath->execChildGL(selector);
    glEnable(GL_BLEND);

    glPopMatrix();
    glPopName();

    /* draw text */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-100.0, 100.0, -100.0, 100.0, -9.0, 3.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);


    /* draw a box */
    glDisable(GL_FOG);
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_DEPTH_TEST);

    GColor status_color = Prefs::statusbarColor;
    status_color.setAlpha(0.6);
    status_color.execFullGL();
    glBegin(GL_QUADS);
    glVertex3f(-100.0, -100.0, 0.0);
    glVertex3f(-100.0,  -90.0, 0.0);
    glVertex3f(+100.0,  -90.0, 0.0);
    glVertex3f(+100.0, -100.0, 0.0);
    glEnd();

    /* draw tooltip text */

    glDisable(GL_BLEND);
    Prefs::statusbartextColor.execFullGL();
//    GColor(1.0, 1.0, 0.0).execFullGL();
    glRasterPos3f(-95.0, -96.2, 0.5);
    ourPrintString(GLUT_BITMAP_HELVETICA_18, status_tooltip);
    glEnable(GL_BLEND);

    if ((deletePane) && (testPane)) {
	int reread = (int) testPane->ReRead;
 	delete testPane;
	deletePane = 0;
	testPane = NULL;
	if (reread) {
// 	    char *name_ptr = ourPath->getName();
// 	    char *pathname = (char *) malloc(strlen(name_ptr) + 1);
// 	    strcpy(pathname, name_ptr);
// 	    GPoint newLoc = ourPath->getLocation();
// 	    delete ourPath;
// 	    ourPath = new FFolder(pathname, Prefs::depth, NULL, 1.0,
// 				  GColor(0.734375, 0.539062, 0.25),
// 				  Prefs::folderColor);
// 	    ourPath->setLocation(newLoc);
// 	    //resetLookat = resetEye = resetFolder = 1;
// 	    free(pathname);
	    reload();
	} else 	if (Prefs::showOnlyChecked) {
	    ourPath->positionChildren();
	}
    } else if (testPane) {
	if (testPane->execGL())
		dirty = 1;
    }

    /* draw the file pane */
    if ((deleteFilePane) && (filePane)) {
	Prefs::showLabels = storedLabels;
	delete filePane;
	deleteFilePane = 0;
	filePane = NULL;
    } else if (filePane) {
	if (filePane->execGL()) {
	    dirty = 1;
	}
    }

    /* draw the preferences pane */
    if ((deletePrefsDlg) && (prefsDlg)) {
	delete prefsDlg;
	deletePrefsDlg = 0;
	prefsDlg = NULL;
    } else if (prefsDlg) {
	if (prefsDlg->execGL()) {
	    dirty = 1;
	}
    }

    /* draw the search box */
    if ((deleteSearch) && (searchBox)) {
	if (searchBox->haveData) {
	    /* do search */
	    char *text = searchBox->gettext();
	    char *text_copy = (char *) malloc(sizeof(char) 
					      * (1 + strlen(text)));
	    strcpy(text_copy, text);

	    if (fopen(text_copy, "r") == NULL) {
		/* invalid */ 
		searchBox->notfound();
		deleteSearch = 0;
		free(text_copy);
		glutTimerFunc(50, CheckClose, 1);

		goto DONE_SEARCH;
	    }
	    delete searchBox;
	    deleteSearch = 0;
	    searchBox = NULL;
	    dirty = 1;
	    GPoint loc = ourPath->getLocation();
	    navigate(text_copy, loc, MRU_IGNORE);
	    free(text_copy);
	} else {
	    delete searchBox;
	    deleteSearch = 0;
	    searchBox = NULL;
	    dirty = 1;
	}
    } else if (searchBox) {
    DONE_SEARCH:
	if (searchBox->execGL()) {
	    dirty = 1;
	}
    }

    if (prefsIcon) {
      prefsIcon->execGL();
    }

    if (gotoIcon) {
      gotoIcon->execGL();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_FOG);

    /* done with screen coords - reset to 3d coords */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glutSwapBuffers();
}

void CheckClose(int value) {

    /* if the list tells us to navigate (meaning someone clicked on a
     * folder in the list, then we need to ablidge */
    if ((testPane) && (testPane->DoNavigate)) {

	/* get the location */
	GPoint active_pt = ourPath->getLocation();

	/* grab a copy of the path to which to navigate */
	char *navpath_str = (char *) malloc(strlen(testPane->NavigateLocation)
					    * sizeof(char) + 1);
	strcpy(navpath_str, testPane->NavigateLocation);

	/* do the navigation */
	navigate(navpath_str, active_pt);

	/* clean up and set the flag to delete the folder list */
	free(navpath_str);
	deletePane = 1;
    }

    /* if the folder list tells us that we need to close it, we should
     * do that */
    if ((testPane) && (testPane->CloseList)) {
    	deletePane = 1;
	dirty = 1;
	return;  /* return here, so we don't schedule this again */
    }

    if ((searchBox) && ((searchBox->closeBox) || (searchBox->haveData))) {
	deleteSearch = 1;
	dirty = 1;
	return;
    }

    if ((prefsDlg) && ((prefsDlg->exitStatus) != 0)) {
	if (prefsDlg->exitStatus == 1) {

	    /* grab the colors from the dialog */
	    s_colorpacket colors = prefsDlg->getcolorpacket();
	    Prefs::folderColor = colors.normalfolders;
	    Prefs::hiddenColor = colors.hiddenfolders;
	    Prefs::parentColor = colors.parentfolder;
	    Prefs::linkColor = colors.linkedfolders;
	    Prefs::systemColor = colors.systemfolders;
	    Prefs::backgroundColor = colors.background;
	    Prefs::filelistboxColor = colors.filelistbox;
	    Prefs::folderlistboxColor = colors.folderlistbox;
	    Prefs::statusbarColor = colors.statusbar;
	    Prefs::statusbartextColor = colors.statusbartext;

	    /* grab the breadth and depth */
	    Prefs::depth = prefsDlg->getdepth();
	    Prefs::breadth = prefsDlg->getbreadth();

	    s_checkpacket checks = prefsDlg->getcheckpacket();
	    Prefs::showLabels = checks.showlabels;
	    Prefs::showHidden = checks.hiddenfolders;
	    Prefs::showLinks = checks.linkedfolders;
	    Prefs::showSystem = checks.systemfolders;
	    Prefs::showOnlyChecked = checks.enablechecks;
	    Prefs::strokeFont = checks.strokefonts;

//	    navigate(ourPath->getName(), ourPath->getLocation(), MRU_IGNORE);
	    reload();

	}
    	deletePrefsDlg = 1;
	dirty = 1;
	return;  /* return here, so we don't schedule this again */
    }
    /* if the file list tells us that we need to close it, we should
     * do that */
    if ((filePane) && (filePane->CloseList)) {
    	deleteFilePane = 1;
	dirty = 1;
	ourPath->setState(FOLDER_CLOSE);
	return;  /* return here, so we don't schedule this again */
    }

    /* if we are displaying the pane, pass passive mouse motion in */
    if (testPane) {
	GLdouble fx ,fy ,fz ;
	gluUnProject(mousePointX, mousePointY, 0.0, modelMatrix,
			       projMatrix,viewport,&fx,&fy,&fz);

	testPane->mouseMove(GPoint((float)fx, -(float)fy, 0.1));
    }

    /* if we are displaying the pane, pass passive mouse motion in */
    if (filePane) {
	GLdouble fx ,fy ,fz ;
        gluUnProject(mousePointX, mousePointY, 0.0, modelMatrix,
			       projMatrix,viewport,&fx,&fy,&fz);

	filePane->mouseMove(GPoint((float)fx, -(float)fy, 0.1));
    }

    /* if not navigating or closing, schedule another check in 1ms */
    glutTimerFunc(50, CheckClose, 1);
}

void generate_status(int moused, char *tooltip) {
    if (!moused) {
	sprintf(status_tooltip, "3dfm Beta - %s",
		ourPath->getName());
    }


    if (moused) {
	sprintf(status_tooltip, "%s", tooltip);
    }
}

void findhit(int x, int y, int traverse) {
    int pointed = 0;  /* set to one if we pointed to something */
    GLuint selectBuf [512];
    GLint hits, i, j, k;
    int namez, z1, z2;
    GLint viewport [4];
//    char linez [255], text [255], temp [255];
    GSelectList *selector = new GSelectList();

    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(512, selectBuf);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    glRenderMode(GL_SELECT);
    gluPickMatrix(x, viewport[3]-y, 1, 1, viewport);
    gluPerspective(60.0f,1.0f,5.0f,500.0f);
    render(selector);
    hits = glRenderMode(GL_RENDER);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
#ifdef _DEBUG_SELECT
    fprintf(stderr, "Total No. of hits (object under mouse): %d\n", hits);
#endif

    /* stolen code below! */
    j = 0;
    for (i = 0; i < hits; i++) {
 	namez = selectBuf [j]; j++;
 	z1 = selectBuf [j]; j++;
 	z2 = selectBuf [j]; j++;
 	for ( k = 0; k < namez; k++) {
#ifdef _DEBUG_SELECT
	    fprintf(stderr, "Hit object number %d.\n", selectBuf[j];
#endif
	    FFolder *folder = selector->getHit(selectBuf[j]);
	    if (folder) {
		char *name_ptr = folder->getName();

		if (!traverse) {
#ifdef _DEBUG_SELECT
		    fprintf(stderr, "Moused object %s.\n", name_ptr);
#endif
		    pointed = 1;
		    if (selectBuf[j] == 1) {
			char *t = (char *) malloc(sizeof(char) 
						  * strlen(name_ptr) + 30);
			strcpy(t, name_ptr);
			strcat(t, " - Click here to open folder");
			generate_status(1, t);
			free(t);
		    } else if ((selectBuf[j] == 2) || (selectBuf[j] == 3) ||
			((selectBuf[j] == 4) 
			 && ourPath->getShowDotDot() == 3)) {
			generate_status(1, "Click here to open folder list");
		    } else {
			generate_status(1, name_ptr);
		    }
//		    sprintf(status_tooltip, "%s", name_ptr);
		    dirty = 1;

		    /* exit the loopy */
		    k = namez;
		    i = hits;
		    continue;
		}
#ifdef _DEBUG_SELECT
		else {
		    fprintf(stderr, "Hit object %s.\n", name_ptr);
		}
#endif

		/* did we click on the active folder? (id = 1) */
		if ((selectBuf[j] == 1) && (testPane == NULL)) {
		    keypress('o', 0, 0);
		} else if ((selectBuf[j] == 2) || (selectBuf[j] == 3) ||
			   ((selectBuf[j] == 4) 
			    && ourPath->getShowDotDot() == 3)) {
		    keypress('f', 0, 0);
		} else {

		    /** navigate **/
		    char *pathname = (char *) malloc(strlen(name_ptr) + 1);
		    strcpy(pathname, name_ptr);
		    navigate(pathname, folder->getLocation());
		    free(pathname);
		}
		k = namez;
		i = hits;  /* end the loopy */
	    }
 	    j++;
 	}
    }

    if (!pointed) {
//	sprintf(status_tooltip, "3dfm Alpha - Current %s",
//		ourPath->getName());
	generate_status(0, NULL);
	dirty = 1;
    }
    delete selector;
}

void mouseCommand(int button, int state, int x, int y) {
    int specialKey = glutGetModifiers();

    if ((prefsDlg) && (button == GLUT_LEFT_BUTTON)) {
	GLdouble fx ,fy ,fz ;
	gluUnProject(x, y, 0.0, modelMatrix,
			       projMatrix,viewport,&fx,&fy,&fz);

	prefsDlg->clicked(GPoint((float)fx, -(float)fy, 0.0), state);
    }

    if (searchBox) {
	GLdouble fx ,fy ,fz ;
	gluUnProject(x, y, 0.0, modelMatrix,
			       projMatrix,viewport,&fx,&fy,&fz);

	searchBox->clicked(GPoint((float)fx, -(float)fy, 0.0), state);
    }

    if (prefsIcon) {
      GLdouble fx ,fy ,fz ;
      gluUnProject(x, y, 0.0, modelMatrix,
		   projMatrix,viewport,&fx,&fy,&fz);

      prefsIcon->clicked(GPoint((float)fx, -(float)fy, 0.0), state);
      if (prefsIcon->Clicked) {
	keypress('p', 0, 0);
	return;
      }
      if (prefsIcon->Capture)
         return;
    }

    if (gotoIcon) {
      GLdouble fx ,fy ,fz ;
      gluUnProject(x, y, 0.0, modelMatrix,
		   projMatrix,viewport,&fx,&fy,&fz);

      gotoIcon->clicked(GPoint((float)fx, -(float)fy, 0.0), state);
      if (gotoIcon->Clicked) {
	keypress('g', 0, 0);
	return;
      }
      if (gotoIcon->Capture)
      	return;
    }

    /******* AFTER THIS POINT, NO MORE GLUT_UP *********************/

    if (state == GLUT_UP) {
	moveAction = ACTION_NONE;
	return;
    }

    if ((testPane) && (button ==GLUT_LEFT_BUTTON)) {
	GLdouble fx ,fy ,fz ;
	gluUnProject(x, y, 0.0, modelMatrix,
			       projMatrix,viewport,&fx,&fy,&fz);

	testPane->clicked(GPoint((float)fx, -(float)fy, 0.0));
	return;
    }

    if (filePane)
    {
      if  (button == GLUT_LEFT_BUTTON) {
	GLdouble fx ,fy ,fz ;
	gluUnProject(x, y, 0.0, modelMatrix,
			       projMatrix,viewport,&fx,&fy,&fz);

	filePane->clicked(GPoint((float)fx, -(float)fy, 0.0));
      }
      return;
    }	


    /* right mouse button does eye movement */
    if ((button == GLUT_RIGHT_BUTTON)
	&& (state == GLUT_DOWN)) {
	moveAction = ACTION_EYEMOVE;
    }

    /* middle mouse button does lookat target movement */
    if ((button == GLUT_MIDDLE_BUTTON)
	&& (state == GLUT_DOWN)) {
	moveAction = ACTION_LOOKATMOVE;
	resetFolder = 0;
    }

    if (specialKey == GLUT_ACTIVE_CTRL) {
	moveType = MOVE_Z;
    } else {
	moveType = MOVE_XY;
    }

    mouseX = x;
    mouseY = y;

    /* don't process click further if the pane is up */
    if ((testPane) || (filePane) || (prefsDlg) || (searchBox)) {
  	return;
    }

    /* left mouse button does hit testing */
    if ((button == GLUT_LEFT_BUTTON)
	&& (state == GLUT_DOWN)) {
	findhit(x, y, 1);
    }

}

void mouseMove(int x, int y) {
    GLfloat Xoff = (GLfloat) 0.0;
    GLfloat Yoff = (GLfloat) 0.0;
    GLfloat Zoff = (GLfloat) 0.0;
    if (moveType == MOVE_Z) {
	Zoff = (GLfloat) (y - mouseY) / 5.0f;
    } else {
	Xoff = (GLfloat) -((x - mouseX) / 5.0f);
	Yoff = (GLfloat) (y - mouseY) / 5.0f;
    }
    /* the orientation vector (direction we are pointing) */
    GPoint ZVector = eye - lookat;
    ZVector.normalize();
    GPoint YVector = up;
    GPoint XVector = YVector * ZVector;
    GPoint offset = XVector * Xoff + YVector * Yoff + ZVector * Zoff;

    if (moveAction == ACTION_EYEMOVE) {
	eye = eye + offset;
	resetEye = 0;
	dirty = 1;
    }

    if (moveAction == ACTION_LOOKATMOVE) {
	lookat = lookat + offset;
	dirty = 1;
    }

    if (prefsDlg) {
	GLdouble fx ,fy ,fz ;
	//int ret = gluUnProject(mousePointX, mousePointY, 0.0, modelMatrix,
	//		       projMatrix,viewport,&fx,&fy,&fz);
	gluUnProject(x, y, 0.0, modelMatrix,
			       projMatrix,viewport,&fx,&fy,&fz);

	prefsDlg->dragged(GPoint((float)fx, -(float)fy, 0.0));
    }


    mouseX = x;
    mouseY = y;

}

void mousePoint(int x, int y) {
    mousePointX = x;
    mousePointY = y;
    if (prefsDlg) {
	GLdouble fx ,fy ,fz ;
	gluUnProject(mousePointX, mousePointY, 0.0, modelMatrix,
			       projMatrix,viewport,&fx,&fy,&fz);

	prefsDlg->moved(GPoint((float)fx, -(float)fy, 0.1));
    }	

    if (gotoIcon) {
      GLdouble fx ,fy ,fz ;
      gluUnProject(x, y, 0.0, modelMatrix,
		   projMatrix,viewport,&fx,&fy,&fz);
      
      prefsIcon->moved(GPoint((float)fx, -(float)fy, 0.1));
    } 

    if (gotoIcon) {
      GLdouble fx ,fy ,fz ;
      gluUnProject(x, y, 0.0, modelMatrix,
		   projMatrix,viewport,&fx,&fy,&fz);
      
      gotoIcon->moved(GPoint((float)fx, -(float)fy, 0.1));
    }

}

/*
 * draw
 *
 * This function performs the OpenGL drawing everything it is necessary.  This
 * usually happens when the window is first presented, or any time all or
 * part of the window is unhidden.
 */
void draw(void) {
    render(NULL); /* NULL means we are not doing object selection */
}

void flashy_intro(int value) {
    resetEye = resetLookat = resetFolder = 1;
}

void checkRedraw(int value) {
    static int tooltip_delay = 3;
    GPoint vector;

    if (resetEye) {
	vector = eyeOrig - eye;
	if (vector.getLength() < 0.1) {
	    eye = eyeOrig;
	    resetEye = 0;
	} else {
	    eye = eye + vector * 0.1;
	}
	dirty = 1;
    }
    if (resetLookat) {
	vector = lookatOrig - lookat;
	if (vector.getLength() < 0.1) {
	    lookat = lookatOrig;
	    resetLookat = 0;
	} else {
	    lookat = lookat + vector * 0.1;
	}
	dirty = 1;
    }
    if (resetFolder) {
	vector = GPoint(0.0, 0.0, 0.0) - ourPath->getLocation();
	if (vector.getLength() < 0.1) {
	    ourPath->setLocation(GPoint(0.0, 0.0, 0.0));
	    resetFolder = 0;
	} else {
	    ourPath->setLocation(ourPath->getLocation() + vector * 0.1);
	}
	glutPostRedisplay();
    }
    if (resetRotate) {
	if (Prefs::child_rotate > 0.10) {
	    Prefs::child_rotate -= 0.1;
	} else if (Prefs::child_rotate < -0.10) {
	    Prefs::child_rotate += 0.1;
	}

	if ((Prefs::child_rotate < 0.10)
	    && (Prefs::child_rotate > -0.10)) {
	    resetRotate = 0;
	    Prefs::child_rotate = 0.0;
	}
	dirty = 1;
	ourPath->positionChildren();
    }
    if (dirty) {
	dirty = 0;
	glutPostRedisplay();
    }

    if (tooltip_delay == 0) {
	findhit(mousePointX, mousePointY, 0);
 	tooltip_delay = 3;
    } else {
	--tooltip_delay;
    }

    glutTimerFunc(CHECK_DELTA, checkRedraw, 0);
}

void start(int argc, char **argv) {

    /* we want a RGB double-buffered depth aware display
     * of 500x500 pixels located at (100,100) */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(50,50);
    glutCreateWindow("./innolab 3dfm");

    /* our background color */
    GColor bgColor = Prefs::backgroundColor;
    glClearColor(bgColor.getRed(), bgColor.getGreen(), bgColor.getBlue(), 1.0);

    /* we want flat (not gourad) shading, both of our polygon faces filled,
     * and depth testing to remove hidden lines */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
    GLfloat ambLight[4] = {0.4, 0.4, 0.4, 1.0};
//    GLfloat ambLight[4] = {0.2, 0.2, 0.2, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambLight);
    glEnable(GL_LIGHT0);
    glEnable(GL_FOG);

    /* this is our coordinate system */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f,1.0f,5.0f,500.0f);

    /* fog */
    glMatrixMode(GL_MODELVIEW);
    GLfloat color[4] = {bgColor.getRed(), bgColor.getGreen(), 
			bgColor.getBlue(), 1.0};
    glFogfv(GL_FOG_COLOR, color);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 10.0); //5
    glFogf(GL_FOG_END, 250.0); //150

    /* register our callback functions */
    glutReshapeFunc(reshape);
    glutDisplayFunc(draw);
    glutIdleFunc(draw);

    /* register keyboard stuff and set defaults*/
    glutKeyboardFunc(keypress);
    glutSpecialFunc(special_keypress);
    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseCommand);
    glutPassiveMotionFunc(mousePoint);

    /* redraw stuff */
    resetLookat = resetEye = resetFolder = 0;
    dirty = 1;
    testPane = NULL;
    filePane = NULL;
    prefsDlg = NULL;
    glutTimerFunc(CHECK_DELTA, checkRedraw, 0);
    glutTimerFunc(2500, flashy_intro, 0);
//    glutTimerFunc(1, CheckClose, 1);

    /* create icons */
    GColor iconBackColor = GColor(0.1, 0.1, 0.1, 0.4);
    //    GColor iconBackColor = Prefs::statusbarColor;
    //    iconBackColor.setAlpha(0.4);
    prefsIcon = new IPrefsIcon(GPoint(-100.0, 100.0, 0.0),    /* location */
			       GPoint(10.0, 10.0, 0.0),     /* size */
			       iconBackColor);              /* color*/
    gotoIcon = new IGotoIcon(GPoint(-100.0, 90.0, 0.0),      /* location */
			     GPoint(10.0, 10.0, 0.0),       /* size */
			     iconBackColor);                /* color */
    /* go to it */
    glutMainLoop();

}

void cleanup(void) {
    Prefs::writeChecked();
    Prefs::writeMRU();
    Prefs::writePrefs();
}

int main(int argc, char **argv) {

    char *path;

//    Prefs::showHidden = 1;

    /* read in the data files */
    Prefs::readSys();
    Prefs::readChecked();
    Prefs::readMRU();
    Prefs::readPrefs();

    glutInit(&argc, argv);

    /* optional parameter (path) */
    if (argc > 1) {
	path = argv[1];
    } else {
      path = getcwd(NULL, 0);
      // path = "/home/praxis/Zoo";
    }

    /* optional parameter (depth) */
    if (argc > 2) {
	int depth = atoi(argv[2]);
	if ((depth <= 0) || (depth > 10)) {
	    fprintf(stderr, "Depth should be between 1 and 6, inclusive.\n");
	    exit(2);
	}
	Prefs::depth = depth;
    }

    if (argc > 3) {
	Prefs::breadth = atoll(argv[3]);
    }

    fprintf(stderr, "Reading...");
//    Prefs::breadth = br;
    ourPath = new FFolder(path, Prefs::depth, NULL, 1.0,
			  GColor(0.734375, 0.539062, 0.25),
			  Prefs::folderColor);
    ourPath->setShowDotDot((Prefs::breadth < ourPath->countFolders(0)) 
			   ? 3 : 2);
    fprintf(stderr, "done.\n");

    atexit(cleanup);
    start(argc, argv);
    delete ourPath;
}


