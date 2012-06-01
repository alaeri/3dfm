/*
 * 3dfm - 3D File Manager
 * $Id: FFSObject.cpp,v 1.24 2003/04/08 17:54:09 adam Exp $
 */

/* system includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* 3dfm includes */
#include <FFSObject.h>
#include <Prefs.h>

/*
 * FFSObject
 *
 * This object defines the basic properties of file system objects.
 */

FFSObject::FFSObject(void) {
}

FFSObject::FFSObject(FFolder *new_parent, struct stat *new_stat, 
		     char *new_name) {
    init(new_parent, new_stat, new_name);
}

void FFSObject::init(FFolder *new_parent, struct stat *new_stat,
		     char *new_name) {
    parent = new_parent;
    int name_len = strlen(new_name);
    name = (char *) malloc(sizeof(char) * name_len + 1);
    strncpy(name, new_name, strlen(new_name) + 1);
    owner = new_stat->st_uid;
    group = new_stat->st_gid;
    modify = new_stat->st_mtime;
    create = new_stat->st_ctime;
    next = NULL;
    parentP = false;
}

/* Desctructor releases memory.
 */
FFSObject::~FFSObject(void) {
#ifdef _DEBUG_FSAL
	fprintf(stderr, "Descructing %s.\n", name);
#endif
    if (next) {
#ifdef _DEBUG_FSAL
	fprintf(stderr, "Deleting next.\n");
#endif
	delete next;
    }
    free(name);
}

/* Returns the name of the current object
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   A pointer to the name of the current object
 */

char* FFSObject::getName()
{
    return name;
}

/* Returns the name of the current object
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   A pointer to the name of the current object
 */

char* FFSObject::getShortName()
{
    if (strcmp(name, "/") == 0) {
	return name;
    }
#ifdef _DEBUG_FSAL
    fprintf(stderr, "FFSObject::getShortName() name = %s\n", name);
#endif
    char *goodsubstr = name;
    char *substr = name;
    do {
	goodsubstr = substr;
	substr = strstr(substr, "/");
	if (substr != NULL) {
	    /* check if we have a trailing slash, and remove it if necessary */
//  	    if ((strlen(substr) == 1) &&
//  		(strncmp(substr, "/", 1) == 0)) {
//  		substr[strlen(substr) - 1] = 0;
//  		break;
//  	    }
	    /* otherwise continue searching */
	    substr++;
	}
    } while (substr != NULL);

#ifdef _DEBUG_FSAL
    fprintf(stderr, "FFSObject::shortName() goodsubstr = %s\n", goodsubstr);
#endif
    return goodsubstr;
}

/* Returns the parent of the current object
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   A pointer to the name of parent of the current object
 */

FFolder* FFSObject::getParent()
{
    return parent;
}
 
/* Returns the owner of the current object
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   An integer value corresponding to the owner of the current object
 */

unsigned int FFSObject::getOwner()
{
    return owner;
}

/* Returns the group of the current object
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   An integer value corresponding to the group of the current object
 */

unsigned int FFSObject::getGroup()
{
    return group;
}
 
/* Returns the modified date of the current object
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   A structure for the modify time & date of the current object
 */

time_t FFSObject::getModify()
{
    return modify;
}

/* Returns the creation date of the current object
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   A structure for the creation time & date of the current object
 */

time_t FFSObject::getCreate()
{
    return create;
}

/* Sets the name of the current object
 *
 * PARAMS
 *   A pointer to the new name of the object
 *
 * RETURN
 *   none
 */

void FFSObject::setName(char *new_name)
{
    name = new_name;
}

/* Gets the next FFSObject 
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   Pointer to FFSObject
 */
FFSObject *FFSObject::getNext(void) {
    return next;
}

/* Adds an object to the linked list.
 *
 * PARAMS
 *   The FFSObject to add to the list
 *
 * RETURN
 *   nothing
 */
void FFSObject::link(FFSObject *new_next) {
    next = new_next;
}

int FFSObject::compareName(FFSObject *ffso) {
    char *name1 = (char *) malloc(sizeof(char) * strlen(name) + 1);
    strcpy(name1, name);

    char *name2 = (char *) malloc(sizeof(char) * strlen(ffso->getName()) + 1);
    strcpy(name2, ffso->getName());

    for (char *p = name1; *p != 0; p++) {
	*p = tolower(*p);
    }
    for (char *q = name2; *q != 0; q++) {
	*q = tolower(*q);
    }
    int cmp = strcmp(name1, name2);

    free(name1);
    free(name2);

    return cmp;
}

/*
 * Compare this objects name with the specified objects name.
 */
int FFSObject::compareAll(FFSObject *ff) {

    char *name1 = (char *) malloc(sizeof(char) * strlen(name) + 1);
    strcpy(name1, name);

    char *name2 = (char *) malloc(sizeof(char) * strlen(ff->getName()) + 1);
    strcpy(name2, ff->getName());

    char *name1_s = (char *) malloc(sizeof(char) * strlen(getShortName()) + 1);
    strcpy(name1_s, getShortName());

    char *name2_s = (char *) malloc(sizeof(char) 
				    * strlen(ff->getShortName()) + 1);
    strcpy(name2_s, ff->getShortName());

    int check1 = Prefs::isChecked(name1);
    int check2 = Prefs::isChecked(name2);

    for (char *p = name1; *p != 0; p++) {
	*p = tolower(*p);
    }
    for (char *q = name2; *q != 0; q++) {
	*q = tolower(*q);
    }
    for (char *p = name1_s; *p != 0; p++) {
	*p = tolower(*p);
    }
    for (char *q = name2_s; *q != 0; q++) {
	*q = tolower(*q);
    }

    int sys1 = Prefs::isSystemName(name1_s);
    int sys2 = Prefs::isSystemName(name2_s);
    int scmp = strcmp(name1, name2);
    time_t mru1 = Prefs::getMRU(name1);
    time_t mru2 = Prefs::getMRU(name2);
    int cmp = (difftime(mru1, mru2) > 0) ? 0 : 1;
    free(name1);
    free(name2);
    free(name1_s);
    free(name2_s);

    if (ff->isParent()) {
	return 1;
    } else if (this->isParent()) {
 	return 0;
    }
    
    if (check1 > check2) {
	return 0;
    } else if (check1 < check2) {
	return 1;
    }
    
    if (sys1 > sys2) {
	return 1;
    } else if (sys1 < sys2) {
	return 0;
    }

    if ((mru1 == 0) && (mru2 == 0)) {
	return scmp;
    }

    return cmp;
}

int FFSObject::isParentOf(char *current, char *parent) {
    if (strcmp(current, parent) <= 0) {
	return 0;
    }

    char *current_local = 
	(char *) malloc((strlen(current) + 4) * sizeof(char));
    sprintf(current_local, "%s/..", current);
//    strcpy(current_local, current);

//    strcat(current_local, "/..");
    stripName(current_local);
    if (strcmp(current_local, parent) == 0) {
	fprintf(stderr, "%s is a parent of %s\n", parent, current);
	free(current_local);
	return 1;
    }
    fprintf(stderr, "%s is NOT a parent of %s (cl: %s)\n", parent, current,
	    current_local);
    free(current_local);
    return 0;
}

void FFSObject::stripName(char *name) {
    char *loc;
    if ((loc = strstr(name, "/.."))) {
	*loc = 0;
	loc--;
	while (*loc != '/') {
	    loc--;
	}
	*loc = 0;
	if (strlen(name) == 0) {
	    *loc = '/';
	    *(loc+1) = 0;
	}
    }
    if (name[0] == '/' && name[1] == '/') {
	strcpy(name, name+1);
    }
}

void FFSObject::isParent(bool isit) {
    parentP = isit;
}

bool FFSObject::isParent(void) {
    return parentP;
}

