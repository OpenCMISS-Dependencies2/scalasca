/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "epk_archive.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SCALASCA_DEBUG_MODULE_NAME    EPIK
#include <UTILS_CStr.h>
#include <UTILS_Debug.h>
#include <UTILS_Error.h>


#define EPK_NAME          "epik"
#define EPK_DIR_PREFIX    "epik_"


/*
 *-----------------------------------------------------------------------------
 * Helper functions
 *-----------------------------------------------------------------------------
 */

typedef struct
{
    epk_type_t type;
    char       name[5];
    char       suffix[5];
    char       description[13];
} epk_t;

/* the index/order of types in the table must match enum epk_type_t */

static epk_t epk_types[] = {
    { EPK_TYPE_DIR,  "",     "",     "measurement"  },
    { EPK_TYPE_ESD,  "ESD",  "esd",  "definitions"  },
    { EPK_TYPE_ETM,  "ETM",  "etm",  "profile sum"  },
    { EPK_TYPE_ELG,  "ELG",  "elg",  "event trace"  },
    { EPK_TYPE_EMP,  "EMP",  "map",  "id mappings"  },
    { EPK_TYPE_OTF,  "OTF",  "otf",  "OTF trace"    },
    { EPK_TYPE_CUBE, "CUBE", "cube", "CUBE summary" },
    { EPK_TYPE_SION, "SION", "sion", "SION traces"  },
    { EPK_TYPE_MAX,  "",     "",     "NOT DEFINED"  },
};

static char epk_archive_dir[PATH_MAX] = { '\0' };

char*
epk_archive_get_name(void)
{
    UTILS_BUG_ON(epk_archive_dir[0] == '\0', "epk_archive_dir unset!");

    return epk_archive_dir;
}


char*
epk_archive_set_name(const char* name)
{
    UTILS_ASSERT(name && name[0]);

    char* epik_dir = epk_archive_filename(EPK_TYPE_DIR, name);
    if (strlen(epik_dir))
    {
        strncpy(epk_archive_dir, epik_dir, PATH_MAX);
        UTILS_DEBUG("Archive directory set to %s", epk_archive_dir);
    }
    else
    {
        UTILS_WARNING("Archive undefined for \"%s\"", name);

        return NULL;    /* invalid EPIK directory name */
    }

    return epk_archive_dir;
}


/* determine whether provided pathname is to an EPIK archive directory */
char*
epk_archive_is_epik(const char* path)
{
    int   is_epik;
    char* ch;
    char* begch = (char*)path;
    if (!path)
    {
        return NULL;
    }
    while ((ch = strstr(begch, "/" EPK_DIR_PREFIX)) != NULL)
    {
        begch = ch + 1;
    }
    is_epik = (  (begch != path)
              || (strncmp(path, EPK_DIR_PREFIX, strlen(EPK_DIR_PREFIX)) == 0));
    if (is_epik)
    {
        return begch;
    }
    else
    {
        return NULL;
    }
}


char*
epk_archive_directory(const epk_type_t type)
{
    switch (type)
    {
        case EPK_TYPE_DIR:
            return epk_archive_get_name();

        case EPK_TYPE_ESD:
        case EPK_TYPE_ELG:
        case EPK_TYPE_EMP:
        case EPK_TYPE_OTF:
        case EPK_TYPE_CUBE:
            if (epk_archive_dir[0] != '\0')
            {
                char* epik_name = (char*)calloc(strlen(epk_archive_get_name()) + 6, sizeof(char));
                sprintf(epik_name, "%s/%s", epk_archive_get_name(), epk_types[type].name);

                return epik_name;
            }
            else
            {
                return UTILS_CStr_dup(".");
            }

        default:
            return NULL;
    }
}


typedef struct
{
    char* enddir;    /* pointer to end of name of root directory (with expt) */
    char* begnam;    /* pointer to beginning of experiment name */
    char* endnam;    /* pointer to end of experiment name */
} splits_t;


int
epk_archive_path_tok(const char* path,
                     splits_t*   splits)
{
    splits->enddir = epk_archive_is_epik(path);
    if (splits->enddir)
    {
        splits->begnam = splits->enddir + strlen(EPK_DIR_PREFIX);
        splits->endnam = strchr(splits->begnam, '/');
        if (!splits->endnam)
        {
            splits->endnam = (char*)path + strlen(path);
        }
    }
    else
    {
        splits->begnam = splits->endnam = NULL;
    }

    return (splits->enddir != NULL);    /* is_epik or not */
}


char*
epk_archive_filename(const epk_type_t type,
                     const char*      name)
{
    splits_t splits;
    int      is_epik   = epk_archive_path_tok(name, &splits);
    char*    epik_name = (char*)calloc(strlen(name) + 11, sizeof(char));

    switch (type)
    {
        case EPK_TYPE_DIR:
            if (is_epik)
            {
                /* extract directory name from path name */
                strncpy(epik_name, name, splits.endnam - name);
            }
            break;

        case EPK_TYPE_ESD:
        case EPK_TYPE_EMP:
        case EPK_TYPE_ELG:
        case EPK_TYPE_CUBE:
        case EPK_TYPE_SION:
            if (is_epik)
            {
                sprintf(epik_name, "%s/%s.%s", name, EPK_NAME, epk_types[type].suffix);
            }
            else
            {
                sprintf(epik_name, "%s.%s", name, epk_types[type].suffix);
            }
            break;

        default:
            /* EPIK name undefined */
            break;
    }

    return epik_name;
}


char*
epk_archive_rankname(const epk_type_t type,
                     const char*      name,
                     const unsigned   rank)
{
    int   is_epik   = (epk_archive_is_epik(name) != NULL);
    char* epik_name = (char*)calloc(strlen(name) + 12, sizeof(char));

    switch (type)
    {
        case EPK_TYPE_ESD:
        case EPK_TYPE_ELG:
            if (is_epik)
            {
                sprintf(epik_name, "%s/%s/%05u", name, epk_types[type].name, rank);
            }
            else
            {
                sprintf(epik_name, "%s.%u.%s", name, rank, epk_types[type].suffix);
            }
            break;

        case EPK_TYPE_DIR:
        default:
            /* EPIK name undefined */
            break;
    }

    return epik_name;
}


char*
epk_archive_filetype(const char* filename)
{
    int         len;
    const char* ext;
    unsigned    type = 0;
    if (!filename || ((len = strlen(filename)) < 5))
    {
        return epk_types[EPK_TYPE_MAX].description;
    }
    ext = filename + (len - 3);
    do
    {
        type++;
        if (  strstr(filename, epk_types[type].name)
           || (strcmp(epk_types[type].suffix, ext) == 0))
        {
            break;    /* got a match */
        }
    }
    while (type < EPK_TYPE_MAX);

    return epk_types[type].description;
}


int
epk_archive_exists(const char* dir)
{
    struct stat statbuf;
    int         is_dir = (  (stat(dir ? dir : epk_archive_get_name(), &statbuf) == 0)
                         && S_ISDIR(statbuf.st_mode));

    return is_dir;
}


int
epk_archive_locked(void)
{
    char epik_lock[PATH_MAX];
    snprintf(epik_lock, PATH_MAX, "%s/%s", epk_archive_get_name(), "epik.lock");
    int locked = (access(epik_lock, F_OK) == 0);
    UTILS_DEBUG("%s=%d", epik_lock, locked);

    return locked;
}
