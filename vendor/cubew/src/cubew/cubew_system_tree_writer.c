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
 * \file cubew_system_tree_writer.c
   \brief Provides the data structure traverser and system tree writer
 */

#include <config.h>
#include "cubew_system_tree_writer.h"
#include "UTILS_Error.h"
#include "cubew_memory.h"

#define MEMORY_TRACING_PREFIX "[SYSTEM TREE WRITER]"

const uint32_t states_size_chunk = 128;


cube_system_tree_writer*
cube_system_tree_writer_create()
{
    cube_system_tree_writer* __system_tree_writer = ( cube_system_tree_writer* )CUBEW_MALLOC( sizeof( cube_system_tree_writer ), MEMORY_TRACING_PREFIX "Allocate system tree writer"  );
    __system_tree_writer->init   = NULL;
    __system_tree_writer->step   = NULL;
    __system_tree_writer->finish = NULL;

    __system_tree_writer->user_ptr = NULL;

    __system_tree_writer->stn_id = 0;
    __system_tree_writer->lg_id  = 0;
    __system_tree_writer->loc_id = 0;

    __system_tree_writer->loc_id = 0;

    __system_tree_writer->stn_plain = cube_system_tree_node_plain_create();
    __system_tree_writer->lg_plain  = cube_location_group_plain_create();
    __system_tree_writer->loc_plain = cube_location_plain_create();



    /** basic state stack */
    __system_tree_writer->state_index                                  = 0;
    __system_tree_writer->states_size                                  = 1;
    __system_tree_writer->states                                       = CUBEW_MALLOC( states_size_chunk * sizeof( cube_system_tree_writer_state ), MEMORY_TRACING_PREFIX "Allocate vector of system tree writer states"  );
    __system_tree_writer->states[  __system_tree_writer->state_index ] = CUBE_SYSTEM_TREE_WRITER_INIT;

    __system_tree_writer->system_tree_information = ( cube_system_tree_information* )CUBEW_CALLOC( 1, sizeof( cube_system_tree_information ), MEMORY_TRACING_PREFIX "Allocate system tree information structure"  );

    return __system_tree_writer;
}


void
cube_system_tree_writer_free( cube_system_tree_writer* this )
{
    if ( this != NULL )
    {
        cube_system_tree_node_plain_free( this->stn_plain );
        cube_location_group_plain_free( this->lg_plain );
        cube_location_plain_free( this->loc_plain );
        CUBEW_FREE( this->states, MEMORY_TRACING_PREFIX "Release vector of system tree writer states" );
        CUBEW_FREE( this->system_tree_information, MEMORY_TRACING_PREFIX "Release system tree information structure"  );
    }
    CUBEW_FREE( this, MEMORY_TRACING_PREFIX "Release system tree writer" );
}


void
cube_system_tree_writer_setup( cube_system_tree_writer* this, cube_system_tree_writer_init init, cube_system_tree_writer_step step, cube_system_tree_writer_driver driver, cube_system_tree_writer_finish finish, void* _user_ptr )
{
    this->init     = init;
    this->step     = step;
    this->driver   = driver;
    this->finish   = finish;
    this->user_ptr = _user_ptr;
}


static
void
__cube_system_tree_writer_state_pop( cube_system_tree_writer* this )
{
#if DEBUG
    if ( this->state_index == 0 )
    {
        UTILS_FATAL( "Cannot reduce size of the states stack. Current size id 0\n" );
    }
#endif
    this->state_index--;
}


static
void
__cube_system_tree_writer_state_push( cube_system_tree_writer*      this,
                                      cube_system_tree_writer_state state )
{
    if ( this->state_index == ( this->states_size - 1 ) )
    {
        this->states       = ( cube_system_tree_writer_state* )CUBEW_REALLOC( this->states, this->states_size * sizeof( cube_system_tree_writer_state ),  ( this->states_size + states_size_chunk ) * sizeof( cube_system_tree_writer_state ), MEMORY_TRACING_PREFIX "Expand vector of states" );
        this->states_size += states_size_chunk;
    }
    this->state_index++;
    this->states[  this->state_index ] = state;
}

static
void
__cube_system_tree_writer_process( cube_system_tree_writer*      this,
                                   cube_system_tree_writer_state next_state,
                                   cube_system_tree_node_plain*  _stn_plain,
                                   cube_location_group_plain*    _lg_plain,
                                   cube_location_plain*          _loc_plain )
{
    cube_system_tree_writer_state current_state = this->states[  this->state_index ];
    switch ( current_state )
    {
        case CUBE_SYSTEM_TREE_WRITER_INIT:
            switch ( next_state )
            {
                case CUBE_SYSTEM_TREE_WRITER_STN:
                    __cube_system_tree_writer_state_push( this,  CUBE_SYSTEM_TREE_WRITER_STN );
                    cube_system_tree_node_plain_write( _stn_plain, this->stn_id,  this->meta_data_writer );
                    this->stn_id++;
                    break;
                case CUBE_SYSTEM_TREE_WRITER_INIT:
                case CUBE_SYSTEM_TREE_WRITER_LG:
                case CUBE_SYSTEM_TREE_WRITER_LOC:
                case CUBE_SYSTEM_TREE_WRITER_UP:
                    UTILS_WARNING( "Wrong system tree structure, One cannot start with location group, location or a 'step up'. System tree node is expected.\n" );
                    break;
            }
            ;
            break;
        case CUBE_SYSTEM_TREE_WRITER_STN:
            switch ( next_state )
            {
                case CUBE_SYSTEM_TREE_WRITER_STN:
                    __cube_system_tree_writer_state_push( this, CUBE_SYSTEM_TREE_WRITER_STN );
                    cube_system_tree_node_plain_write( this->stn_plain, this->stn_id,  this->meta_data_writer );
                    this->stn_id++;
                    break;
                case CUBE_SYSTEM_TREE_WRITER_LG:
                    __cube_system_tree_writer_state_push( this, CUBE_SYSTEM_TREE_WRITER_LG );
                    cube_location_group_plain_write( _lg_plain, this->lg_id, this->meta_data_writer );
                    this->lg_id++;
                    break;
                case CUBE_SYSTEM_TREE_WRITER_LOC:
                    UTILS_FATAL( "Wrong system tree structure. System tree node cannot have location as a child. Location group or system tree node expected.\n" );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_INIT:
                    UTILS_FATAL( "Error in processing. Illegal 'INIT' state.\n" );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_UP:
                    cube_system_tree_node_plain_write( NULL, 0, this->meta_data_writer );
                    __cube_system_tree_writer_state_pop( this );
                    break;
            }
            ;
            break;
        case CUBE_SYSTEM_TREE_WRITER_LG:
            switch ( next_state )
            {
                case CUBE_SYSTEM_TREE_WRITER_STN:
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, _stn_plain, _lg_plain, _loc_plain );
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_STN, _stn_plain, _lg_plain, _loc_plain  );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_LG:
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, _stn_plain, _lg_plain, _loc_plain  );
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_LG, _stn_plain, _lg_plain, _loc_plain  );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_LOC:
                    cube_location_plain_write( _loc_plain, this->loc_id, this->meta_data_writer );
                    cube_location_plain_write( NULL, 0, this->meta_data_writer );
                    this->loc_id++;
                    break;
                case CUBE_SYSTEM_TREE_WRITER_INIT:
                    UTILS_FATAL( "Error in processing. Illegal 'INIT' state.\n" );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_UP:
                    cube_location_group_plain_write( NULL, 0, this->meta_data_writer );
                    __cube_system_tree_writer_state_pop( this );
                    break;
            }
            ;
            break;
        case CUBE_SYSTEM_TREE_WRITER_LOC:
            switch ( next_state )
            {
                case CUBE_SYSTEM_TREE_WRITER_STN:
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, _stn_plain, _lg_plain, _loc_plain  );
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, _stn_plain, _lg_plain, _loc_plain  );
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_STN, _stn_plain, _lg_plain, _loc_plain  );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_LG:
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, _stn_plain, _lg_plain, _loc_plain  );
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, _stn_plain, _lg_plain, _loc_plain  );
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_LG, _stn_plain, _lg_plain, _loc_plain  );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_LOC:
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, _stn_plain, _lg_plain, _loc_plain  );
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_LOC, _stn_plain, _lg_plain, _loc_plain  );
                    __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, _stn_plain, _lg_plain, _loc_plain  );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_INIT:
                    UTILS_FATAL( "Error in processing. Illegal 'INIT' state.\n" );
                    break;
                case CUBE_SYSTEM_TREE_WRITER_UP:
                    cube_location_plain_write( NULL, 0, this->meta_data_writer );
                    __cube_system_tree_writer_state_pop( this );
                    break;
            }
            ;
            break;
    }
    ;
}



void
cube_system_tree_writer_driver_step( void*                         this,
                                     cube_system_tree_writer_state next_state,
                                     cube_system_tree_node_plain*  _stn_plain,
                                     cube_location_group_plain*    _lg_plain,
                                     cube_location_plain*          _loc_plain )
{
    cube_system_tree_writer* writer = ( cube_system_tree_writer* )this;
    __cube_system_tree_writer_process( writer, next_state, _stn_plain, _lg_plain, _loc_plain );


    /*
     * cleanup for next step
     */
    cube_system_tree_node_plain_clear( writer->stn_plain );
    cube_location_group_plain_clear( writer->lg_plain );
    cube_location_plain_clear( writer->loc_plain );
}



void
cube_system_tree_writer_start( cube_system_tree_writer* this )
{
    if ( ( this != NULL )
         && ( this->init != NULL )
         )
    {
        this->init( this->system_tree_information, this->user_ptr );
    }
}



void
cube_system_tree_writer_driver_write( cube_system_tree_writer* this, cube_meta_data_writer* writer )
{
    if ( ( this != NULL )
         && ( this->driver != NULL )
         )
    {
        this->driver( this, this->user_ptr );
    }
}






void
cube_system_tree_writer_write( cube_system_tree_writer* this, cube_meta_data_writer* writer )
{
    this->meta_data_writer = writer;
    cube_system_tree_node_plain_clear( this->stn_plain );
    cube_location_group_plain_clear( this->lg_plain );
    cube_location_plain_clear( this->loc_plain );

    if ( this->step == NULL && this->driver == NULL )
    {
        return;
    }
    if ( this->step == NULL && this->driver != NULL )
    {
        return cube_system_tree_writer_driver_write( this, writer );
    }
    if ( this->step != NULL && this->driver != NULL )
    {
        UTILS_FATAL( "'step' and 'driver' callbacks are not NULL. Only one at the time is allowed.\n" );
    }


    cube_system_tree_writer_state next_state = this->step( this->stn_plain, this->lg_plain, this->loc_plain, this->user_ptr );
    while (  next_state != CUBE_SYSTEM_TREE_WRITER_STOP )
    {
        __cube_system_tree_writer_process( this, next_state, this->stn_plain, this->lg_plain, this->loc_plain );

        cube_system_tree_node_plain_clear( this->stn_plain );
        cube_location_group_plain_clear( this->lg_plain );
        cube_location_plain_clear( this->loc_plain );

        next_state = this->step( this->stn_plain, this->lg_plain, this->loc_plain, this->user_ptr );
        if ( next_state == CUBE_SYSTEM_TREE_WRITER_END )
        {
            while ( this->state_index != 0 )
            {
                __cube_system_tree_writer_process( this, CUBE_SYSTEM_TREE_WRITER_UP, this->stn_plain, this->lg_plain, this->loc_plain  );
            }
        }
    }

    cube_system_tree_node_plain_clear( this->stn_plain );
    cube_location_group_plain_clear( this->lg_plain );
    cube_location_plain_clear( this->loc_plain );
}

void
cube_system_tree_writer_end( cube_system_tree_writer* this )
{
    if ( ( this != NULL )
         &&
         ( this->finish != NULL ) )
    {
        this->finish( this->user_ptr );
    }
}
