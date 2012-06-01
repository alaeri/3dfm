/*
 * 3dfm - 3D File Manager
 * $Id: FFSObject.h,v 1.7 2003/04/04 03:49:33 adam Exp $
 */

#ifndef _FFSOBJECT_H_
#define _FFSOBJECT_H_

#include <time.h>
#include <sys/stat.h>

class FFolder;

/*
 * FFSObject
 *
 * This object defines the basic properties of file system objects.
 */

class FFSObject {
 protected:
    char* name;
    FFolder *parent;
    bool parentP;
    int owner;
    int group;
    time_t modify;
    time_t create;

    FFSObject *next;
    void init(FFolder *new_parent, struct stat *new_stat,
	      char *new_name);

 public:
    FFSObject(void);
    FFSObject(FFolder *new_parent, struct stat *new_stat, char *new_name);
    virtual ~FFSObject(void);
    char *getName(void);
    char *getShortName(void);
    FFolder *getParent(void);
    unsigned int getOwner(void);
    unsigned int getGroup(void);
    time_t getModify(void);
    time_t getCreate(void);
    void setName(char *new_name);
    FFSObject *getNext(void);
    virtual void link(FFSObject *new_next);
    virtual int compareAll(FFSObject *ffso);
    virtual int compareName(FFSObject *ffso);
    static void stripName(char *name);  /* remove '../' from the path name */
    static int isParentOf(char *current, char *parent);
    virtual void isParent(bool isit);
    virtual bool isParent(void);
    
};

#endif
