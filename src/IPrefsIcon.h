/*
 * 3dfm - 3D File Manager
 * $Id: IPrefsIcon.h,v 1.3 2003/04/10 03:26:26 kristopher Exp $
 */

/*
 * IPrefsIcon
 */

#ifndef _IPREFSICON_H_
#define _IPREFSICON_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>


class IPrefsIcon {
 private:
    GPoint location;  			/* location of the button */
    GPoint size;   			/* length of the button */
    GColor paneColor; 			/* the color for this button */

    GRect* Button;			/* GRect to detect if button was hit */

    bool Over;				/* are we over the button */

    //Animation varibles
    GLfloat slidertop;
    GLfloat sliderbottom;
    int slidertopdir;
    int sliderbotdir;

 public:
    IPrefsIcon(GPoint location, GPoint size, GColor paneColor);
    void execGL(void);
    void clicked(GPoint pt, int ClickType);
    void CreateClickable();
    bool Clicked;
    void moved(GPoint pt);
    bool Capture;    			/* is the button captured? */

};

#endif
