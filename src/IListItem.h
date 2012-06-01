/*
 * 3dfm - 3D File Manager
 * $Id: IListItem.h,v 1.4 2003/04/08 17:54:12 adam Exp $
 */

/*
 * IList
 */

#ifndef _ILISTITEM_H_
#define _ILISTITEM_H_

class IListItem {
 private:
    char *name;		/* short name of string */
    char *name_l;	/* long name of string */
    IListItem *next;	/* the next item in the list */
    IListItem *prev;	/* the previous item in the list */
    int checked;	/* click detection */
    int idx;		/* index of the items in the list */

 public:
    IListItem(char *str, char *str_l, unsigned int max, int idx, 
	      int checked = 0);
    ~IListItem(void);
    char *getName(void);
    char *getLongName(void);
    IListItem *getNext(void);
    void setNext(IListItem *next);
    IListItem *getPrev(void);
    void setPrev(IListItem *prev);
    int getChecked(void);
    void setCheck(int check);
    int getIdx(void);
};

#endif
