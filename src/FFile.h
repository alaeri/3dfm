/*
 * 3dfm - 3D File Manager
 * $Id: FFile.h,v 1.2 2002/12/12 16:02:40 adam Exp $
 */

#ifndef _FFILE_H_
#define _FFILE_H_

#define FFILE 3

#include "FFSObject.h"

/*
 * FFile
 *
 * This object defines a file
 */

class FFile : public FFSObject {
 private:
    unsigned long long int size;
 public:
    FFile(FFolder *new_parent, struct stat *new_stat, char *new_name);

    unsigned long long int getSize(void);
};

#endif
