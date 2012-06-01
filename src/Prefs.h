/*
 * 3dfm - 3D File Manager
 * $Id: Prefs.h,v 1.22 2003/04/22 04:17:25 adam Exp $
 */

/*
 * Prefs
 *
 * This object manages the preferes for 3dfm
 */

#ifndef _PREFS_H_
#define _PREFS_H_

/* 3dfm includes */
#include <GColor.h>
#include <time.h>


/*
 * Structure to create a list of character strings
 * for the system folder list.
 */

typedef struct t_sysnode {
    char *name;			/* (short) name of the folder */
    struct t_sysnode *next;	/* next item in the list */
} s_sysnode;

/*
 * Structure to create a list of character strings
 * for the checked folder list.
 */
typedef struct t_checknode {
    char *name;			/* full path of the folder */
    struct t_checknode *next;	/* next item in the list */
} s_checknode;

/*
 * Structure to create a list of character strings for the MRU list.
 */
typedef struct t_mrunode {
    char *name;                 /* full path of the folder */
    time_t last_use;            /* timestamp of the last folder use */
    struct t_mrunode *next;     /* pointer to the next item in the list */
} s_mrunode;

class Prefs {
 public:

    /*** Various colors ***/
    static GColor folderColor;      /* outer color of any folder not covered 
				       by one of the categories below */
    static GColor innerColor;       /* inner color of *all* folders */
    static GColor hiddenColor;      /* outer color of hidden folders (those 
				       whose short names start with a '.' */
    static GColor parentColor;      /* outer color of the parent folder */
    static GColor linkColor;        /* outer color of linked folders */
    static GColor systemColor;
    static GColor backgroundColor;  /* color of the window background */
    static GColor filelistboxColor;
    static GColor folderlistboxColor;
    static GColor statusbarColor;
    static GColor statusbartextColor;

    /*** various integer preferences ***/
    static int showHidden;         /* do we show hidden files in the tree?  
				      They are always shown in the folder 
				      list */
    static int depth;              /* maximum depth to show in the list (and 
				      traverse when reading in the file 
				      system */
    static long long int breadth;  /* maximum breadth to show in the list (all 
				      the children are always read into the 
				      FSAL) */
    static int mru_max;            /* maximum number of mru items to display */
    static float child_rotate;
    static int showLabels;
    static int showLinks;
    static int showSystem;
    static int showOnlyChecked;
    static int strokeFont;

    /*** system file handling ***/
    static s_sysnode *syshead;            /* the system folder linked-list */
    static void readSys();              /* reads the system data
					     from the data file */
    static int isSystemName(char *name);  /* checks a folder name to see if
					     it is on the system list */
    
    /*** checkmark handling ***/
    static s_checknode *chead;            /* list of checked folders */
    static int isChecked(char *name);     /* is a folder on the list? */
    static void addCheck(char *name);     /* add a folder to the list */
    static void removeCheck(char *name);  /* remove a folder from the list */
    static void readChecked();            /* read checked folders from the
					     data file */
    static void writeChecked();           /* write checked folders to the data
					     file */

    /*** MRU handling ***/
    static s_mrunode *mruhead;            /* the MRU list */
    static void setMRU(char *name);       /* sets the access time on a folder,
					     and adds it to the list of MRUs
					     if it is not already there */
    static time_t getMRU(char *name);     /* gets the access time on a folder,
					     or returns 0 if there is none
					     (i.e. first access) */
    static void removeMRU(char *name);    /* removes the MRU from the list */
    static void writeMRU(void);           /* writes the MRU list to the data
					     file */
    static void readMRU(void);            /* reads the MRU list from the data
					     file */

    /*** General Prefs. handling ***/
    static void writePrefs(void);
    static void readPrefs(void);
    
    /* Debugging functions!  DO NOT DOCUMENT! */
    static void dumpChecked(void);

};

#endif
