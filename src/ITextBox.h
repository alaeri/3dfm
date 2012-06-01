/*
 * 3dfm - 3D File Manager
 * $Id: ITextBox.h,v 1.4 2003/04/04 03:49:07 kristopher Exp $
 */

/*
 * ITextBox
 */

#ifndef _ITEXTBOX_H_
#define _ITEXTBOX_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>

class ITextBox {
 private:
    GPoint location;  		/* location of the pane */
    GPoint size;      		/* size of the pane */
    GColor paneColor; 		/* the color for this pane */

    //Animation variables
    bool CreateCarot;		/* Should we draw the carot? */
    bool direction;		/* direction of the carot alpha */
    float aalpha;		/* variable alpha for carot */
    int CharacterMax;		/* max number of characters box can fit */
    bool number;		/* only accept numbers */

    GRect* Box;			/* used to detect is box was hit */

 public:
    bool FocusLock;		/* do we have focus */
    bool redraw;
    char* string;     		/* the text string */

 public:
    ITextBox(GPoint location, GPoint size, GColor paneColor, bool number);
    ~ITextBox();
    void execGL(void);
    void SetText(char* NewString);
    void CreateClickable();
    void clicked(GPoint pt, int ClickType);
    void SendKeys(unsigned char key);
    void AddCharacter(char* NewCharacter);
    void RemoveCharacter(void);
    void Clear();
    bool IsEmpty();
    void SetFocus(bool focus);
};

#endif
