/*
 * 3dfm - 3D File Manager
 * $Id: PrefsControl.h,v 1.7 2003/04/09 20:59:15 kristopher Exp $
 */

/*
 * ISlider
 */

#ifndef _PREFSCONTROL_H_
#define _PREFSCONTROL_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <IButton.h>
#include <PrefsLFtab.h>
#include <PrefsFiltertab.h>
#include <GRect.h>

class PrefsControl {
 private:
    GPoint location;  			/* location of the pane */
    GPoint size;   			/* length of the slider bar */
    GColor paneColor; 			/* the color for this pane */
    GColor titlebarColor;		/* the color of the title bar */
    char* titleText;

    void CreateObjects();		/* creates all IFC widgets to be used */
    void CreateClickable();		/* creates clickable objects */

    bool animatePane;			/* do we anumate the pane? */

    //IFC buttons
    IButton *okButton;
    IButton *cancelButton;

    //Clickable objects
    GRect *lfTab;
    GRect *filterTab;

    //the current tab
    int tab;

    //The two tabs to be created
    PrefsLFtab *lf;
    PrefsFiltertab *filter;

    //Anime the pane
    bool AnimatePane();
    GLfloat aa;
    bool Animate;
    
    //input locks
    bool lockio;

 public:
    PrefsControl(GPoint location, GPoint size, GColor paneColor, GColor titlebarColor, char* titleText);
    bool execGL(void);
    void clicked(GPoint pt, int ClickType);
    void dragged(GPoint pt);
    void moved(GPoint pt);

    void keypress(unsigned char key, int x, int y);
    void specialkeypress(int key, int x, int y);
    void setdepth(int depth);
    void setbreadth(int breadth);
    int getdepth ();
    int getbreadth ();
    void setcolorpacket(s_colorpacket colorPacket);
    s_colorpacket getcolorpacket();
    void setcheckpacket(s_checkpacket checkPacket);
    s_checkpacket getcheckpacket();

    bool redraw;			/* do we redraw */
    int exitStatus;			/* do we exit the dialog */
};

#endif
