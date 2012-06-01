/*
 * 3dfm - 3D File Manager
 * $Id: Prefs.cpp,v 1.28 2003/04/22 04:17:58 adam Exp $
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* system includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* 3dfm includes */
#include <Prefs.h>
#include <FFolder.h>

/* prefs constants */
int Prefs::depth = 2;
long long int Prefs::breadth = 100;
int Prefs::showHidden = 1;
int Prefs::mru_max = 5;
float Prefs::child_rotate = 0.0;
int Prefs::showLabels = 1;
int Prefs::showLinks = 1;
int Prefs::showSystem = 1;
int Prefs::showOnlyChecked = 0;
int Prefs::strokeFont = 0;

/* linked lists start empty */
s_sysnode *Prefs::syshead = NULL;
s_checknode *Prefs::chead = NULL;
s_mrunode *Prefs::mruhead = NULL;

/* default colors */
GColor Prefs::innerColor = GColor(0.734375, 0.539062, 0.25);
GColor Prefs::folderColor = GColor(0.539062, 0.25, 0.734375);
GColor Prefs::hiddenColor = GColor(0.0, 1.0, 0.0);
GColor Prefs::parentColor = GColor(1.0, 0.0, 0.0);
GColor Prefs::linkColor = GColor(1.0, 1.0, 0.0);
GColor Prefs::systemColor = GColor(0.0, 1.0, 1.0);

GColor Prefs::backgroundColor = GColor(0.0, 0.2, 0.2);
GColor Prefs::filelistboxColor = GColor(1.0, 0.2, 0.0);
GColor Prefs::folderlistboxColor = GColor(0.2, 0.1, 0.8);
GColor Prefs::statusbarColor = GColor(0.7, 0.5, 0.7);
GColor Prefs::statusbartextColor = GColor(1.0, 1.0, 0.0);


/* Opens the file that reads in the list of system defined folders and places
 * them into a linked list.
 *
 * PARAMS
 *   none
 *
 */
void Prefs::readSys(void) {
    //Open the file in location:/tmp/3dfm_sys in read-only mode
    FILE *in = fopen("/tmp/3dfm_sys", "r");
//    char *line;
    char line[200];
    size_t size;
    s_sysnode *temp;

    //Check for empty file
    if (in == NULL) {
	return;
    }
    //Read through until we reach the end of the file
    while (!feof(in)) {
//	line = NULL;
       
	/* get the next line in the file */
//	getline(&line, &size, in);
	fscanf(in, "%200s", &line);

	//Remove the new-line character, because we don't need it in the list
//	line[strlen(line) - 1] = 0;

	/*
	 *Comments in the data file are signaled by a # at the beginning of the
	 *line.  If the line starts with a # we don't read it.
	 */
	if (line[0] != '#') {
	    //If the list is empty create a head node and initialize the list
	    if (syshead == NULL) {
		syshead = (s_sysnode *) malloc(sizeof(s_sysnode));
		syshead->next = NULL;
		syshead->name = (char *) malloc(sizeof(char) * strlen(line));
		strcpy(syshead->name, line);
	    } else {
	        //Otherwise add to the list
		temp = syshead;
		syshead = (s_sysnode *) malloc(sizeof(s_sysnode));
		syshead->next = temp;
		syshead->name = (char *) malloc(sizeof(char) * strlen(line));
		strcpy(syshead->name, line);
	    }
	}

	/* dump this dude */
// 	if (line) {
// 	    free(line);
// 	}
    }

    //Close the file
    fclose(in);
}

/* Opens the file that reads in the list of checked folders and places
 * them into a linked list.
 *
 * PARAMS
 *   none
 *
 */
void Prefs::readChecked(void) {
    //Open the file in location: /tmp/3dfm_check in read-only mode
    FILE *in = fopen("/tmp/3dfm_check", "r");
//    char *line;
    char line[200];
    size_t size;
    s_checknode *temp;

    //Look for an empty file if so return
    if (in == NULL) {
	return;
    }
    //Read until the end of the file
    while (!feof(in)) {
//	line = NULL;
	/* get the next line in the file */
//	getline(&line, &size, in);
	fscanf(in, "%200s", line);
//	line[strlen(line) - 1] = 0;

	//Check that the line is not a commented line
	if (line[0] != '#') {
	    if (chead == NULL) {
	        //If the list is empty initialize the list and the first item
		chead = (s_checknode *) malloc(sizeof(s_checknode));
		chead->next = NULL;
		chead->name = (char *) malloc(sizeof(char) * strlen(line) + 1);
		strcpy(chead->name, line);
	    } else {
	        //Add the next item to the end of the list
		temp = chead;
		chead = (s_checknode *) malloc(sizeof(s_checknode));
		chead->next = temp;
		chead->name = (char *) malloc(sizeof(char) * strlen(line) + 1);
		strcpy(chead->name, line);
	    }
	} else if (line[1] == '#') {
	    break;
	}

	/* dump this dude */
// 	if (line) {
// 	    free(line);
// 	}
    }

    //Close the file
    fclose(in);
}

/* Opens the file that reads in the list of checked folders and places
 * them into a linked list.
 *
 * PARAMS
 *   none
 *
 */
void Prefs::readMRU(void) {
    //Open the file in location: /tmp/3dfm_check in read-only mode
    FILE *in = fopen("/tmp/3dfm_mru", "r");
//    char *line;
    char line[200];
    size_t size;
    time_t mru;
    s_mrunode *temp;

    //Look for an empty file if so return
    if (in == NULL) {
	return;
    }
    //Read until the end of the file
    while (!feof(in)) {
//	line = NULL;
	/* get the next line in the file */
	fscanf(in, "%200s %ld", &line, &mru);
//	getline(&line, &size, in);
//	line[strlen(line) - 1] = 0;

	if (strlen(line) < 2) {
	    break;
	}
	if ((line[0] == '#') && (line[1] == '#')) {
	    break;
	}
	if (line[0] == '#') {
	    continue;
	}

// 	/* count the number of characters in the file path */
// 	char *p = line;
// 	int path_len = 0;
// 	while ((p != NULL) && (*p != ' ')) {
// 	    ++path_len;
// 	    p++;
// 	}
// 	p++;

	int path_len = strlen(line);

// 	/* check that something isn't fishy here */
// 	if (p == NULL) {
// 	    fprintf(stderr, "Error reading MRU file. Ignoring stored MRU from "
// 		    "this point.\n");
// 	    fclose(in);
// 	    return;
// 	}

// 	/* read in the time as a long int */
// 	sscanf(p, "%ld", &mru);

	/* allocate and store the characters of the path name */
	char *path = (char *) malloc(sizeof(char) * path_len + 1);
	strncpy(path, line, path_len);
	path[path_len] = 0;

#ifdef _DEBUG_MRU
	fprintf(stderr, "readMRU path = %s mru = %s\n", path, ctime(&mru));
#endif
	if (mruhead == NULL) {
	    //If the list is empty initialize the list and the first item
	    mruhead = (s_mrunode *) malloc(sizeof(s_mrunode));
	    mruhead->next = NULL;
	    mruhead->name = path;
	    mruhead->last_use = mru;
	} else {
	    //Add the next item to the end of the list
	    temp = mruhead;
	    mruhead = (s_mrunode *) malloc(sizeof(s_mrunode));
	    mruhead->next = temp;
		mruhead->name = path;
		mruhead->last_use = mru;
	}

	/* dump this dude */
// 	if (line) {
// 	    free(line);
// 	}
    }

    //Close the file
    fclose(in);
    
}

/* Opens the and writes the list of checked folders from the linked list
 *
 * PARAMS
 *   none
 *
 */
void Prefs::writeChecked(void) {
    /*
     * Open the file from location: /tmp/3dfm_check in write only mode.
     * Note: This mode erases everything in the current file so now we
     *       do not have to check if we removed any items because we are always
     *       writting back the most updated list.
     */
    FILE *out = fopen("/tmp/3dfm_check", "w");

    if (out == NULL) {
	return;
    }

    //Set the current pointer to the head of the list
    s_checknode *curr = chead;
    //Go through the list until we get to the end
    while (curr != NULL) {

	/*
	 * Print the name into the list, and add a new-line because they
	 * are not stored in the linked list.
	 */
	fprintf(out, "%s\n", curr->name);
	curr = curr->next;
    }
    /*
     * Complete this file with ## so that next we read it in we do not
     * get any garbage and know when the file as ended
     */
    fprintf(out, "##\n");

    //Close the file
    fclose(out);
}

void Prefs::writePrefs(void) {
    FILE *out = fopen("/tmp/3dfm_prefs", "w");
    if (out == NULL) {
	return;
    }
    fprintf(out, "%lld\n", breadth);
    fprintf(out, "%d\n", depth);
    fprintf(out, "%d\n", showHidden);
    fprintf(out, "%d\n", showLabels);
    fprintf(out, "%d\n", showLinks);
    fprintf(out, "%d\n", showSystem);
    fprintf(out, "%d\n", showOnlyChecked);
    fprintf(out, "%f %f %f\n", folderColor.getRed(),
	    folderColor.getGreen(), folderColor.getBlue());
    fprintf(out, "%f %f %f\n", innerColor.getRed(),
	    innerColor.getGreen(), innerColor.getBlue());
    fprintf(out, "%f %f %f\n", hiddenColor.getRed(),
	    hiddenColor.getGreen(), hiddenColor.getBlue());
    fprintf(out, "%f %f %f\n", parentColor.getRed(),
	    parentColor.getGreen(), parentColor.getBlue());
    fprintf(out, "%f %f %f\n", linkColor.getRed(),
	    linkColor.getGreen(), linkColor.getBlue());
    fprintf(out, "%f %f %f\n", systemColor.getRed(),
	    systemColor.getGreen(), systemColor.getBlue());
    fprintf(out, "%f %f %f\n", backgroundColor.getRed(),
	    backgroundColor.getGreen(), backgroundColor.getBlue());
    fprintf(out, "%f %f %f\n", filelistboxColor.getRed(),
	    filelistboxColor.getGreen(), filelistboxColor.getBlue());
    fprintf(out, "%f %f %f\n", folderlistboxColor.getRed(),
	    folderlistboxColor.getGreen(), folderlistboxColor.getBlue());
    fprintf(out, "%f %f %f\n", statusbarColor.getRed(),
	    statusbarColor.getGreen(), statusbarColor.getBlue());
    fprintf(out, "%f %f %f\n", statusbartextColor.getRed(),
	    statusbartextColor.getGreen(), statusbartextColor.getBlue());
    fclose(out);
}

void Prefs::readPrefs(void) {
    /* colors read from file */
    float red;
    float green;
    float blue;

    /* open the file */
    FILE *in = fopen("/tmp/3dfm_prefs", "r");
    if (in == NULL) {
	return;
    }

    /* read prefs */
    fscanf(in, "%lld\n", &breadth);
    fscanf(in, "%d\n", &depth);
    fscanf(in, "%d\n", &showHidden);
    fscanf(in, "%d\n", &showLabels);
    fscanf(in, "%d\n", &showLinks);
    fscanf(in, "%d\n", &showSystem);
    fscanf(in, "%d\n", &showOnlyChecked);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    folderColor = GColor(red, green, blue); 
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    innerColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    hiddenColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    parentColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    linkColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    systemColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    backgroundColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    filelistboxColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    folderlistboxColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    statusbarColor = GColor(red, green, blue);
    fscanf(in, "%f %f %f\n", &red, &green, &blue);
    statusbartextColor = GColor(red, green, blue);
    fclose(in);
}

/* Opens the and writes the list of checked folders from the linked list
 *
 * PARAMS
 *   none
 *
 */
void Prefs::writeMRU(void) {
    /*
     * Open the file from location: /tmp/3dfm_check in write only mode.
     * Note: This mode erases everything in the current file so now we
     *       do not have to check if we removed any items because we are always
     *       writting back the most updated list.
     */
    FILE *out = fopen("/tmp/3dfm_mru", "w");

    if (out == NULL) {
	return;
    }

    //Set the current pointer to the head of the list
    s_mrunode *curr = mruhead;
    //Go through the list until we get to the end
    while (curr != NULL) {

	/*
	 * Print the name into the list, and add a new-line because they
	 * are not stored in the linked list.
	 */
#ifdef _DEBUG_MRU
	fprintf(stderr, "writeMRU path = %s mru = %s\n", curr->name, 
		ctime(&(curr->last_use)));
#endif
	fprintf(out, "%s %d\n", curr->name, (int) curr->last_use);
	curr = curr->next;
    }
    /*
     * Complete this file with ## so that next we read it in we do not
     * get any garbage and know when the file as ended
     */
    fprintf(out, "##\n");

    //Close the file
    fclose(out);
}

/* Check the inputed string against the list of system folders, and returns
 * a one if it is a system folder and a zero otherwise.
 *
 * PARAMS
 *   *name - the name of the folder to be compared
 *
 */
int Prefs::isSystemName(char *name) {
    //Set the current pointer to the head of the list
    s_sysnode *curr = syshead;
    //Traverse until we reach the end of the list
    while (curr != NULL) {
	//If it does match an entry in the list return a one
	if (strcmp(curr->name, name) == 0) {
	    return 1;
	}
	//Go to next item if no hit yet
	curr = curr->next;
    }
    //No hits in the list; return zero
    return 0;
}

/* Check the inputed string against the list of checked folders, and returns
 * a one if it is a system folder and a zero otherwise.
 *
 * PARAMS
 *   *name - the name of the folder to be compared
 *
 */
int Prefs::isChecked(char *name) {
    //Set the current pointer to the head of the list
    s_checknode *curr = chead;
    //Traverse until we reach the end of the list
    while (curr != NULL) {
        //If it does match an entry in the list return a one
	if (strcmp(curr->name, name) == 0) {
	    return 1;
	}
	//Go to the next item if no hit yet
	curr = curr->next;
    }
    //No hits in the list; return zero
    return 0;
}

/* Add the passed in folder name to the list of check items
 *
 * PARAMS
 *   *name - the name of the folder to be added
 *
 */
void Prefs::addCheck(char *name) {
    /* first check to see if it exists */
    if (isChecked(name)) {
	return;
    }

    //Set the current pointer to the head of the list
    s_checknode *temp = chead;
    //Create a new node object
    chead = (s_checknode *) malloc(sizeof(s_checknode));
    //Create the location for the new object
    chead->next = temp;
    /*
     * Create a node with the size of the name, and dont forget the room
     * for the new line character.
     */
    chead->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
    //Copy the new object into the list
    strcpy(chead->name, name);
}

/* Remove the passed in folder name from the list of checked item
 *
 * PARAMS
 *   *name - the name of the folder to beremoved
 *
 */
void Prefs::removeCheck(char *name) {
    //Set the current pointer to the beginning of the list
    s_checknode *curr = chead;
    //Set previous and temporary folders to null to start
    s_checknode *prev = NULL;
    s_checknode *temp = NULL;
    //Traverse until we reach the end of the list
    while (curr != NULL) {
	//Check for a match...
	if (strcmp(curr->name, name) == 0) {
	    if (prev == NULL) {
	    	/*
		 * If it is the first item in the list remove it and set
		 * previous to null.
		 */
		chead = temp = curr->next;
	    } else {
	        //Otherwise add it to the middle or end of the list
		prev->next = temp = curr->next;
	    }
	    //Free up memory
	    free(curr);
	    curr = temp;
	    continue;
	}
	//Traverse through the list
	prev = curr;
	curr = curr->next;
    }
}

void Prefs::removeMRU(char *name) {
    s_mrunode *curr = mruhead;
    s_mrunode *prev = NULL;
    s_mrunode *temp = NULL;

    while (curr != NULL) {
	if (strcmp(curr->name, name) == 0) {
	    if (prev == NULL) {
		mruhead = temp = curr->next;
	    } else {
		prev->next = temp = curr->next;
	    }
	    free(curr);
	    curr = temp;
	    continue;
	}
	prev = curr;
	curr = curr->next;
    }
}

/* Prints out the entire checked list
 * NOTE:  This is a debugging program ONLY and may be removed later!
 *        This functions DOES NOT need to be documented!
 *
 * PARAMS
 *   none
 *
 */
void Prefs::dumpChecked(void) {
    s_checknode *curr = chead;
#ifdef _DEBUG_FSAL
    fprintf(stderr, "Checked: ");
#endif
    while (curr != NULL) {
#ifdef _DEBUG_FSAL
	fprintf(stderr, "%s->", curr->name);
#endif
	curr = curr->next;
    }
#ifdef _DEBUG_FSAL
    fprintf(stderr, "EOL\n");
#endif
}

/* Specifies a folder as the most recently used.
 */
void Prefs::setMRU(char *name) {
    s_mrunode *curr = mruhead;
    while (curr != NULL) {
	if (strcmp(name, curr->name) == 0) {
	    break;
	}
	curr = curr->next;
    }

    /* if curr is NULL we are not in the list .., add new entry */
    if (curr == NULL) {
	//Set the current pointer to the head of the list
	s_mrunode *temp = mruhead;
	//Create a new node object
	mruhead = (s_mrunode *) malloc(sizeof(s_mrunode));
	//Create the location for the new object
	mruhead->next = temp;
	/*
	 * Create a node with the size of the name, and dont forget the room
	 * for the new line character.
	 */
	mruhead->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
	//Copy the new object into the list
	strcpy(mruhead->name, name);
	/* add the time */
	mruhead->last_use = time(NULL);
    } else {
	curr->last_use = time(NULL);
    }
}

/* Gets the MRU for a folder */
time_t Prefs::getMRU(char *name) {
    s_mrunode *curr = mruhead;
    while (curr != NULL) {
	if (strcmp(name, curr->name) == 0) {
	    return curr->last_use;
	}
	curr = curr->next;
    }

    return (time_t) 0;
}
