/*
 * 3dfm - 3D File Manager
 * $Id: IList.h,v 1.13 2003/04/02 21:48:40 adam Exp $
 */

/*
 * IList
 */

#ifndef _ILIST_H_
#define _ILIST_H_

/* system includes */
#include <GL/gl.h>

/* 3dfm includes */
#include <GPoint.h>
#include <GColor.h>
#include <GRect.h>

class IListItem;

class IList {
 private:
    GPoint location;  /* location of the pane */
    GPoint size;      /* size of the pane */
    GColor paneColor; /* the color for this pane */
    int line_max;     /* number of lines we can fit */
    int char_max;     /* number of chars per line */
    IListItem *head;  /* linked list of items */
    IListItem *top;   /* top item to show */
    IListItem *inspos; /* position in list to insert new items */
    long long int item_cnt;     /* number of items */
    long long int remaining_checks;
    int next_idx;     /* index to assign next item */

    GRect **Items;    /* array of clickable items */

    //Animation variables
    bool Animate;
    GLfloat aa;
    bool dohighlight;
    int highlight;
    char* Title;

    //Customize variables
    bool Checkboxes;
    GLfloat shift_distance;

public:
    //Close list flags
    bool CloseList;

    //Navigation flags
    bool DoNavigate;
    char* NavigateLocation;
    
    //Reading flags
    bool ReRead;




 public:
    IList(GPoint location, GPoint size, GColor paneColor, bool Checkbox);
    ~IList();
    int execGL(void);
    void add(char *name, char* name_l, int checked);
    void scroll(int offset);
    void check(int idx);
    void clicked(GPoint pt);
    void CreateClickable();
    void CheckControl();
    void Navigate(int idx);
    void SetTitle(char* NewTitle);

    bool AnimatePane();
    void mouseMove(GPoint);
};

#endif
