/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/**
 * \file cubew_tar_writing.h
 * \brief Provides a file name transformation scheme: name -> tar file + shift. Produces tared cube report.
 *
 *
 *
 */

#ifndef __CUBEW_TAR_WRITING_H
#define __CUBEW_TAR_WRITING_H 0

#include "cubew_types.h"

#define TAR_BLOCKSIZE 512




typedef struct tar_gnu_header
{
    char name[ 100 ];
    char mode[ 8 ];
    char uid[ 8 ];
    char gid[ 8 ];
    char size[ 12 ];
    char mtime[ 12 ];
    char checksum[ 8 ];
    char typeflag[ 1 ];
    char linkname[ 100 ];
    char magic[ 6 ];
    char version[ 2 ];
    char uname[ 32 ];
    char gname[ 32 ];
    char devmajor[ 8 ];
    char devminor[ 8 ];
    char atime[ 12 ];
    char ctime[ 12 ];
    char offset[ 12 ];
    char longnames[ 4 ];
    char unused[ 1 ];
    struct
    {
        char offset[ 12 ];
        char numbytes[ 12 ];
    } sparse[ 4 ];
    char isextended[ 1 ];
    char realsize[ 12 ];
    char pad[ 17 ];
} /* __attribute__ ((__packed__))*/ tar_gnu_header;


typedef struct tar_empty_block
{
    char block[ TAR_BLOCKSIZE ];
} tar_empty_block;





typedef struct tar_writer_t
{
    char*               actual_tar_file;
    char*               cubename;
    char*               username;
    char*               group;
    char*               mode;
    unsigned            uid;
    unsigned            gid;
    FILE*               tar;
    enum bool_t         anchor_writing;
    cube_metric*        actual_metric;
    tar_gnu_header*     actual_tar_header;
    uint64_t            header_position;
    uint64_t            file_start_position;
    enum CubeFlavours_t cube_flavour;
} __attribute__ ( ( __packed__ ) ) tar_writer_t;



// typedef  tar_writer_t report_layout_writer;



// char * cube_get_anchor_file_name( char * cubename);
// char * cube_get_metric_data_file( char * cubename,  cube_metric * met);
// char * cube_get_metric_index_file( char * cubename,  cube_metric * met);

// void cube_report_finish();
FILE*
cube_report_anchor_start( report_layout_writer* rlw );
FILE*
cube_report_metric_data_start( report_layout_writer* rlw,
                               cube_metric*          met );
FILE*
cube_report_metric_index_start( report_layout_writer* rlw,
                                cube_metric*          met );
FILE*
cube_report_misc_data_start( report_layout_writer* rlw,
                             const char*           dataname );


void
cube_report_anchor_finish( report_layout_writer* rlw,
                           FILE*                 file );
void
cube_report_metric_data_finish( report_layout_writer* rlw,
                                cube_metric*          met,
                                FILE*                 file );
void
cube_report_misc_data_finish( report_layout_writer* rlw,
                              FILE*                 file );
void
cube_report_metric_data_weak_finish( report_layout_writer* rlw,
                                     cube_metric*          met,
                                     FILE*                 file );
void
cube_report_metric_index_finish( report_layout_writer* rlw,
                                 cube_metric*          met,
                                 FILE*                 file );





report_layout_writer*
cube_writing_start( char*               cubename,
                    enum CubeFlavours_t cf );
report_layout_writer*
cube_writing_end( report_layout_writer* rlw );

void
cube_report_write_metric_row( report_layout_writer* rlw,
                              cube_metric*          met );


#endif
