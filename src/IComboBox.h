/*
 * 3dfm - 3D File Manager
 * $Id: IComboBox.h,v 1.3 2003/04/04 02:30:14 kristopher Exp $
 */

/*
 * IComboBox
 */

#ifndef _ICOMBOBOX_H_
#define _ICOMBOBOX_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>

class IListItem;

class IComboBox {
 private:
    GPoint location;  /* location of the pane */
    GPoint size;      /* size of the pane */
    GColor paneColor; /* the color for this pane */
    int line_max;     /* number of lines we can fit */
    int char_max;     /* number of chars per line */
    IListItem *head;  /* linked list of items */
    IListItem *top;   /* top item to show */
    long long int item_cnt;     /* number of items */
    int next_idx;     /* index to assign next item */

    GRect **Items;    /* array of clickable items */
    GRect *DownButton;
    GRect *Device;

    //Drawing Variables
    GPoint ListBoxLocation;
    GPoint ListBoxSize;

    //Animation variables
    bool Animate;
    GLfloat aa;
    bool dohighlight;
    int highlight;

    bool FocusLock;
    //Customize variables
    //GLfloat shift_distance;

public:
    //Close list flags
    bool CloseList;
    bool OpenList;

    //Navigation flags
    bool Selected;
    char* Text;
    char* TextLongName;
    bool TitleChange;

    //Reading flags
    bool ReRead;




 public:
    IComboBox(GPoint location, GPoint size, GColor paneColor, int itemNumber);
    ~IComboBox();
    int execGL(void);
    void add(char *name);
    void scroll(int offset);
    void check(int idx);
    bool clicked(GPoint pt, int ClickType);
    void CreateClickable();
    void SetAsText(int idx);
    char* GetText();
    void SetText(char* NewText);
    void SendKeys(int key);

    bool AnimatePane();
    void mouseMove(GPoint);
};

#endif
