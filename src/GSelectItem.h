/*
 * 3dfm - 3D File Manager
 * $Id: GSelectItem.h,v 1.4 2003/02/07 16:27:12 adam Exp $
 */

#ifndef _GSELECTITEM_H_
#define _GSELECTITEM_H_

#include <FFolder.h>

/*
 * GSelectItem
 *
 * This object is the linked list of selectable FFolders
 */

class GSelectItem {
 private:
    GSelectItem *next;   /* the head of the list */
    FFolder *folder;     /* the folder in this list */
    int item_id;         /* OpenGL object id */
 public:
    GSelectItem(FFolder *folder, int id);
    ~GSelectItem(void);
    FFolder *getFolder(void);
    void link(GSelectItem *new_next);
    GSelectItem *getNext(void);
    int hasID(int id);
};

#endif
