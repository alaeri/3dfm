/*
 * 3dfm - 3D File Manager
 * $Id: GSelectList.cpp,v 1.6 2003/02/24 20:57:17 adam Exp $
 */

/* system includes */
#include <stdlib.h>

/* 3dfm includes */
#include <GSelectList.h>
#include <GSelectItem.h>

/*
 * GSelectList
 *
 * This object stores pointers to FFolders that match OpenGL object IDs.
 */

GSelectList::GSelectList(void) {
    head = last = NULL;
    next_id = 1;
}

/* Destructor destroys the list of selectable objects by deleting the head
 * of the list, which will delete its head and so on down the line until the
 * end of the list (NULL) is encountered.
 */
GSelectList::~GSelectList(void) {
    if (head) {
	delete head;
	head = NULL;
    }
}

/* Adds a FFolder to the linked-list of selectable objects.  If the list is
 * empty, that item becomes the list, otherwise, the item is appended to the
 * end of the list.  A 'last' pointer keeps track of the end.
 */ 
void GSelectList::add(FFolder *folder) {
    if (head == NULL) {
	last = head = new GSelectItem(folder, next_id++);
    } else {
	last->link(new GSelectItem(folder, next_id++));
	last = last->getNext();
    }
}

/* Returns the FFolder at a particular position of the list, or NULL if the
 * index is not found.  The items in the list should be numbered sequentially,
 * but that does not need to be the case because we check the index via the
 * hasID(int) method.
 */
FFolder *GSelectList::getHit(int idx) {
    GSelectItem *curr = head;
    while (curr != NULL) {
	if (curr->hasID(idx)) {
	    return curr->getFolder();
	}
	curr = curr->getNext();
    }
    return NULL;
}
