/*
 * 3dfm - 3D File Manager
 * $Id: PrefsFiltertab.h,v 1.9 2003/04/18 21:43:34 kristopher Exp $
 */

/*
 * PreftsFiltertab
 */

#ifndef _PREFSFILERTAB_H_
#define _PREFSFILERTAB_H_

/* system includes */
#include <GL/gl.h>
#include <stdlib.h>

/* 3dfm includes */
#include <Prefs.h>
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>
#include <ICheckBox.h>

//The check packet structure
typedef struct t_checkpacket
{
    bool hiddenfolders;
    bool linkedfolders;
    bool systemfolders;
    bool showlabels;
    bool enablechecks;
    bool strokefonts;

} s_checkpacket;

class PrefsFiltertab {
 private:
    GPoint location;  			/* location of the pane */
    GPoint size;   			/* length of the slider bar */
    GColor paneColor; 			/* the color for this pane */
    char* tabTitle;

    void CreateObjects();		/* creates all IFC widgets to be used */
    bool animatePane;			/* do we animate the tab */
    s_checkpacket checkPacket;		/* the check packet */

    //IFC widgets to be used
    ICheckBox* systemCheckBox;
    ICheckBox* hiddenCheckBox;
    ICheckBox* linkedCheckBox;
    ICheckBox* labelsCheckBox;
    ICheckBox* checksCheckBox;
    ICheckBox* fontsCheckBox;


    bool AnimatePane();
    
    //input locks
    bool lockio;

 public:
    PrefsFiltertab(GPoint location, GPoint size, GColor paneColor, char* tabTitle);
    bool execGL(void);
    void clicked(GPoint pt, int ClickType);
    void dragged(GPoint pt);
    void moved(GPoint pt);

    void keypress(unsigned char key, int x, int y);
    void specialkeypress(int key, int x, int y);
    void setdepth(char* depth);
    void setbreadth(char* breadth);
    void setcheckpacket(s_checkpacket checkPacket);
    s_checkpacket getcheckpacket();

    bool redraw;			/* do we redraw */
    bool Animate;			
    GLfloat aa;
};

#endif
