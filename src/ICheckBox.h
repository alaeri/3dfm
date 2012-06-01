/*
 * 3dfm - 3D File Manager
 * $Id: ICheckBox.h,v 1.3 2003/04/08 01:39:24 kristopher Exp $
 */

/*
 * ICheckBox
 */

#ifndef _ICHECKBOX_H_
#define _ICHECKBOX_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>


class ICheckBox {
 private:
    GPoint location;  			/* location of the CheckBox */
    GColor paneColor; 			/* the color for this CheckBox */
    char* Caption;			/* the caption of the CheckBox */
    bool Capture;

    GRect* box;				/* GRect to detect if the checkbox was hit */

 public:
    ICheckBox(GPoint location, GColor paneColor, char* Caption, bool Checked);
    ~ICheckBox();
    void execGL(void);
    void clicked(GPoint pt, int ClickType);
    void CreateClickable();
    void SetCaption(char* NewCaption);
    bool Checked;
};

#endif
