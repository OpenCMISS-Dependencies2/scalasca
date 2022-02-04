/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_READER_H
#define OTF2_READER_H


/**
 *  @file
 *
 *  @brief      Reading interface for OTF2 archives.
 *
 *  @addtogroup usage OTF2 usage examples
 *  @{
 *
 *  @defgroup usage_reading Usage in reading mode - a simple example
 *  @{
 *  This is a short example of how to use the OTF2 reading interface. It shows
 *  how to define and register callbacks and how to use the reader interface to
 *  read all events of a given OTF2 archive. This example is available as
 *  source code in the file @refexample{otf2_reader_example,c}.
 *  @dontinclude otf2_reader_example.c
 *
 *  First include the OTF2 header.
 *  @skipline otf2/otf2.h
 *
 *  For this example some additional include statements are necessary.
 *  @line stdlib.h
 *  @until inttypes.h
 *
 *  Define an event callback for entering and leaving a region.
 *  @line static OTF2_CallbackCode
 *  @until }
 *  @until }
 *
 *  The global definition file provides all location IDs that are included in
 *  the OTF2 trace archive. When reading the global definitions these location
 *  IDs must be collected and stored by the user. Probably, the easiest way to
 *  do that is to use a C++ container.
 *  @line vector
 *  @until }
 *  @until }
 *  @until }
 *
 *  Now everything is prepared to begin with the main program.
 *  @line int
 *  @until {
 *
 *  Create a new reader handle. The path to the OTF2 anchor file must be
 *  provided as argument.
 *  @skipline OTF2_Reader_Open
 *
 *  We will operate in an serial context.
 *  @skipline SerialCollective
 *
 *  OTF2 provides an API to query the number of locations prior reading the
 *  global definitions. We use this to pre-allocate the storage for all
 *  locations.
 *  @line number_of_locations
 *  @until 0
 *
 *  Get the global definition reader from the reader handle.
 *  @skipline global_def_reader
 *
 *  Register the above defined global definition callbacks. All other definition
 *  callbacks will be deactivated. And instruct the reader to pass the
 *  @a locations object to each call of the callbacks.
 *  @line global_def_callbacks
 *  @until Delete
 *
 *  Read all global definitions. Every time a location definition is read, the
 *  previously registered callback is triggered.
 *  In @c definitions_read the number of read definitions is returned.
 *  @line definitions_read
 *  @until );
 *
 *  After reading all global definitions all location IDs are stored in the
 *  vector @c locations. After that, the locations that are supposed to be
 *  read are selected. In this example all.
 *  @line for
 *  @until }
 *
 *  When the locations are selected the according event and definition files can
 *  be opened. Note that the local definition files are optional, thus we need
 *  to remember the success of this call.
 *  @line successful_open_def_files
 *  @until OpenEvtFiles
 *
 *  When the files are opened the event and definition reader handle can be
 *  requested. In this example for all.
 *  To apply mapping tables stored in the local definitions, the local
 *  definitions must be read. Though the existence of these files are optional.
 *  The call to @eref{OTF2_Reader_GetEvtReader} is mandatory, but the result
 *  is unused.
 *  @line for
 *  @until }
 *  @until }
 *  @until }
 *
 *  The definition files can now be closed, if it was successfully opened in the
 *  first place.
 *  @line successful_open_def_files
 *  @until }
 *
 *  Open a new global event reader. This global reader automatically contains
 *  all previously opened local event readers.
 *  @skipline global_evt_reader
 *
 *  Register the above defined global event callbacks. All other global event
 *  callbacks will be deactivated.
 *  @line event_callbacks
 *  @until Delete
 *
 *  Read all events in the OTF2 archive. The events are automatically ordered by
 *  the time they occurred in the trace. Every time an enter or leave event is
 *  read, the previously registered callbacks are triggered.
 *  In @c events_read the number of read events is returned.
 *  @line events_read
 *  @until );
 *
 *  The global event reader can now be closed and the event files too.
 *  @skipline CloseGlobalEvtReader
 *  @skipline CloseEvtFiles
 *
 *  At the end, close the reader and exit. All opened event and definition
 *  readers are closed automatically.
 *  @line Close
 *  @until }
 *
 *  To compile your program use a command like the following. Note that we need
 *  to activate the C99 standard explicitly for GCC.
 *  @dontinclude Makefile
 *  @skip otf2_reader_example.c
 *  @line gcc
 *  @until -o
 *
 *  Now you can link your program with:
 *  @line gcc
 *  @until -o
 *
 *  @}
 *
 *  @}
 */


#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_Archive.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Keeps all necessary information for the reader. */
typedef struct OTF2_Reader_struct OTF2_Reader;


/** @brief Create a new reader handle.
 *
 *  Creates a new reader handle, opens an according archive handle, and calls a
 *  routine to register all necessary function pointers.
 *
 *  @param anchorFilePath   Path to the anchor file e.g. 'trace.otf2'. This
 *                          can be a relative as well as an absolute path.
 *
 *  @return                 Returns a handle to the reader if successful,
 *                          NULL otherwise.
 */
OTF2_Reader*
OTF2_Reader_Open( const char* anchorFilePath );


/** @brief Close a reader handle.
 *
 *  Closes a reader handle and releases all associated handles. Does nothing if
 *  NULL is provided.
 *
 *  @param reader           Reader handle.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_Close( OTF2_Reader* reader );


/** @brief Set the @a hint in the @a reader to the given @a value.
 *
 *  Hints can only be set once and only before OTF2 itself uses the hint
 *  the first time.
 *
 *  @param reader  Reader handle.
 *  @param hint    Name of the hint.
 *  @param value   Reference to the hint value.
 *
 *  @since Version 1.5
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             in case of NULL pointers for @a archive or @a value\,
 *             or an unknown @a hint value}
 *    @retcode{OTF2_ERROR_HINT_INVALID,
 *             in case the hint is not valid for this handle}
 *    @retcode{OTF2_ERROR_HINT_LOCKED,
 *             in case the hint was already set or was queried at least
 *             once by the handle}
 *    @retcode{OTF2_ERROR_HINT_INVALID_VALUE,
 *             in case the provided value is invalid for this hint}
 *  @retend
 */
OTF2_ErrorCode
OTF2_Reader_SetHint( OTF2_Reader* reader,
                     OTF2_Hint    hint,
                     void*        value );


/** @brief Set the collective callbacks for the reader.
 *
 *  The reader has as the default the serial collectives set.
 *
 *  This function is a collective operation.
 *
 *  @param reader               Reader handle.
 *  @param collectiveCallbacks  Struct holding the collective callback functions.
 *  @param collectiveData       Data passed to the collective callbacks in the
 *                              @p userData argument.
 *  @param globalCommContext    Global communication context.
 *  @param localCommContext     Local communication context. Unused in reading
 *                              mode. A local communication context may be
 *                              created via the callbacks which fits the one
 *                              used when the given trace was written.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_SetCollectiveCallbacks( OTF2_Reader*                    reader,
                                    const OTF2_CollectiveCallbacks* collectiveCallbacks,
                                    void*                           collectiveData,
                                    OTF2_CollectiveContext*         globalCommContext,
                                    OTF2_CollectiveContext*         localCommContext );


/** @brief Convenient function to set the collective callbacks to a serial
 *         implementation.
 *
 *  @param reader           Reader handle.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_SetSerialCollectiveCallbacks( OTF2_Reader* reader );


/** @brief Set the locking callbacks for the reader.
 *
 *  Can be called any time, but only once. Before this call no thread-safety
 *  is guaranteed.
 *
 *  @param reader            Reader handle.
 *  @param lockingCallbacks  Struct holding the locking callback functions.
 *  @param lockingData       Data passed to the locking callbacks in the
 *                           @p userData argument.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             in case of NULL pointers for @a archive or @a lockingCallbacks\,
 *             or mandatory callbacks in @a lockingCallbacks are missing}
 *    @retcode{OTF2_ERROR_INVALID_CALL,
 *             in case there were locking callbacks already set}
 *  @retend
 */
OTF2_ErrorCode
OTF2_Reader_SetLockingCallbacks( OTF2_Reader*                 reader,
                                 const OTF2_LockingCallbacks* lockingCallbacks,
                                 void*                        lockingData );


/** @brief Register event reader callbacks.
 *
 *  @param reader           OTF2_Reader handle.
 *  @param evtReader        Local event reader handle.
 *  @param callbacks        Callbacks for the event readers.
 *  @param userData         Addition user data.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_RegisterEvtCallbacks( OTF2_Reader*                   reader,
                                  OTF2_EvtReader*                evtReader,
                                  const OTF2_EvtReaderCallbacks* callbacks,
                                  void*                          userData );


/** @brief Register global event reader callbacks.
 *
 *  @param reader           OTF2_Reader handle.
 *  @param evtReader        Global event reader handle.
 *  @param callbacks        Callbacks for the global event reader.
 *  @param userData         Addition user data.
 *
 *  @return Returns @eref{OTF2_SUCCESS} if successful, an error code if an error
 *          occurs.
 */
OTF2_ErrorCode
OTF2_Reader_RegisterGlobalEvtCallbacks( OTF2_Reader*                         reader,
                                        OTF2_GlobalEvtReader*                evtReader,
                                        const OTF2_GlobalEvtReaderCallbacks* callbacks,
                                        void*                                userData );


/** @brief Register local definition reader callbacks.
 *
 *  @param reader           OTF2_Reader handle.
 *  @param defReader        Local definition reader handle.
 *  @param callbacks        Callbacks for the local definition readers.
 *  @param userData         Addition user data.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_RegisterDefCallbacks( OTF2_Reader*                   reader,
                                  OTF2_DefReader*                defReader,
                                  const OTF2_DefReaderCallbacks* callbacks,
                                  void*                          userData );


/** @brief Register global definition reader callbacks.
 *
 *  @param reader           OTF2_Reader handle.
 *  @param defReader        Global definition reader handle.
 *  @param callbacks        Callbacks for the global definition readers.
 *  @param userData         Addition user data.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_RegisterGlobalDefCallbacks( OTF2_Reader*                         reader,
                                        OTF2_GlobalDefReader*                defReader,
                                        const OTF2_GlobalDefReaderCallbacks* callbacks,
                                        void*                                userData );


/** @brief Register snapshot event reader callbacks.
 *
 *  @param reader           OTF2_Reader handle.
 *  @param snapReader       Local snap reader handle.
 *  @param callbacks        Callbacks for the event readers.
 *  @param userData         Addition user data.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_RegisterSnapCallbacks( OTF2_Reader*                    reader,
                                   OTF2_SnapReader*                snapReader,
                                   const OTF2_SnapReaderCallbacks* callbacks,
                                   void*                           userData );


/** @brief Register global event reader callbacks.
 *
 *  @param reader           OTF2_Reader handle.
 *  @param evtReader        Global event reader handle.
 *  @param callbacks        Callbacks for the global event reader.
 *  @param userData         Addition user data.
 *
 *  @since Version 1.2
 *
 *  @return Returns @eref{OTF2_SUCCESS} if successful, an error code if an error
 *          occurs.
 */
OTF2_ErrorCode
OTF2_Reader_RegisterGlobalSnapCallbacks( OTF2_Reader*                          reader,
                                         OTF2_GlobalSnapReader*                evtReader,
                                         const OTF2_GlobalSnapReaderCallbacks* callbacks,
                                         void*                                 userData );


/** @brief Register marker reader callbacks.
 *
 *  @param reader           OTF2_Reader handle.
 *  @param markerReader     Marker reader handle.
 *  @param callbacks        Callbacks for the marker reader.
 *  @param userData         Addition user data.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_RegisterMarkerCallbacks( OTF2_Reader*                      reader,
                                     OTF2_MarkerReader*                markerReader,
                                     const OTF2_MarkerReaderCallbacks* callbacks,
                                     void*                             userData );


/** @brief Read a given number of events via a local event reader.
 *
 *  @param reader           Reader handle.
 *  @param evtReader        Local event reader handle.
 *  @param eventsToRead     Number events to be read.
 *  @param eventsRead       Return pointer to the number of events actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadLocalEvents( OTF2_Reader*    reader,
                             OTF2_EvtReader* evtReader,
                             uint64_t        eventsToRead,
                             uint64_t*       eventsRead );


/** @brief Read all events via a local event reader.
 *
 *  @param reader           Reader handle.
 *  @param evtReader        Local event reader handle.
 *  @param[out] eventsRead  Return pointer to the number of events actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadAllLocalEvents( OTF2_Reader*    reader,
                                OTF2_EvtReader* evtReader,
                                uint64_t*       eventsRead );


/** @brief Read a given number of events via a local event reader backwards.
 *
 *  @param reader           Reader handle.
 *  @param evtReader        Local event reader handle.
 *  @param eventsToRead     Number events to be read.
 *  @param[out] eventsRead  Return pointer to the number of events actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadLocalEventsBackward( OTF2_Reader*    reader,
                                     OTF2_EvtReader* evtReader,
                                     uint64_t        eventsToRead,
                                     uint64_t*       eventsRead );


/** @brief Read an event via a global event reader.
 *
 *  @param reader           Reader handle.
 *  @param evtReader        Global event reader handle.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadGlobalEvent( OTF2_Reader*          reader,
                             OTF2_GlobalEvtReader* evtReader );


/** @brief Has the global event reader at least one more event to deliver.
 *
 *  @param reader           Global event reader handle.
 *  @param evtReader        Global event reader handle.
 *  @param[out] flag        In case of success, the flag will be set to 1
 *                          when there is at least more more event to read.
 *                          To 0 if not. Otherwise the value is undefined.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_HasGlobalEvent( OTF2_Reader*          reader,
                            OTF2_GlobalEvtReader* evtReader,
                            int*                  flag );


/** @brief Read a given number of events via a global event reader.
 *
 *  @param reader           Reader handle.
 *  @param evtReader        Global event reader handle.
 *  @param eventsToRead     Number events to be read.
 *  @param[out] eventsRead  Return pointer to the number of events actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadGlobalEvents( OTF2_Reader*          reader,
                              OTF2_GlobalEvtReader* evtReader,
                              uint64_t              eventsToRead,
                              uint64_t*             eventsRead );


/** @brief Read all events via a global event reader.
 *
 *  @param reader           Reader handle.
 *  @param evtReader        Global event reader handle.
 *  @param[out] eventsRead  Return pointer to the number of events actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadAllGlobalEvents( OTF2_Reader*          reader,
                                 OTF2_GlobalEvtReader* evtReader,
                                 uint64_t*             eventsRead );


/** @brief Read a given number of definitions via a local definition reader.
 *
 *  @param reader               Reader handle.
 *  @param defReader            Local definition reader handle.
 *  @param definitionsToRead    Number definitions to be read.
 *  @param[out] definitionsRead Return pointer to the number of definitions
 *                              actually read.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INTERRUPTED_BY_CALLBACK,
 *             if an user supplied callback returned OTF2_CALLBACK_INTERRUPT}
 *    @retcode{OTF2_ERROR_DUPLICATE_MAPPING_TABLE,
 *             if a duplicate mapping table definition was read}
 *    @retelse{the error code}
 *  @retend
 */
OTF2_ErrorCode
OTF2_Reader_ReadLocalDefinitions( OTF2_Reader*    reader,
                                  OTF2_DefReader* defReader,
                                  uint64_t        definitionsToRead,
                                  uint64_t*       definitionsRead );


/** @brief Read all definitions via a local definition reader.
 *
 *  @param reader               Reader handle.
 *  @param defReader            Local definition reader handle.
 *  @param[out] definitionsRead Return pointer to the number of definitions
 *                              actually read.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INTERRUPTED_BY_CALLBACK,
 *             if an user supplied callback returned OTF2_CALLBACK_INTERRUPT}
 *    @retcode{OTF2_ERROR_DUPLICATE_MAPPING_TABLE,
 *             if a duplicate mapping table definition was read}
 *    @retelse{the error code}
 *  @retend
 */
OTF2_ErrorCode
OTF2_Reader_ReadAllLocalDefinitions( OTF2_Reader*    reader,
                                     OTF2_DefReader* defReader,
                                     uint64_t*       definitionsRead );


/** @brief Read a given number of definitions via a global definition reader.
 *
 *  @param reader               Reader handle.
 *  @param defReader            Global definition reader handle.
 *  @param definitionsToRead    Number definitions to be read.
 *  @param[out] definitionsRead Return pointer to the number of definitions
 *                              actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadGlobalDefinitions( OTF2_Reader*          reader,
                                   OTF2_GlobalDefReader* defReader,
                                   uint64_t              definitionsToRead,
                                   uint64_t*             definitionsRead );


/** @brief Read all definitions via a global definition reader.
 *
 *  @param reader               Reader handle.
 *  @param defReader            Global definition reader handle.
 *  @param[out] definitionsRead Return pointer to the number of definitions
 *                              actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadAllGlobalDefinitions( OTF2_Reader*          reader,
                                      OTF2_GlobalDefReader* defReader,
                                      uint64_t*             definitionsRead );


/** @brief Read a given number of records via a local snapshot reader.
 *
 *  @param reader           Reader handle.
 *  @param snapReader       Local snapshot reader handle.
 *  @param recordsToRead    Number records to be read.
 *  @param recordsRead      Return pointer to the number of records actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 *  @since Version 1.2
 */
OTF2_ErrorCode
OTF2_Reader_ReadLocalSnapshots( OTF2_Reader*     reader,
                                OTF2_SnapReader* snapReader,
                                uint64_t         recordsToRead,
                                uint64_t*        recordsRead );


/** @brief Read all records via a local snapshot reader.
 *
 *  @param reader            Reader handle.
 *  @param snapReader        Local snapshot reader handle.
 *  @param[out] recordsRead  Return pointer to the number of records
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 *  @since Version 1.2
 */
OTF2_ErrorCode
OTF2_Reader_ReadAllLocalSnapshots( OTF2_Reader*     reader,
                                   OTF2_SnapReader* snapReader,
                                   uint64_t*        recordsRead );


/** @brief Read a given number of records via a global snapshot reader.
 *
 *  @param reader           Reader handle.
 *  @param snapReader       Global snapshot reader handle.
 *  @param recordsToRead    Number records to be read.
 *  @param[out] recordsRead Return pointer to the number of records
 *                          actually read.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 *  @since Version 1.2
 */
OTF2_ErrorCode
OTF2_Reader_ReadGlobalSnapshots( OTF2_Reader*           reader,
                                 OTF2_GlobalSnapReader* snapReader,
                                 uint64_t               recordsToRead,
                                 uint64_t*              recordsRead );


/** @brief Read all records via a global snapshot reader.
 *
 *  @param reader           Reader handle.
 *  @param snapReader       Global snapshot reader handle.
 *  @param[out] recordsRead Return pointer to the number of records
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 *  @since Version 1.2
 */
OTF2_ErrorCode
OTF2_Reader_ReadAllGlobalSnapshots( OTF2_Reader*           reader,
                                    OTF2_GlobalSnapReader* snapReader,
                                    uint64_t*              recordsRead );


/** @brief Read a given number of markers via a marker reader.
 *
 *  @param reader               Reader handle.
 *  @param markerReader         Marker reader handle.
 *  @param markersToRead        Number markers to be read.
 *  @param[out] markersRead     Return pointer to the number of markers
 *                              actually read.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadMarkers( OTF2_Reader*       reader,
                         OTF2_MarkerReader* markerReader,
                         uint64_t           markersToRead,
                         uint64_t*          markersRead );


/** @brief Read all markers via a marker reader.
 *
 *  @param reader               Reader handle.
 *  @param markerReader         Marker reader handle.
 *  @param[out] markersRead     Return pointer to the number of markers
 *                              actually read.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_ReadAllMarkers( OTF2_Reader*       reader,
                            OTF2_MarkerReader* markerReader,
                            uint64_t*          markersRead );


/** @brief Get a local event reader.
 *
 *  @param reader           Valid reader handle.
 *  @param location         Location ID for the requested local reader.
 *
 *  @return                 Returns a handle to the local event reader if
 *                          successful, NULL otherwise.
 */
OTF2_EvtReader*
OTF2_Reader_GetEvtReader( OTF2_Reader*     reader,
                          OTF2_LocationRef location );


/** @brief Get a global event reader.
 *
 *  @param reader           Valid reader handle.
 *
 *  @return                 Returns a handle to the global event reader if
 *                          successful, NULL otherwise.
 */
OTF2_GlobalEvtReader*
OTF2_Reader_GetGlobalEvtReader( OTF2_Reader* reader );


/** @brief Get a local definition reader.
 *
 *  @param reader           Valid reader handle.
 *  @param location         Location ID for the requested local reader.
 *
 *  @return                 Returns a handle to the local definition reader if
 *                          successful, NULL otherwise.
 */
OTF2_DefReader*
OTF2_Reader_GetDefReader( OTF2_Reader*     reader,
                          OTF2_LocationRef location );


/** @brief Get a global definition reader.
 *
 *  @param reader           Valid reader handle.
 *
 *  @return                 Returns a handle to the global definition reader if
 *                          successful, NULL otherwise.
 */
OTF2_GlobalDefReader*
OTF2_Reader_GetGlobalDefReader( OTF2_Reader* reader );


/** @brief Get a local snapshot reader.
 *
 *  @param reader           Valid reader handle.
 *  @param location         Location ID for the requested local reader.
 *
 *  @return                 Returns a handle to the local event reader if
 *                          successful, NULL otherwise.
 *  @since Version 1.2
 */
OTF2_SnapReader*
OTF2_Reader_GetSnapReader( OTF2_Reader*     reader,
                           OTF2_LocationRef location );


/** @brief Get a global snap reader.
 *
 *  @param reader           Valid reader handle.
 *
 *  @return                 Returns a handle to the global snap reader if
 *                          successful, NULL otherwise.
 *  @since Version 1.2
 */
OTF2_GlobalSnapReader*
OTF2_Reader_GetGlobalSnapReader( OTF2_Reader* reader );


/** @brief Get a thumb reader.
 *
 *  @param reader           Reader handle.
 *  @param number           Thumbnail number.
 *
 *  @since Version 1.2
 *
 *  @return                 Returns a global definition writer handle if
 *                          successful, NULL if an error occurs.
 */
OTF2_ThumbReader*
OTF2_Reader_GetThumbReader( OTF2_Reader* reader,
                            uint32_t     number );


/** @brief Get a marker reader.
 *
 *  @param reader           Valid reader handle.
 *
 *  @since Version 1.2
 *
 *  @return                 Returns a handle to the marker reader if
 *                          successful, NULL otherwise.
 */
OTF2_MarkerReader*
OTF2_Reader_GetMarkerReader( OTF2_Reader* reader );


/** @brief Get a marker writer.
 *
 *  @param reader           Valid reader handle.
 *
 *  @since Version 1.2
 *
 *  @return                 Returns a handle to the marker writer if
 *                          successful, NULL otherwise.
 */
OTF2_MarkerWriter*
OTF2_Reader_GetMarkerWriter( OTF2_Reader* reader );


/** @brief Close a local event reader.
 *
 *  @param reader           Valid reader handle.
 *  @param evtReader        Event reader to be closed.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_CloseEvtReader( OTF2_Reader*    reader,
                            OTF2_EvtReader* evtReader );


/** @brief Closes the global event reader.
 *
 *  This closes also all local event readers.
 *
 *  @param reader           Valid reader handle.
 *  @param globalEvtReader  The global event reader.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_CloseGlobalEvtReader( OTF2_Reader*          reader,
                                  OTF2_GlobalEvtReader* globalEvtReader );


/** @brief Close a local definition reader.
 *
 *  @param reader           Valid reader handle.
 *  @param defReader        Definition reader to be closed.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_CloseDefReader( OTF2_Reader*    reader,
                            OTF2_DefReader* defReader );


/** @brief Closes the global definition reader.
 *
 *  @param reader           Valid reader handle.
 *  @param globalDefReader  The global definition reader.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_CloseGlobalDefReader( OTF2_Reader*          reader,
                                  OTF2_GlobalDefReader* globalDefReader );


/** @brief Close a local snapshot reader.
 *
 *  @param reader           Valid reader handle.
 *  @param snapReader       snapshot reader to be closed.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 *  @since Version 1.2
 */
OTF2_ErrorCode
OTF2_Reader_CloseSnapReader( OTF2_Reader*     reader,
                             OTF2_SnapReader* snapReader );


/** @brief Closes the global snapshot reader.
 *
 *  @param reader           Valid reader handle.
 *  @param globalSnapReader The global snapshot reader.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 *  @since Version 1.2
 */
OTF2_ErrorCode
OTF2_Reader_CloseGlobalSnapReader( OTF2_Reader*           reader,
                                   OTF2_GlobalSnapReader* globalSnapReader );


/** @brief Close an opened thumbnail reader.
 *
 *  @param reader           Reader handle.
 *  @param thumbReader      Thumbnail reader handle to be closed.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_CloseThumbReader( OTF2_Reader*      reader,
                              OTF2_ThumbReader* thumbReader );


/** @brief Closes the marker reader.
 *
 *  @param reader           Valid reader handle.
 *  @param markerReader     The marker reader.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_CloseMarkerReader( OTF2_Reader*       reader,
                               OTF2_MarkerReader* markerReader );


/** @brief Closes the marker writer.
 *
 *  @param reader           Valid reader handle.
 *  @param markerWriter     The marker writer.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_CloseMarkerWriter( OTF2_Reader*       reader,
                               OTF2_MarkerWriter* markerWriter );


/** @brief Get OTF2 version.
 *
 *  @param reader           Valid reader handle.
 *  @param[out] major       Major version.
 *  @param[out] minor       Minor version.
 *  @param[out] bugfix      Bugfix revision.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetVersion( OTF2_Reader* reader,
                        uint8_t*     major,
                        uint8_t*     minor,
                        uint8_t*     bugfix );


/** @brief Get event and definition chunk sizes.
 *
 *  @param reader                    Reader handle.
 *  @param[out] chunkSizeEvents      Returned size of event chunks
 *  @param[out] chunkSizeDefinitions Returned size of definition chunks.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetChunkSize( OTF2_Reader* reader,
                          uint64_t*    chunkSizeEvents,
                          uint64_t*    chunkSizeDefinitions );


/** @brief Get file substrate information.
 *
 *  @param reader           Reader handle.
 *  @param[out] substrate   Returned file substrate.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetFileSubstrate( OTF2_Reader*        reader,
                              OTF2_FileSubstrate* substrate );


/** @brief Get compression mode.
 *
 *  @param reader           Reader handle.
 *  @param[out] compression Returned compression mode.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetCompression( OTF2_Reader*      reader,
                            OTF2_Compression* compression );


/** @brief Get number of locations.
 *
 *  @param reader                 Reader handle.
 *  @param[out] numberOfLocations Returned number of locations.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetNumberOfLocations( OTF2_Reader* reader,
                                  uint64_t*    numberOfLocations );


/** @brief Get number of global definitions.
 *
 *  @param reader                   Reader handle.
 *  @param[out] numberOfDefinitions Returned number of global definitions.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetNumberOfGlobalDefinitions( OTF2_Reader* reader,
                                          uint64_t*    numberOfDefinitions );


/** @brief Get machine name.
 *
 *  @param reader           Reader handle.
 *  @param[out] machineName Returned machine name. Allocated with @a malloc.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetMachineName( OTF2_Reader* reader,
                            char**       machineName );


/** @brief Get creator name.
 *
 *  @param reader           Reader handle.
 *  @param[out] creator     Returned creator. Allocated with @a malloc.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetCreator( OTF2_Reader* reader,
                        char**       creator );


/** @brief Get description.
 *
 *  @param reader           Reader handle.
 *  @param[out] description Returned description. Allocated with @a malloc.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetDescription( OTF2_Reader* reader,
                            char**       description );


/** @brief Get the names of all trace file properties.
 *
 *  @param reader                  Reader handle.
 *  @param[out] numberOfProperties Returned number of trace file properties.
 *  @param[out] names              Returned list of property names. Allocated
 *                                 with @a malloc. To release memory, just pass
 *                                 @p *names to @a free.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetPropertyNames( OTF2_Reader* reader,
                              uint32_t*    numberOfProperties,
                              char***      names );


/** @brief Get the value of the named trace file property.
 *
 *  @param reader           Reader handle.
 *  @param name             Name of the property.
 *  @param[out] value       Returned value of the property. Allocated with
 *                          @a malloc.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_PROPERTY_NOT_FOUND,
 *             if the named property was not found}
 *  @retend
 */
OTF2_ErrorCode
OTF2_Reader_GetProperty( OTF2_Reader* reader,
                         const char*  name,
                         char**       value );


/** @brief Get the value of the named trace file property as boolean.
 *
 *  @param reader           Reader handle.
 *  @param name             Name of the property.
 *  @param[out] value       Returned boolean value of the property.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_PROPERTY_NOT_FOUND,
 *             if the named property was not found}
 *    @retcode{OTF2_ERROR_PROPERTY_VALUE_INVALID,
 *             if the value could not be interpreted as a boolean value}
 *  @retend
 */
OTF2_ErrorCode
OTF2_Reader_GetBoolProperty( OTF2_Reader* reader,
                             const char*  name,
                             bool*        value );


/** @brief Get the identifier of the trace file.
 *
 *  @param reader           Reader handle.
 *  @param[out] id          Trace identifier.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_GetTraceId( OTF2_Reader* reader,
                        uint64_t*    id );


/** @brief Get number of snapshots.
 *
 *  @param reader                   Reader handle.
 *  @param[out] number              Returned number of snapshots.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 *  @since Version 1.2
 */
OTF2_ErrorCode
OTF2_Reader_GetNumberOfSnapshots( OTF2_Reader* reader,
                                  uint32_t*    number );


/** @brief Get number of thumbs.
 *
 *  @param reader                   Reader handle.
 *  @param[out] number              Returned number of thumbs.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 *  @since Version 1.2
 */
OTF2_ErrorCode
OTF2_Reader_GetNumberOfThumbnails( OTF2_Reader* reader,
                                   uint32_t*    number );


/** @brief Open the events file container.
 *
 *  This function is a collective operation.
 *
 *  @param reader           Reader handle.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 */
OTF2_ErrorCode
OTF2_Reader_OpenEvtFiles( OTF2_Reader* reader );


/** @brief Closes the events file container.
 *
 *  All previously used event readers must be closed before this call.
 *
 *  This function is a collective operation.
 *
 *  @param reader          Reader handle.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 */
OTF2_ErrorCode
OTF2_Reader_CloseEvtFiles( OTF2_Reader* reader );


/** @brief Open the local definitions file container.
 *
 *  This function is a collective operation.
 *
 *  @param reader          Reader handle.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 */
OTF2_ErrorCode
OTF2_Reader_OpenDefFiles( OTF2_Reader* reader );


/** @brief Closes the local definitions file container.
 *
 *  This function is a collective operation.
 *
 *  All previously used local definition readers must be closed before this call.
 *
 *  @param reader          Reader handle.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 */
OTF2_ErrorCode
OTF2_Reader_CloseDefFiles( OTF2_Reader* reader );


/** @brief Open the snapshots file container.
 *
 *  This function is a collective operation.
 *
 *  @param reader          Reader handle.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 */
OTF2_ErrorCode
OTF2_Reader_OpenSnapFiles( OTF2_Reader* reader );


/** @brief Closes the snapshots file container.
 *
 *  This function is a collective operation.
 *
 *  All previously used snapshot readers must be closed before this call.
 *
 *  @param reader          Reader handle.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 */
OTF2_ErrorCode
OTF2_Reader_CloseSnapFiles( OTF2_Reader* reader );


/** @brief Select a location to be read.
 *
 *  @param reader           Reader handle.
 *  @param location         Location ID.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 *
 */
OTF2_ErrorCode
OTF2_Reader_SelectLocation( OTF2_Reader*     reader,
                            OTF2_LocationRef location );


/** @brief Determines if this reader is the master object which handles the
 *  global archive state (anchor file, global definitions, marker, ...).
 *
 *  @param reader       Reader handle.
 *  @param[out] result  Storage for the result.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Reader_IsMaster( OTF2_Reader* reader,
                      bool*        result );



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_READER_H */
