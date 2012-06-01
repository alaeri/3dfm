 /*
 * 3dfm - 3D File Manager
 * $Id: ISearchBox.h,v 1.8 2003/04/08 01:55:50 kristopher Exp $
 */

/*
 * PreftsLFtab
 */

#ifndef _ISEARCHBOX_H_
#define _ISEARCHBOX_H_

/* system includes */
#include <GL/gl.h>
#include <stdlib.h>

/* 3dfm includes */
#include <ITextBox.h>
#include <IButton.h>

class ISearchBox {
 private:
    GPoint location;  			/* location of the pane */
    GColor paneColor; 			/* the color for this pane */

    void CreateObjects();		/* IFC objects to be created */
    bool animatePane;			/* do you we animate */

    ITextBox *searchText;		/* the text in the textbox */

    //IFC widgets
    IButton *findButton;
    IButton *clearButton;
    IButton *closeButton;

    bool notFound;			/* put up the not found notification */

    //Animation variables
    bool AnimatePane();
    GLfloat aa;
    bool Animate;

 public:
    ISearchBox(GPoint location, GColor paneColor);
    bool execGL(void);
    void clicked(GPoint pt, int ClickType);
    char* gettext();
    void keypress(unsigned char key, int x, int y);
    void notfound();

    //Box control variables
    bool closeBox;
    bool haveData;
    bool redraw;
};

#endif
