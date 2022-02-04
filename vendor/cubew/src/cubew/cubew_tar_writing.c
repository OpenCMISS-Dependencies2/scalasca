/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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
 * \file cubew_tar_writing.c
 * \brief Provides a file name transformation scheme: name -> tar file + shift. Produces tared cube report.
 *
 *
 *
 */

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include "config.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>

#include "cubew_memory.h"
#include "cubew_report_layouts.h"
#include "cubew_tar_writing.h"
#include "cubew_meta_data_writer.h"
#include "cubew_compat_platform.h"

#include "UTILS_Error.h"

#define MEMORY_TRACING_PREFIX "[TAR WRITER]"



static
char*
__cube_get_tared_cube_name( char* cubename )
{
    char* buff = ( char* )CUBEW_CALLOC( 1, strlen( cubename ) + 7, MEMORY_TRACING_PREFIX "Allocate cubename" );
    strcat( buff, cubename );
    strcat( buff, ".cubex" );
    return buff;
}



static
void
__cube_calculate_checksum( tar_gnu_header* header )
{
    /*  set checksum fiels on "         "*/
    char* pre_checksum = "        ";
    memcpy( header->checksum, pre_checksum,  strlen( pre_checksum ) );

    uint8_t*      pos      = ( uint8_t* )header;
    unsigned      i        = 0;
    unsigned long checksum = 0;
    for ( i = 0; i < ( sizeof( tar_gnu_header ) ); i++ )
    {
        checksum += ( unsigned long long )( *pos );
        pos++;
    }

    snprintf( header->checksum, 8, "%6.6lo", ( unsigned long )checksum );
}


static
void
__cube_set_size_and_calculate_checksum( tar_gnu_header* header, uint64_t size )
{
    /*  set checksum fiels on "         "*/
    snprintf( header->size, 12, "%11.11lo", ( unsigned long )size );
    __cube_calculate_checksum( header );
}



static
tar_gnu_header*
__cube_create_tar_header( report_layout_writer* tar_writer, char* filename, size_t size )
{
    tar_gnu_header* tar_header = ( tar_gnu_header* )CUBEW_CALLOC( 1, sizeof( tar_gnu_header ), MEMORY_TRACING_PREFIX "Allocate tar header" );
    memcpy( tar_header->name, filename,  strlen( filename ) );
    memcpy( tar_header->mode, tar_writer->mode,  strlen( tar_writer->mode ) );
    snprintf( tar_header->uid, 8,  "%7.7o", ( unsigned  )( tar_writer->uid ) );
    snprintf( tar_header->gid, 8, "%7.7o", ( unsigned )( tar_writer->gid ) );
    unsigned int mtime = time( NULL );
    snprintf( tar_header->mtime, 12, "%11.11lo", ( unsigned long )mtime );
    memcpy( tar_header->typeflag, "0", 1 );
    memcpy( tar_header->uname, tar_writer->username, strlen( tar_writer->username ) );
    memcpy( tar_header->gname, tar_writer->group, strlen( tar_writer->group ) );

    char* _magic = "ustar";
    memcpy( tar_header->magic, _magic, strlen( _magic ) );
    char* _version = "00";
    memcpy( tar_header->version, _version, strlen( _version ) );

    __cube_set_size_and_calculate_checksum( tar_header, size );

    return tar_header;
}


static
void
__cube_tar_file_finish( report_layout_writer* tar_writer, uint64_t size )
{
    fflush( tar_writer->tar );
    uint64_t actual_pos = ftell( tar_writer->tar );

    cubew_fseeko( tar_writer->tar, tar_writer->header_position, SEEK_SET );
    __cube_set_size_and_calculate_checksum( tar_writer->actual_tar_header, size );
    fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
    cubew_fseeko( tar_writer->tar, actual_pos, SEEK_SET );
    uint64_t difference = ( ( size / sizeof( tar_empty_block ) + 1 ) * sizeof( tar_empty_block )  - size );
    char*    _tmp       = ( char* )CUBEW_CALLOC( difference, sizeof( char ), MEMORY_TRACING_PREFIX "Allocate tail of a tar block" );
    fwrite( _tmp, 1, difference, tar_writer->tar );
    CUBEW_FREE( _tmp, MEMORY_TRACING_PREFIX "Release tail in a tar block" );
    CUBEW_FREE( tar_writer->actual_tar_header, MEMORY_TRACING_PREFIX "Release tar header" );
    tar_writer->actual_tar_header = NULL;
}

static
void
__cube_tar_data_file_finish( report_layout_writer* tar_writer, uint64_t size )
{
    fflush( tar_writer->tar );
    uint64_t difference = ( ( size / sizeof( tar_empty_block ) + 1 ) * sizeof( tar_empty_block )  - size );
    char*    _tmp       = ( char* )CUBEW_CALLOC( difference, sizeof( char ), MEMORY_TRACING_PREFIX "Allocate tail of a tar block" );
    fwrite( _tmp, 1, difference, tar_writer->tar );
    CUBEW_FREE( _tmp, MEMORY_TRACING_PREFIX "Release tail in a tar block" );
    CUBEW_FREE( tar_writer->actual_tar_header, MEMORY_TRACING_PREFIX "Release tar header" );
    tar_writer->actual_tar_header = NULL;
}

static
void
__cube_tar_finish( report_layout_writer* tar_writer )
{
    tar_empty_block* block = ( tar_empty_block* )CUBEW_CALLOC( 1, sizeof( tar_empty_block ), MEMORY_TRACING_PREFIX "Allocate tarblock " );
    fwrite( ( char* )block, 1, sizeof( tar_empty_block ), tar_writer->tar );
    fwrite( ( char* )block, 1, sizeof( tar_empty_block ), tar_writer->tar );
    CUBEW_FREE( block, MEMORY_TRACING_PREFIX "Release block " );
}

static
void
__fill_pax_header( tar_gnu_header* tar4pax, tar_gnu_header* pax_block, uint64_t size )
{
    memset( pax_block, 0, TAR_BLOCKSIZE );
    snprintf( ( char* )pax_block, sizeof( tar_empty_block ), "xx size=%"  PRIu64 "\n", size );
    uint8_t len = strlen( ( char* )pax_block );
    snprintf( ( char* )pax_block, sizeof( tar_empty_block ), "%2.2d size=%"  PRIu64 "\n", len, size );
    memcpy( tar4pax->typeflag, "x", 1 );
    __cube_set_size_and_calculate_checksum( tar4pax, len );
}

FILE*
cube_report_anchor_start( report_layout_writer* tar_writer )
{
    /* if metric is not finished yet, one has to */

    if ( tar_writer == NULL )
    {
        UTILS_WARNING( "Non stanard run. Create faked tar_writer with temp name of cube \"__NOFILE__\". \n" );
        tar_writer = cube_writing_start( "__NOFILE__", CUBE_MASTER );
    }
    char* anchorname = cube_get_path_to_anchor( tar_writer->cubename );
    tar_writer->actual_tar_header = __cube_create_tar_header( tar_writer, anchorname, 0 );
    CUBEW_FREE( anchorname, MEMORY_TRACING_PREFIX "Release anchor name" );
    tar_writer->header_position = ftell( tar_writer->tar );
    fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
    tar_writer->file_start_position = ftell( tar_writer->tar );
    tar_writer->anchor_writing      = CUBE_TRUE;
    return tar_writer->tar;
}




FILE*
cube_report_metric_data_start( report_layout_writer* tar_writer, cube_metric* met )
{
    if ( tar_writer == NULL )
    {
        UTILS_WARNING( "Non stanard run. Create faked tar_writer with temp name of cube \"__NOFILE__\". \n" );
        tar_writer = cube_writing_start( "__NOFILE__", CUBE_MASTER );
    }
    /* size of metric !?*/
    char*    dataname = cube_get_path_to_metric_data( tar_writer->cubename, met );
    uint64_t size     = cube_metric_size_of_data_file( met );
    tar_writer->actual_tar_header = __cube_create_tar_header( tar_writer, dataname, size );
    if ( size >= 0x1FFFFFFFF ) /* write pax header*/
    {
        char* paxname = calloc( 1, 11 + strlen( dataname ) );
        sprintf( paxname, "PaxHeader/%s", dataname );
        tar_gnu_header* tar4pax    = __cube_create_tar_header( tar_writer, paxname, TAR_BLOCKSIZE );
        tar_gnu_header* pax_header = __cube_create_tar_header( tar_writer, dataname, TAR_BLOCKSIZE );
        __fill_pax_header( tar4pax, pax_header, size );
        fwrite( tar4pax, 1, sizeof( tar_gnu_header ), tar_writer->tar );
        fwrite( pax_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
        CUBEW_FREE( paxname, MEMORY_TRACING_PREFIX "Release report_layout_writer metric pax name" );
        CUBEW_FREE( tar4pax, MEMORY_TRACING_PREFIX "Release tar header for PAX block" );
        CUBEW_FREE( pax_header, MEMORY_TRACING_PREFIX "Release PAX block" );
    }

    CUBEW_FREE( dataname, MEMORY_TRACING_PREFIX "Release metric data name" );
    tar_writer->header_position = ftell( tar_writer->tar );
    fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
    tar_writer->file_start_position = ftell( tar_writer->tar );
    met->start_pos_of_datafile      = tar_writer->file_start_position;
    return tar_writer->tar;
}


FILE*
cube_report_metric_index_start( report_layout_writer* tar_writer, cube_metric* met )
{
    if ( tar_writer == NULL )
    {
        UTILS_WARNING( "Non stanard run. Create faked tar_writer with temp name of cube \"__NOFILE__\". \n" );
        tar_writer = cube_writing_start( "__NOFILE__", CUBE_MASTER );
    }

    char* indexname = cube_get_path_to_metric_index( tar_writer->cubename, met );
    tar_writer->actual_tar_header = __cube_create_tar_header( tar_writer, indexname, 0 );
    CUBEW_FREE( indexname, MEMORY_TRACING_PREFIX "Release metric index name"  );
    tar_writer->header_position = ftell( tar_writer->tar );
    fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
    tar_writer->file_start_position = ftell( tar_writer->tar );
    return tar_writer->tar;
}


FILE*
cube_report_misc_data_start( report_layout_writer* tar_writer, const char* dataname )
{
    if ( tar_writer == NULL )
    {
        UTILS_WARNING( "Non stanard run. Create faked tar_writer with temp name of cube \"__NOFILE__\". \n" );
        tar_writer = cube_writing_start( "__NOFILE__", CUBE_MASTER );
    }
    else
    {
        if ( tar_writer->actual_metric != NULL )
        {
            cube_metric_finish( tar_writer->actual_metric, 1 );  /* 1 - signalizes, that writing will be  done. This call can be performed only by CUBE_MASTER */
        }
        tar_writer->actual_metric = NULL;

        // from here we finished writing metric.

        char* _dataname = cube_get_path_to_misc_data( tar_writer->cubename, dataname );
        tar_writer->actual_tar_header = __cube_create_tar_header( tar_writer, _dataname, 0 );
        CUBEW_FREE( _dataname, MEMORY_TRACING_PREFIX "Release _dataname " );
        tar_writer->header_position = ftell( tar_writer->tar );
        fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
        tar_writer->file_start_position = ftell( tar_writer->tar );
    }
    return tar_writer->tar;
}

void
cube_report_metric_data_finish( report_layout_writer* tar_writer, cube_metric* met, FILE* file )
{
    if ( cubew_fseeko( tar_writer->tar, 0, SEEK_END ) != 0 )
    {
        UTILS_WARNING( "Cannot seek to the end of the data  file to finish its writing in the tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
    }
    ;
    /* check for finish*/
    uint64_t size = ftell( tar_writer->tar ) - ( tar_writer->file_start_position );
    __cube_tar_data_file_finish( tar_writer, size );
}


void
cube_report_misc_data_finish( report_layout_writer* tar_writer, FILE* file )
{
    if ( cubew_fseeko( tar_writer->tar, 0, SEEK_END ) != 0 )
    {
        UTILS_WARNING( "Cannot seek to the end of the data  file to finish its writing in the tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
    }
    ;
    /* check for finish*/
    uint64_t size = ftell( tar_writer->tar ) - ( tar_writer->file_start_position );
    __cube_tar_file_finish( tar_writer, size );
}

void
cube_report_metric_data_weak_finish( report_layout_writer* tar_writer, cube_metric* met, FILE* file )
{
    return; /*  in tar case this does nothing*/
}


void
cube_report_metric_index_finish( report_layout_writer* tar_writer, cube_metric* met, FILE* file )
{
    if ( cubew_fseeko( tar_writer->tar, 0, SEEK_END ) != 0 )
    {
        UTILS_WARNING( "Cannot seek to the end of index file to finish its writing in the tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
    }
    ;
    uint64_t size = ftell( tar_writer->tar ) - ( tar_writer->file_start_position );
    __cube_tar_file_finish( tar_writer, size );
}


void
cube_report_anchor_finish( report_layout_writer* tar_writer, FILE* file )
{
    if ( cubew_fseeko( tar_writer->tar, 0, SEEK_END ) != 0 )
    {
        UTILS_WARNING( "Cannot seek to the end of anchor file to finish its writing in the tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
    }
    ;

    uint64_t size = ftell( tar_writer->tar ) - ( tar_writer->file_start_position );
    __cube_tar_file_finish( tar_writer, size );
}






report_layout_writer*
cube_writing_start( char* cubename, enum CubeFlavours_t cf )
{
    report_layout_writer* tar_writer = ( tar_writer_t* )CUBEW_CALLOC( 1, sizeof( tar_writer_t ), MEMORY_TRACING_PREFIX "Allocate tar writer" );
    tar_writer->cubename = cubew_strdup( cubename );
    tar_writer->mode     = cubew_strdup( "0000600" );
    tar_writer->username = cubew_strdup( getenv( "USER" ) );
    if ( tar_writer->username == NULL )
    {
        tar_writer->username = cubew_strdup( getenv( "LOGNAME" ) );
    }
    if ( tar_writer->username == NULL )
    {
        tar_writer->username = cubew_strdup( "nouser" );
    }

    tar_writer->group = ( char* )CUBEW_CALLOC( 32, sizeof( char ), MEMORY_TRACING_PREFIX "Allocate group name" );
    strcpy( tar_writer->group, "users" );
    tar_writer->uid             = getuid();
    tar_writer->gid             = getgid();
    tar_writer->actual_tar_file = __cube_get_tared_cube_name( cubename );
    tar_writer->tar             = fopen( tar_writer->actual_tar_file, "wb" );
    if ( tar_writer->tar == NULL )
    {
        UTILS_WARNING( "Cannot open tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
        UTILS_WARNING( "Return NULL.\n" );
    }
    tar_writer->actual_metric       = NULL;
    tar_writer->actual_tar_header   = NULL;
    tar_writer->header_position     = 0;
    tar_writer->file_start_position = 0;
    tar_writer->anchor_writing      = CUBE_FALSE;
    tar_writer->cube_flavour        = cf;
    return tar_writer;
}




void
cube_report_write_metric_row( report_layout_writer* tar_writer, cube_metric* met )
{
    if ( tar_writer->cube_flavour == CUBE_SLAVE )
    {
        return;
    }
    if ( tar_writer->actual_metric == met )
    {
        return;
    }
    if ( tar_writer->actual_metric != NULL )
    {
        cube_metric_finish( tar_writer->actual_metric, 1 );  /* 1 - signalizes, that writing will be  done. This call can be performed only by CUBE_MASTER */
    }
    met->data_file            = cube_report_metric_data_start( tar_writer, met );
    tar_writer->actual_metric = met;
}






report_layout_writer*
cube_writing_end( report_layout_writer* tar_writer )
{
    if ( tar_writer->cube_flavour != CUBE_MASTER )
    {
        return NULL;
    }
    __cube_tar_finish( tar_writer );
    CUBEW_FREE( tar_writer->actual_tar_file, MEMORY_TRACING_PREFIX "Release tar file name " );
    CUBEW_FREE( tar_writer->cubename, MEMORY_TRACING_PREFIX "Release cube filename " );
    CUBEW_FREE( tar_writer->username, MEMORY_TRACING_PREFIX "Release username " );
    CUBEW_FREE( tar_writer->mode, MEMORY_TRACING_PREFIX "Release permission mode" );
    CUBEW_FREE( tar_writer->group, MEMORY_TRACING_PREFIX "Release group name" );
    fclose( tar_writer->tar );
    CUBEW_FREE( tar_writer, MEMORY_TRACING_PREFIX "Release tar writer" );
    tar_writer = NULL;
    return tar_writer;
}
