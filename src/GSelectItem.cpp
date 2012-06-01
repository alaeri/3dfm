/*
 * 3dfm - 3D File Manager
 * $Id: GSelectItem.cpp,v 1.6 2003/02/24 20:57:16 adam Exp $
 */

/* system includes */
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

/* 3dfm includes */
#include <GSelectItem.h>

GSelectItem::GSelectItem(FFolder *folder, int id) {
    this->folder = folder;
    item_id = id;
    next = NULL;
    glLoadName(id); 
}

GSelectItem::~GSelectItem(void) {
    if (next) {
	delete next;
    }
}

FFolder *GSelectItem::getFolder(void) {
    return folder;
}

void GSelectItem::link(GSelectItem *new_next) {
    next = new_next;
}

GSelectItem *GSelectItem::getNext(void) {
    return next;
}

int GSelectItem::hasID(int id) {
    return (item_id == id);
}
