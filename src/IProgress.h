/*
 * 3dfm - 3D File Manager
 * $Id: IProgress.h,v 1.3 2003/02/28 01:09:05 kristopher Exp $
 */

/*
 * IList
 */

#ifndef _IPROGRESS_H_
#define _IPROGRESS_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <string.h>
#include <stdio.h>

class IProgress {
 private:
    GPoint location;  			/* location of the pane */
    GLfloat length;   			/* length of the slider bar */
    GColor paneColor; 			/* the color for this pane */
    GColor progressColor; 		/* the color of the progress bar */
    float current;  			/* current position of the slider must */
                    			/* be a percentage between 0 and 100 */
    char* title; 			/* labels */
    GLfloat CalculateLocation(void); 	/* where the bar needs to be drawn */
    GLfloat aalpha;
    bool direction;

 public:
     bool redraw;

 public:
    IProgress(GPoint location, GLfloat length, GColor paneColor, GColor
	    progressColor, float current, char* title);
    void execGL(void);
    void UpdateCurrent(int new_current);
};

#endif
