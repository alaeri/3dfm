/*
 * 3dfm - 3D File Manager
 * $Id: FFolder.h,v 1.22 2003/04/10 03:23:13 adam Exp $
 */

#ifndef _FFOLDER_H_
#define _FFOLDER_H_

/* the state of the folder */
#define FOLDER_OPEN       1  /* opened */
#define FOLDER_CLOSE      2  /* closed */
#define FOLDER_TRANSITION 3  /* in the process or opening or closing */

#include <FFSObject.h>
#include <FFile.h>
#include <GColor.h>
#include <GPoint.h>
#include <GSelectList.h>
#include <AVLTree.h>

/*
 * FFolder
 *
 * This object defines a folder to be drawn.
 */

class FFolder : public FFSObject {
 private:
//    FFolder *headFolder;  /* the first child folder */
    AVLTree folders;
//    FFolder *nextFolder;
//    FFile *headFile;      /* the first child file */
//    FFile *nextFile;
    AVLTree files;
    void addFolder(FFolder *folder);
    void addFile(FFile *file);
    /* graphical attributes */
    GPoint location;  /* location of the folder's center */
    GLfloat scale;    /* the scaling factor at which to draw
			 this folder */
    GColor innerColor;     /* the color for this folder */
    GColor outerColor;     /* the outer color for this folder */
    int state;        /* are we open or closed ? */
    int opening;      /* set when the folder is in the midst of its opening
			 animation */
    int closing;      /* set when the folder is in the midst of its closing
			 animation */
    int hasParent;    /* if we already added the parent to the folder list */
    int haveCheckData;
    int checkP;
    GLfloat z_rot;        /* rotation around the Y axis */
    GLfloat z_pos;
    GLfloat theta;    /* angle the folder is open */
    GLfloat x_pos;
    const GLfloat zrot_closed;  /* rotation around Y axis when closed */
    const GLfloat zrot_opened;  /* rotation around Y axis when open */
    const GLfloat zpos_closed;
    const GLfloat zpos_opened;
    const GLfloat scale_opened;
    const GLfloat scale_closed;
    const GLfloat theta_closed;
    const GLfloat theta_opened;
    const GLfloat x_opened;
    const GLfloat x_closed;
    int showLabel;
    int showDotDot;
 public:
    FFolder(char *path, int depth, FFolder *parent, GLfloat scale,
	    GColor newInnerColor, GColor newOuterColor);
    int execGL(GSelectList *selector);
    void execChildGL(GSelectList *selector);
    GPoint getLocation(void);
    void setLocation(GPoint pt);
    virtual ~FFolder(void);
    FFolder *getChildFolder(int idx);
    FFolder *getNextFolder(int checked);
    void rewindFolders(void);
    FFile *getChildFile(int idx);
    FFile *getNextFile(void);
    void rewindFiles(void);
    int countFiles(void);
    int countFolders(int checked);
    void positionChildren(void);
    void DumpChildList();
    int isLink(void);  /* checks if this is a symbolic link */
    int isHidden(void); /* checks if this is a hidden folder */
    int isSystem(void);
    int isChecked(int forceQuery);
    void setShowDotDot(int show);
    int getShowDotDot(void);
    void checkMRUs(void);
    int setState(int state);  /* tell the folder to open or close */
    int getState(void);        /* is the folder open, closed, or 
				  tranistioning? */
    void dumpFolderList(void);
    void setShowLabel(int show);
};

#endif
