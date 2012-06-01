/*
 * 3dfm - 3D File Manager
 * $Id: FFolder.cpp,v 1.56 2003/05/04 22:42:09 adam Exp $
 */
#include <stdio.h>

/* system includes */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <GL/glut.h>

/* 3dfm includes */
#include <FFolder.h>
#include <FFile.h>
#include <FFSObject.h>
#include <GPoint.h>
#include <GColor.h>
#include <GLine.h>
#include <Prefs.h>

/* Returns the pointer to the first child.  Each child has next and prev
 * pointers to the other children in a linked list format.
 *
 * PARAMS
 *   path - the path to scan from
 *   depth - the number of relative levels to traverse
 */
FFolder::FFolder(char *path, int depth, FFolder *parent, GLfloat scale, 
		 GColor newInnerColor, GColor newOuterColor) 
    :    zrot_closed(10), zrot_opened(90), zpos_closed(0.0), 
	 zpos_opened(3.0), haveCheckData(0), checkP(0),
	 scale_closed(1.0), scale_opened(7.89), theta(0.628), 
	 theta_closed(0.628), theta_opened(2.6),
	 x_closed(0.0), x_opened(9.5), showLabel(0), showDotDot(0),
	 state(FOLDER_CLOSE)
{

    location = GPoint(0.0, 0.0, 0.0);
    z_pos = 0.0;
    x_pos = 0.0;
    this->scale = scale;
    innerColor = newInnerColor;
    outerColor = newOuterColor;

    /* our files AVL tree uses name compares only */
    files.setCompare(AVL_CMP_NAME);

    /* initially the folder is neither opening nor closing */
    opening = 0;
    closing = 0;
    z_rot = zrot_closed;

    if ((Prefs::depth + depth) == 1) {
      showLabel = 1;
    }

    /* strip the path */
    FFSObject::stripName(path);

//    headFolder = NULL;
//    headFile = NULL;
    hasParent = false;
    DIR* dir;
    struct dirent *dent;  // one entry in the directory

    /* get our information */
    struct stat ourstat;
    stat(path, &ourstat);
    init(parent, &ourstat, path);

    /* see if we are a link */
    if (isLink()) {
	outerColor = Prefs::linkColor;
    }

    /* see if we are hidden */
    if (isHidden()) {
	outerColor = Prefs::hiddenColor;
    }

    /* open the directory */
    dir = opendir(name);
    if (dir == NULL) {
#ifdef _DEBUG_FSAL
        switch (errno) {
	    case EACCES: 
		fprintf(stderr, "Permission denied reading directory"
				 " %s.\n", path); 
		break;
	    case EMFILE: 
		fprintf(stderr, "Too many file descriptors in use"
			" by process.  (OPENDIR)\n"); 
		break;
	    case ENFILE: 
		fprintf(stderr, "Too many files are currently open"
				 " in the system.\n"); 
		break;
	    case ENOENT: 
		fprintf(stderr, "Directory does not exist.\n"); 
		break;
	    case ENOMEM: 
		fprintf(stderr, "Insufficient memory to complete"
				 " the operation.\n"); 
		break;
	    case ENOTDIR: 
		fprintf(stderr, "%s is not a directory\n", path);
		break;
	    default:
		fprintf(stderr, "ERROR! Unkown error.\n");
	}
#endif
    } else {

	char *child_name;
	/* read the children */
	while ((dent = readdir(dir)) != NULL) {
	    if (strcmp(dent->d_name, ".") == 0) {
		continue;
	    } 
	    if (strcmp(dent->d_name, "..") == 0) {
		if (depth < 0) {
		    continue;
		} else {
		    depth = -depth;
		}
	    }

	    struct stat *statbuf;

	    int name_size = strlen(path) + strlen(dent->d_name) + 2;
	    child_name = (char *) malloc(sizeof(char) * name_size);
	    
	    strncpy(child_name, path, strlen(path)+1);
	    strcat(child_name, "/");
	    strcat(child_name, dent->d_name);
#ifdef _DEBUG_FSAL
	    fprintf(stderr, "Level %d: %s ", depth, child_name);
#endif
	    statbuf = (struct stat *) malloc(sizeof(struct stat));
	    if (stat(child_name, statbuf)) {
#ifdef _DEBUG_FSAL
		switch (errno) {
		    case EBADF:
			fprintf(stderr, "EBADF\n");
			break;
		    case ELOOP:
			fprintf(stderr, "ELOOP\n");
			break;
		    case EFAULT:
			fprintf(stderr, "EFAULT\n");
			break;
		    case ENAMETOOLONG:
			fprintf(stderr, "ENAMETOOLONG\n");
			break;
		    case EACCES: 
			fprintf(stderr, "Permission denied reading directory"
				" %s.\n", path); 
			break;
		    case EMFILE: 
			fprintf(stderr, "Too many file descriptors in use"
				" by process. (STAT)\n"); 
			break;
		    case ENFILE: 
			fprintf(stderr, "Too many files are currently open"
				" in the system.\n"); 
			break;
		    case ENOENT: 
			fprintf(stderr, "Directory does not exist.\n"); 
			break;
		    case ENOMEM: 
			fprintf(stderr, "Insufficient memory to complete"
				" the operation.\n"); 
			break;
		    case ENOTDIR: 
			fprintf(stderr, "%s is not a directory\n", path);
			break;
		    default:
			fprintf(stderr, "ERROR! Unkown error.\n");
		}
#endif
		free(child_name);
		free(statbuf);
		continue;
	    }
	    if (S_ISREG(statbuf->st_mode)) {
#ifdef _DEBUG_FSAL
		fprintf(stderr, " (regular)\n");
#endif
		FFile *file;
		file = new FFile(this, statbuf, child_name);
		this->addFile(file);
	    } else if (S_ISDIR(statbuf->st_mode)) {
#ifdef _DEBUG_FSAL
		fprintf(stderr, " (directory)\n");
#endif
		
		if (depth != 0) {
		    FFolder *child;
		    GColor newColor(outerColor.getRed() - 0.2, 
				    outerColor.getGreen() + 0.2,
				    outerColor.getBlue());
		    if (strcmp(dent->d_name, "..") == 0) {
			child = new FFolder(child_name, 0, this,
					    (GLfloat) scale / 1.68,
					    innerColor,
					    Prefs::parentColor);
			child->isParent(true);
			child->setShowLabel(1);
		    } else {
			child = new FFolder(child_name, depth + 1, this,
					    (GLfloat) scale / 1.68,
					    innerColor, newColor);
		    }

		    /* is this our parent? */
// 		    if (FFSObject::isParentOf(child->getName(),
// 					      this->getName())) {
// 			child->isParent(true);
// 		    }
		    if (((child->isHidden()) && (!Prefs::showHidden)) 
			|| ((child->isLink()) && (!Prefs::showLinks))
			|| ((child->isSystem()) && (!Prefs::showSystem))) {
			delete child;
			child = NULL;
		    }
		    
		    if (child) {
			this->addFolder(child);
		    }
		}
	    } else {
#ifdef _DEBUG_FSAL
		fprintf(stderr, " Not regular file or directory\n");
#endif
	    }

	    free(statbuf);
	    free(child_name);
//	close(f);

	} // while
	closedir(dir);
    } // if readable dir

    /* always show parent ... well, only add if ...
     *  - we have no children (probably due to an unlistable directory)
     *  - we are not a parent ourselves (just in chase, should never be the 
     *    case)
     *  - and our depth is positive (it gets reversed when a parent has 
     *    already been added to this view, and we only want one parent per 
     *    view)
     */
    if ((countFolders(0) == 0) 
	&& (strcmp(path, "..") != 0)
	&& (depth > 0)) {

	/* allocate memory for child name (the number of characters in this
	   path plus three characters (two dots and a null)) */
	char *parent_name = (char *) malloc(sizeof(char) * strlen(path) + 3);
	strcpy(parent_name, path);
	strcat(parent_name, "/..");
	FFolder *child = new FFolder(parent_name, 0, this, 
				     (GLfloat) scale / 1.68,
				     innerColor, GColor(1.0, 0.0, 0.0));
	this->addFolder(child);
	free(parent_name);
    }
    positionChildren();
}

void FFolder::positionChildren(void) {
    int count = 0;
    int child_cnt = countFolders(Prefs::showOnlyChecked);
    if (child_cnt > Prefs::breadth) {
	child_cnt = Prefs::breadth;
    }
    FFolder *child;
    rewindFolders();
    while (((child = getNextFolder(Prefs::showOnlyChecked)) != NULL) 
	   && (count < child_cnt)) {
	GPoint newPoint(location, child_cnt, count, 
			24.0 * (scale * scale * scale));
	if (!child) continue;
//  	if (!showLabel) {
//  	    child->setShowLabel(1);
//  	} else {
//  	    child->setShowLabel(0);
//  	}
	child->setLocation(newPoint);
	child->positionChildren();
	count++;
    } // for each point
}

/* Destructor deletes the linked lists.
 */
FFolder::~FFolder(void) {
//    if (headFolder) {
#ifdef _DEBUG_FSAL
    fprintf(stderr, "FFolder::~FFolder - deleteing headFolder.\n");
#endif
    //delete headFolder;
    // }
    //if (headFile) {
    //delete headFile;
    //}
    folders.makeEmpty();
}

// FFolder *FFolder::getChildFolder(int idx) {
//     FFolder *curr;
//     curr = headFolder;
//     int count = 0;
//     while (curr != NULL) {
// 	if (count++ == idx) {
// 	    return curr;
// 	}
// 	curr = (FFolder *) curr->getNext();
//     }

// }

FFolder *FFolder::getNextFolder(int checked = 0) {
    if (checked) {
	FFolder *f;
	while ((f = (FFolder *) folders.getNext()) != NULL) {
	    if ((f->isChecked(0))
		|| (f->isParent())) {
		return f;
	    }
	}
	return NULL;
    }
    return (FFolder *) folders.getNext();
}

void FFolder::rewindFolders(void) {
    folders.makeList();
}

// FFile *FFolder::getChildFile(int idx) {
//     FFile *curr;
//     curr = headFile;
//     int count = 0;
//     while (curr != NULL) {
// 	if (count++ == idx) {
// 	    return curr;
// 	}
// 	curr = (FFile *) curr->getNext();
//     }
// }

FFile *FFolder::getNextFile(void) {
    return (FFile *) files.getNext();
}

void FFolder::rewindFiles(void) {
    files.makeList();
}

/* Returns a count of the number of files this folder contains.
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   An integer count of the files.
 */
int FFolder::countFiles() 
{
    return files.getCount();
}

/* Returns a count of the number of folders this folder contains.
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   An integer count of the folders.
 */

int FFolder::countFolders(int checked) 
{
    if (checked) {
	int count = 0;
	FFolder *f;
	rewindFolders();
	while ((f = getNextFolder()) != NULL) {
	    if ((f->isChecked(0))
		|| (f->isParent())) {
		count++;
	    }
	}
	return count;
    }

    return folders.getCount();
}

void FFolder::addFile(FFile *file) {
    files.insert(file);
}

void FFolder::addFolder(FFolder *folder) {

    folders.insert(folder);

    /* check that we don't have too many MRU folders */
//    folderscheckMRUs();
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

int FFolder::execGL(GSelectList *selector) {

    if (selector) {
	selector->add(this);
    }

//    glLoadIdentity();
   /* scale to a factor */
    glPushMatrix();
    GPoint newLoc = location - GPoint(x_pos, 0.0, -z_pos);
//    glTranslatef(location.getX(), location.getY(), location.getZ());
    newLoc.translate();
    glScalef(scale, scale, scale);
    glRotatef(15.0, 0.0, 1.0, 0.0); // rotate the scene 20 deg ccw about Y
    glRotatef(z_rot, 0.0, 0.0, 1.0); // rotate the scene z_rot deg ccw about Z

    /* enable face culling */
    glEnable(GL_CULL_FACE);

    /* inner face */
    glCullFace(GL_FRONT);
    innerColor.execFullGL();

    /* inner main square */
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-2.0, +1.5, 0.0);
    glVertex3f(+2.0, +1.5, 0.0);
    glVertex3f(-2.0, -1.4, 0.0);   // to eliminate jaggies
    glVertex3f(+2.0, -1.4, 0.0);   // to eliminate jaggies

    GLfloat y = 2.8 * cos(theta) - 1.5;
    GLfloat z = 2.8 * sin(theta);
//    glNormal3f(0.0, 1.0 * sin(theta), 1.0 * cos(theta));
    glNormal3f(0.0, -0.197395598, 0.980580765);
    glVertex3f(-2.0, y, z);
    glVertex3f(+2.0, y, z);

//     glVertex3f(-2.0, +1.0, +0.5);
//     glVertex3f(+2.0, +1.0, +0.5);
    glEnd();

    /* inner tab */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-1.25,  +1.5, 0.0);
    glVertex3f( -2.0,  +1.5, 0.0);
    glVertex3f(-1.75, +1.75, 0.0);
    glVertex3f(-0.75, +1.75, 0.0);
    glVertex3f( -0.5,  +1.5, 0.0);
    glEnd();

    /* outer face */
    glCullFace(GL_BACK);
    outerColor.execFullGL();

    /* outer main square */
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-2.0, +1.5,  0.0);
    glVertex3f(+2.0, +1.5,  0.0);
    glVertex3f(-2.0, -1.5,  0.0);
    glVertex3f(+2.0, -1.5,  0.0);

//     glNormal3f(0.0, 1.0 * sin(theta), 1.0 * cos(theta));
    glNormal3f(0.0, -0.197395598, 0.980580765);
    glVertex3f(-2.0, y, z);
    glVertex3f(+2.0, y, z);


//      glVertex3f(-2.0, +1.0, +0.5);
//      glVertex3f(+2.0, +1.0, +0.5);
    glEnd();

    /* outer tab */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0, 0.0, -11.0);
    glVertex3f(-1.25,  +1.5, 0.0);
    glVertex3f( -2.0,  +1.5, 0.0);
    glVertex3f(-1.75, +1.75, 0.0);
    glVertex3f(-0.75, +1.75, 0.0);
    glVertex3f( -0.5,  +1.5, 0.0);
    glEnd();

    glDisable(GL_CULL_FACE);
    /* do the name */
    glNormal3f(0.0, 0.0, 1.0);
    // ... OpenGL initialization commands... (no more fonts)
    int baby_count = 0;
    while ((state == FOLDER_CLOSE) && (baby_count < showDotDot)) {
	FFolder f("/", 0, NULL, 0.2, innerColor, outerColor); 
	f.setLocation(location + GPoint(3.0 + (float) baby_count, -2.0, 0.0));
	f.setShowLabel(0);
	f.execGL(selector);
	baby_count++;
    }

    glPopMatrix();

    /* try a label */
    if ((showLabel) && (Prefs::showLabels)) {
      char *str = getShortName();
      glDisable(GL_FOG);
      glDepthMask(GL_FALSE);
      glDisable(GL_LIGHTING);
      glDisable(GL_LIGHT0);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
      glDisable(GL_LINE_SMOOTH);
      glDisable(GL_POLYGON_SMOOTH);
      
      outerColor.execFullGL();
      if (Prefs::strokeFont) {
	  glPushMatrix();
	  glTranslatef(location.getX(), location.getY() - 2.0,
		       location.getZ() + 1.0);
	  glScalef(0.01, 0.01, 0.01);
	  glRotatef(20.0, 0.0, 0.0, 1.0);
	  int i,l=strlen(str);
	  
	  for(i=0;i<l;i++)
	      glutStrokeCharacter(GLUT_STROKE_ROMAN, *str++);
	  glPopMatrix();
      } else {
	  glRasterPos3f(location.getX(), location.getY() - 2.0, 
			location.getZ() + 1.0);
//	  ourPrintString(GLUT_BITMAP_HELVETICA_12, str);
	  ourPrintString(GLUT_BITMAP_HELVETICA_18, str);
      }

      glEnable(GL_BLEND);
      glEnable(GL_LINE_SMOOTH);
      glEnable(GL_POLYGON_SMOOTH);
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glEnable(GL_FOG);
    
    }

    /* check for opening animation effect */
    if (opening) {
	int done = 0;
	if (x_pos > x_opened) {
	    done++;
	} else {
	    x_pos += 1.0;
	}
	if (z_pos > zpos_opened) {
	    done++;
	} else {
	    z_pos += 1.0;
	}
	if (z_rot > zrot_opened) {
	    done++;
	} else {
	    z_rot += (GLfloat) 5.0;
	}
	if (scale > scale_opened) {
 	    done++;
 	} else {
 	    scale += 0.54;
 	}
	if (theta > theta_opened) {
	    done++;
	} else {
	    theta += 0.28;
	}
	if (done == 5) {
	    state = FOLDER_OPEN;
	    opening = 0;
	    return 0;
	} else {
	    return 1;
	}
    }

    /* check for closing animation effect */
    if (closing) {
	int done = 0;
	if (x_pos <= x_closed) {
	    done++;
//	    x_pos = 0.0;
	} else {
	    x_pos -= 1.0;
	}
	if (z_pos <= zpos_closed) {
	    done++;
	} else {
	    z_pos -= 1.0;
	}
	if (z_rot < zrot_closed) {
	    done++;
	} else {
	    z_rot -= (GLfloat) 5.0;
	}
 	if (scale < scale_closed) {
 	    done++;
 	} else {
 	    scale -= 0.54;
 	}
  	if (theta < theta_closed) {
  	    done++;
  	} else {
	    theta -= 0.28;
  	}
	if (done == 5) {
	    state = FOLDER_CLOSE;
	    closing = 0;
	    return 0;
	} else {
	    return 1;
	}
    }

    /* not doing animation */
    return 0;
}

void FFolder::execChildGL(GSelectList *selector) {
    int i;
    int child_cnt = countFolders(0);
    if (child_cnt > Prefs::breadth) {
	child_cnt = Prefs::breadth;
    }
    int count = 0;
#ifdef _DEBUG_DRAW
    fprintf(stderr, "REDRAW START -- Drawing child_cnt = %d\n", child_cnt);
#endif
    FFolder *childFolder;
    rewindFolders();
    while (((childFolder = getNextFolder()) != NULL) 
	   && (++count <= child_cnt)) {
	if (!childFolder) continue;
#ifdef _DEBUG_DRAW
	fprintf(stderr, "GFolder::execChildGL %s -> %s\n",
		getName(),
		childFolder->getName());
#endif
	if (((Prefs::showOnlyChecked) && (childFolder->isChecked(0)))
	    || (!Prefs::showOnlyChecked)
	    || (childFolder->isParent())) {

	    /* draw the line between */
	    GLine connector(location, childFolder->getLocation(), 
			    GColor(1.0, 1.0, 1.0));
	    connector.execGL();
	    
	    childFolder->execGL(selector);
	    childFolder->execChildGL(selector);
	}
    }
}

GPoint FFolder::getLocation(void) {
    return location;
}

void FFolder::setLocation(GPoint pt) {
    location.setX(pt.getX());
    location.setY(pt.getY());
    location.setZ(pt.getZ());
}

// void FFolder::DumpChildList()
// {
//     FFSObject *curr = headFolder;

//     while (curr != NULL){

// 	fprintf(stderr, "%s->",
// 		curr->getName());

// 	curr = curr->getNext();
//     }

// 	fprintf(stderr, "End of list!\n");
// }

int FFolder::isLink(void) {
    char buf[10];
    if (readlink(name, buf, 10) == -1) {
	return 0;
    }

    return 1;
}

int FFolder::isHidden(void) {
    if (getShortName()[0] == '.') {
	return 1;
    }
    return 0;
}

int FFolder::isSystem(void) {
    if (Prefs::isSystemName(getShortName())) {
	return 1;
    }
    return 0;
}

int FFolder::isChecked(int forceQuery) {

    if (forceQuery || !haveCheckData) {

	/* check if this folder is checked */
	if (Prefs::isChecked(getName())) {
	    checkP = 1;
	}

	/* check the sub folders (and recursively, their children) */
	FFolder *f;
	rewindFolders();
	while ((f = getNextFolder()) != NULL) {
	    if (f->isChecked(forceQuery)) {
		checkP = 1;
	    }
	}

	haveCheckData = 1;

    }
    
    return checkP;
}

// void FFolder::checkMRUs(void) {

//     int mru_cnt = 0;
//     FFolder *curr = headFolder;
//     FFolder *prev = headFolder;
//     FFolder *offender = NULL;
//     while (curr != NULL) {
// 	if ((Prefs::getMRU(curr->getName()) != 0) 
// 	    && (++mru_cnt > Prefs::mru_max)) {
// 	    offender = curr;
// 	    prev->link(curr->getNext());
// 	    break;
// 	}
// 	prev = curr;
// 	curr = (FFolder *) curr->getNext();
//     }

//     /* if we don't have an offender, we are done checking */
//     if (offender == NULL) {
// 	return;
//     }

// //    Prefs::removeMRU(offender->getName());

//     curr = (FFolder *) curr->getNext();
//     while ((curr != NULL) && (curr->compareName(offender) <= 0)) {
// 	prev = curr;
// 	curr = (FFolder *) curr->getNext();
//     }

//     if (curr == prev) {
// 	headFolder = offender;
// 	offender->link(curr);
//     } else {
// 	offender->link(curr);
// 	prev->link(offender);
//     }


// }

int FFolder::setState(int state) {
    if (this->state == FOLDER_TRANSITION) {
	return 0;
    }
    if ((state == FOLDER_OPEN) && (this->state != FOLDER_OPEN)) {
	opening = 1;
	this->state = FOLDER_TRANSITION;
//	state = FOLDER_OPEN;
	return 1;
    }
    if ((state == FOLDER_CLOSE) && (this->state != FOLDER_CLOSE)) {
	closing = 1;
	this->state = FOLDER_TRANSITION;
	return 1;
    }
    return 0;
}

int FFolder::getState(void) {
    return state;
}

// void FFolder::dumpFolderList(void) {
//     FFSObject *curr = headFolder;
//     while (curr != NULL) {
// 	fprintf(stderr, "%s->", curr->getName());
// 	curr = curr->getNext();
//     }
//     fprintf(stderr, "EOL\n");
// }

void FFolder::setShowLabel(int show) {
    showLabel = show;
}

void FFolder::setShowDotDot(int show) {
    showDotDot = show;
}

int FFolder::getShowDotDot(void) {
    return showDotDot;
}
