/*
 * 3dfm - 3D File Manager
 * $Id: IButton.h,v 1.4 2003/04/05 21:44:57 kristopher Exp $
 */

/*
 * IButton
 */

#ifndef _IBUTTON_H_
#define _IBUTTON_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>


class IButton {
 private:
    GPoint location;  			/* location of the button */
    GPoint size;   			/* length of the button */
    GColor paneColor; 			/* the color for this button */
    char* Caption;			/* the caption of the button */

    GRect* Button;			/* GRect to detect if button was hit */
    bool Capture;    			/* is the button captured? */

 public:
    IButton(GPoint location, GPoint size, GColor paneColor, char* Caption);
    ~IButton();
    void execGL(void);
    void clicked(GPoint pt, int ClickType);
    void CreateClickable();
    void SetCaption(char* NewCaption);
    bool Clicked;

};

#endif
