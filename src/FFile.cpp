/*
 * 3dfm - 3D File Manager
 * $Id: FFile.cpp,v 1.4 2002/12/12 16:01:51 adam Exp $
 */

#include "FFile.h"

FFile::FFile(FFolder *new_parent, struct stat *new_stat, char *new_name) {
    init(new_parent, new_stat, new_name);
    size = new_stat->st_size;
}

/* Returns size of the file.
 *
 * PARAMS
 *   none
 *
 * RETURN
 *   an unsigned long long int containing the file size in bytes
 */

unsigned long long int FFile::getSize(void) 
{
    return size;
}
