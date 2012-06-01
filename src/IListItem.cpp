/*
 * 3dfm - 3D File Manager
 * $Id: IListItem.cpp,v 1.6 2003/04/08 17:54:10 adam Exp $
 */

/*
 * IList
 */

/* system includes */
#include <string.h>
#include <stdlib.h>

/* 3dfm includes */
#include <IListItem.h>

/* Constructs the IListItem data structure with the given parameters
 *
 *
 * PARAMS
 *   *str - the short name of the string to be added to the list
 *   *str_l - the long name of the string to be added to the list
 *   max - the maximum size the character can be to be displayed
 *   idx - the index of the number in the list
 *   checked - whether or not the item is checked
 *
 */
IListItem::IListItem(char *str, char *str_l, unsigned int max, int idx, 
		     int checked) {
    name_l = (char *) malloc(sizeof(char) * strlen(str_l) + 1);
    strcpy(name_l, str_l);

    //If the name will fit in the list then put it in...
    if (strlen(str) <= max) {
    	name = (char *) malloc(sizeof(char) * strlen(str) + 1);
	strcpy(name, str);
    } else {
        /*
	 * Otherwise perform the edit.  The edit removes the middle and leaves an
	 * equal amount on the beginning and the end.  For example:
	 *     happyname -> happ...name
	 */
	name = (char *) malloc(sizeof(char) * max + 1);
	int cut = (max - 3) / 2;
	int rem = max - cut - 3;
	strncpy(name, str, cut);
	strcpy(name+cut, "...");
	strcpy(name+cut+3, str+(strlen(str) - rem));
    }

    this->checked = checked;
    this->next = NULL;
    this->prev = NULL;
    this->idx = idx;
}

/* Destroys the item in the list
 *
 *
 * PARAMS
 *   none
 *
 */
IListItem::~IListItem(void) {
    free(name);
    if (next) {
	delete next;
    }
}

/* Returns the short name of the item
 *
 *
 * PARAMS
 *   none
 *
 */
char *IListItem::getName(void) {
    return name;
}

/* Returns the long name of the item
 *
 *
 * PARAMS
 *   none
 *
 */
char *IListItem::getLongName(void) {
    return name_l;
}

/* Returns the next item in the list
 *
 *
 * PARAMS
 *   none
 *
 */
IListItem *IListItem::getNext(void) {
    return next;
}

/* Set the next item in the list
 *
 *
 * PARAMS
 *   *next - the next item to be added in the list
 *
 */
void IListItem::setNext(IListItem *next) {
    this->next = next;
}

/* Returns the name of the previous item
 *
 *
 * PARAMS
 *   none
 *
 */
IListItem *IListItem::getPrev(void) {
    return prev;
}

/* Set the previous item in the list
 *
 *
 * PARAMS
 *   *prev - the previous item to be added to the list
 *
 */
void IListItem::setPrev(IListItem *prev) {
    this->prev = prev;
}

/* Returns if the item is checked
 *
 *
 * PARAMS
 *   none
 *
 */
int IListItem::getChecked(void) {
    return checked;
}

/* Set the current item to checked
 *
 *
 * PARAMS
 *   check - check value of the item
 *           Note: 0 - uncheck an item
 *                 1 - check an item
 *
 */
void IListItem::setCheck(int check) {
    this->checked = check;
}

/* Returns the index of the item
 *
 *
 * PARAMS
 *   none
 *
 */
int IListItem::getIdx(void) {
    return idx;
}
