/*
 * 3dfm - 3D File Manager
 * $Id: PrefsLFtab.h,v 1.7 2003/04/17 21:59:08 kristopher Exp $
 */

/*
 * PreftsLFtab
 */

#ifndef _PREFSLFTAB_H_
#define _PREFSLFTAB_H_

/* system includes */
#include <GL/gl.h>
#include <stdlib.h>

/* 3dfm includes */
#include <Prefs.h>
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>
#include <ISlider.h>
#include <ITextBox.h>
#include <IButton.h>
#include <IComboBox.h>
#include <IColorSelect.h>
#include <FFolder.h>

//The color packed structure
typedef struct t_colorpacket
{
    GColor background;
    GColor filelistbox;
    GColor folderlistbox;
    GColor hiddenfolders;
    GColor linkedfolders;
    GColor normalfolders;
    GColor parentfolder;
    GColor statusbar;
    GColor statusbartext;
    GColor systemfolders;
} s_colorpacket;


class PrefsLFtab {
 private:
    GPoint location;  			/* location of the pane */
    GPoint size;   			/* length of the slider bar */
    GColor paneColor; 			/* the color for this pane */
    char* tabTitle;

    void CreateObjects();		/* creates all IFC widgets to be used */
    bool animatePane;			/* do we animate the tab */

    //IFC widgets to be created
    ISlider *depthSlider;
    ISlider *breadthSlider;

    ITextBox *depthText;
    ITextBox *breadthText;
    ITextBox *redText;
    ITextBox *greenText;
    ITextBox *blueText;

    IComboBox *colorCombo;

    IButton *applyButton;

    IColorSelect *colorSelect;

    //Instance of the color packet
    s_colorpacket colorPacket;

    void changecolorview();
    void setcolor();

    //The current color of the selected folder
    GColor savedColor;
    void setRGB(GColor);

    bool AnimatePane();
    
    //input locks
    bool lockio;

 public:
    PrefsLFtab(GPoint location, GPoint size, GColor paneColor, char* tabTitle);
    bool execGL(void);
    void clicked(GPoint pt, int ClickType);
    void dragged(GPoint pt);
    void moved(GPoint pt);

    void keypress(unsigned char key, int x, int y);
    void specialkeypress(int key, int x, int y);
    void setdepth(char* depth);
    void setbreadth(char* breadth);
    int getdepth ();
    int getbreadth ();
    void setcolorpacket(s_colorpacket colorPacket);
    s_colorpacket getcolorpacket();

    bool redraw;
    bool Animate;
    GLfloat aa;
};

#endif
