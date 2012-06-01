/*
 * 3dfm - 3D File Manager
 * $Id: GSelectList.h,v 1.4 2003/02/07 16:26:27 adam Exp $
 */

#ifndef _GSELECTLIST_H_
#define _GSELECTLIST_H_

class FFolder;
class GSelectItem;

/*
 * GSelectList
 *
 * This object stores pointers to FFolders that match OpenGL object IDs.
 */

class GSelectList {
 private:
    GSelectItem *head;   /* the head of the list */
    GSelectItem *last;   /* the last entry on the list */
    int next_id;     /* next object ID */
 public:
    GSelectList(void);
    ~GSelectList(void);
    void add(FFolder *folder);
    FFolder *getHit(int idx);
};

#endif
