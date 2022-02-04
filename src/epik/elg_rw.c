/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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

#include "elg_rw.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define SCALASCA_DEBUG_MODULE_NAME    EPILOG
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include "elg_impl.h"
#include "epk_archive.h"

#if HAVE(LIBZ)
    #include <zlib.h>
#endif    /* HAVE(LIBZ) */


#define ELGIN_MODE_FILE      0
#define ELGIN_MODE_BUFFER    1


/*--- Helper functions ------------------------------------------------------*/

static void
elg_swap(void*  buffer,
         size_t length)
{
    /* length must be an even number */

    char   tmp;
    size_t i;

    if (length > 1)
    {
        for (i = 0; i < (length / 2); i++)
        {
            tmp                             = ((char*)buffer)[i];
            ((char*)buffer)[i]              = ((char*)buffer)[length - 1 - i];
            ((char*)buffer)[length - 1 - i] = tmp;
        }
    }
}


/*--- Macros ----------------------------------------------------------------*/

#define ELGREC_GET_NUM(rec, var)                                                          \
    memcpy(&var, rec->pos, sizeof(var));                                                  \
    rec->pos += sizeof(var);                                                              \
    if (  ((rec->byte_order == ELG_LITTLE_ENDIAN) && (ELG_BYTE_ORDER == ELG_BIG_ENDIAN))  \
       || ((rec->byte_order == ELG_BIG_ENDIAN) && (ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN))) \
    {                                                                                     \
        elg_swap(&var, sizeof(var));                                                      \
    }

/*
 *-----------------------------------------------------------------------------
 * ElgRec
 *-----------------------------------------------------------------------------
 */

/* Data type */
struct ElgRec_struct
{
    elg_ui1  type;
    elg_ui1  length;
    elg_ui1  byte_order;
    buffer_t pos;
    buffer_t body;
};


void
ElgRec_free(ElgRec* rec)
{
    free(rec->body);
    free(rec);
}


/* Reading record header */

elg_ui1
ElgRec_get_type(ElgRec* rec)
{
    return rec->type;
}


int
ElgRec_is_event(ElgRec* rec)
{
    elg_ui1 type = ElgRec_get_type(rec);

    return (type >= 100);
}


int
ElgRec_is_attribute(ElgRec* rec)
{
    elg_ui1 type = ElgRec_get_type(rec);

    return (  (type >= ELG_FIRST_ATTR)
           && (type <= ELG_LAST_ATTR));
}


elg_ui1
ElgRec_get_length(ElgRec* rec)
{
    return rec->length;
}


/* Reading record data */

elg_ui1
ElgRec_read_ui1(ElgRec* rec)
{
    elg_ui1 ui1;

    ELGREC_GET_NUM(rec, ui1);

    return ui1;
}


elg_ui4
ElgRec_read_ui4(ElgRec* rec)
{
    elg_ui4 ui4;

    ELGREC_GET_NUM(rec, ui4);

    return ui4;
}


elg_ui8
ElgRec_read_ui8(ElgRec* rec)
{
    elg_ui8 ui8;

    ELGREC_GET_NUM(rec, ui8);

    return ui8;
}


elg_d8
ElgRec_read_d8(ElgRec* rec)
{
    elg_d8 d8;

    ELGREC_GET_NUM(rec, d8);

    return d8;
}


char*
ElgRec_read_string(ElgRec* rec)
{
    return (char*)rec->pos;
}


/*
 *-----------------------------------------------------------------------------
 * ElgIn
 *-----------------------------------------------------------------------------
 */

/* Data type */

struct ElgIn_struct
{
    int mode;
    #if !HAVE(LIBZ)
        FILE* file;
    #else    /* HAVE(LIBZ) */
        gzFile file;
    #endif    /* HAVE(LIBZ) */
    const elg_ui1* buffer;
    int            sionid;
    long           bytes_avail_in_block;
    size_t         size;
    size_t         offset;
    char           name[PATH_MAX];
    elg_ui1        minor_vnr;
    elg_ui1        major_vnr;
    elg_ui1        byte_order;
};


/* Open and close trace file */

ElgIn*
ElgIn_open(const char* path_name)
{
    ElgIn*      in;
    char        label[7];
    struct stat sbuf;
    int         status;

    /* check file status */
    status = stat(path_name, &sbuf);
    if (status)
    {
        UTILS_DEBUG("stat(%s): %s", path_name, strerror(errno));

        return NULL;
    }
    if (sbuf.st_blocks == 0)
    {
        /* number of 512-byte blocks allocated */
        UTILS_WARNING("%u blocks allocated for %u bytes of %s",
                      sbuf.st_blocks, sbuf.st_size, path_name);
    }

    /* allocate ElgIn record */
    in = (ElgIn*)malloc(sizeof(ElgIn));
    if (in == NULL)
    {
        UTILS_FATAL("Allocation of EPILOG record struct failed");
    }

    /* open file */
    in->mode = ELGIN_MODE_FILE;
    #if !HAVE(LIBZ)
        if ((in->file = fopen(path_name, "r")) == NULL)
    #else    /* HAVE(LIBZ) */
        if ((in->file = gzopen(path_name, "r")) == NULL)
    #endif    /* HAVE(LIBZ) */
    {
        UTILS_WARNING("Cannot open %s file %s", epk_archive_filetype(path_name), path_name);
        free(in);

        return NULL;
    }
    else
    {
        UTILS_DEBUG("Opened %s file %s for reading %d bytes",
                    epk_archive_filetype(path_name), path_name, sbuf.st_size);
    }

    /* store file name */
    strcpy(in->name, path_name);

    #if !HAVE(LIBZ)
        if (  (fread(label, strlen(ELG_HEADER) + 1, 1, in->file) != 1)
           || (fread(&in->major_vnr, sizeof(in->major_vnr), 1, in->file) != 1)
           || (fread(&in->minor_vnr, sizeof(in->minor_vnr), 1, in->file) != 1)
           || (fread(&in->byte_order, sizeof(in->byte_order), 1, in->file) != 1)
           || (strcmp(label, ELG_HEADER) != 0))
        {
            fclose(in->file);
            free(in);

            return NULL;
        }
    #else    /* HAVE(LIBZ) */
        if (  (gzread(in->file, label, strlen(ELG_HEADER) + 1) != (strlen(ELG_HEADER) + 1))
           || (gzread(in->file, &in->major_vnr, sizeof(in->major_vnr)) != sizeof(in->major_vnr))
           || (gzread(in->file, &in->minor_vnr, sizeof(in->minor_vnr)) != sizeof(in->minor_vnr))
           || (gzread(in->file, &in->byte_order, sizeof(in->byte_order)) != sizeof(in->byte_order))
           || (strcmp(label, ELG_HEADER) != 0))
        {
            gzclose(in->file);
            free(in);

            return NULL;
        }
    #endif    /* HAVE(LIBZ) */

    return in;
}


int
ElgIn_close(ElgIn* in)
{
    int result;

    if (in->mode == ELGIN_MODE_FILE)
    {
        #if !HAVE(LIBZ)
            result = fclose(in->file);
        #else    /* HAVE(LIBZ) */
            result = gzclose(in->file);
        #endif    /* HAVE(LIBZ) */
        if (result == 0)
        {
            UTILS_DEBUG("Closed %s file %s",
                        epk_archive_filetype(in->name), in->name);
        }
    }
    else    /* ELGIN_MODE_BUFFER */
    {
        /* UTILS_DEBUG("Closed buffer"); */
        result = 0;
    }

    free(in);

    return result;
}


/* EPILOG version */

elg_ui4
ElgIn_get_version(ElgIn* in)
{
    return ((in->major_vnr * 1000) + in->minor_vnr);
}


/* File position and error management */

int
ElgIn_seek(ElgIn* in,
           long   offset)
{
    if (in->mode == ELGIN_MODE_FILE)
    {
        #if !HAVE(LIBZ)
            return fseek(in->file, offset, SEEK_SET);
        #else    /* HAVE(LIBZ) */
            return gzseek(in->file, offset, SEEK_SET);
        #endif    /* HAVE(LIBZ) */
    }

    /* ELGIN_MODE_BUFFER */
    if (  (offset < 0)
       || ((size_t)offset >= in->size))
    {
        return -1;
    }

    in->offset = offset;

    return 0;
}


/* Reading trace records */

ElgRec*
ElgIn_read_record(ElgIn* in)
{
    ElgRec* rec;

    /* allocate record */
    rec = calloc(1, sizeof(ElgRec));
    if (rec == NULL)
    {
        UTILS_FATAL("Allocation of EPILOG record struct failed");
    }

    if (in->mode == ELGIN_MODE_FILE)
    {
        #if !HAVE(LIBZ)
            if (  (fread(&rec->length, sizeof(rec->length), 1, in->file) != 1)
               || (fread(&rec->type, sizeof(rec->type), 1, in->file) != 1))
        #else    /* HAVE(LIBZ) */
            if (  (gzread(in->file, &rec->length, sizeof(rec->length)) != sizeof(rec->length))
               || (gzread(in->file, &rec->type, sizeof(rec->type)) != sizeof(rec->type)))
        #endif    /* HAVE(LIBZ) */
        {
            free(rec);

            return NULL;
        }
    }
    else    /* ELGIN_MODE_BUFFER */
    {
        if ((in->size - in->offset) < (sizeof(rec->length) + sizeof(rec->type)))
        {
            free(rec);

            return NULL;
        }

        rec->length = in->buffer[in->offset++];
        rec->type   = in->buffer[in->offset++];
    }

    /* allocate record body */
    if (rec->length)
    {
        rec->body = malloc(rec->length);
        if (rec->body == NULL)
        {
            UTILS_FATAL("Allocation of EPILOG record body failed");
        }

        /* read record body */
        if (in->mode == ELGIN_MODE_FILE)
        {
            #if !HAVE(LIBZ)
                if (fread(rec->body, rec->length, 1, in->file) != 1)
            #else    /* HAVE(LIBZ) */
                if (gzread(in->file, rec->body, rec->length) != rec->length)
            #endif    /* HAVE(LIBZ) */
            {
                ElgRec_free(rec);

                return NULL;
            }
        }
        else    /* ELGIN_MODE_BUFFER */
        {
            if ((in->size - in->offset) < rec->length)
            {
                ElgRec_free(rec);

                return NULL;
            }

            memcpy(rec->body, &in->buffer[in->offset], rec->length);
            in->offset += rec->length;
        }
    }
    else
    {
        rec->body = NULL;
    }

    rec->byte_order = in->byte_order;
    rec->pos        = rec->body;

    return rec;
}
