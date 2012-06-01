/*
 * 3dfm - 3D File Manager
 * $Id: ISlider.h,v 1.7 2003/03/26 03:51:13 kristopher Exp $
 */

/*
 * ISlider
 */

#ifndef _ISLIDER_H_
#define _ISLIDER_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>


class ISlider {
 private:
    GPoint location;  			/* location of the pane */
    GLfloat length;   			/* length of the slider bar */
    GColor paneColor; 			/* the color for this pane */
    int min;  				/* minimum value of slider */
    int max;  				/* maximum value of slider */
    float start;  			/* initial setting of slider */
    bool labels; 			/* labels */
    GLfloat CalculateLocation(float);	/* where the slider needs to be drawn */
    GRect* SliderTab;			/* detect if tab was hit */
    GRect* SliderDevice;  		/* detect is rest of slider was hit */
    bool Capture;			/* is the mouse captured */

 public:
    ISlider(GPoint location, GLfloat length, GColor paneColor, bool labels,
                 int min, int max, float start);
    bool execGL(void);
    void clicked(GPoint pt, int ClickType);
    void dragged(GPoint pt);
    void CreateClickable();
    int GetValue();
    void SetValue(int NewValue);
    int GetMin();
    int GetMax();
    bool redraw;
    bool Focus;
};

#endif
