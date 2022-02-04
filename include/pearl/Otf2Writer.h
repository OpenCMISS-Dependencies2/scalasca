/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 * @file
 * @ingroup PEARL_writer
 * @brief   Declaration of the class Otf2Writer.
 *
 * This header file provides the declaration of the class Otf2Writer.
 */
/*-------------------------------------------------------------------------*/


#ifndef PEARL_OTF2WRITER_H
#define PEARL_OTF2WRITER_H


#include <map>
#include <string>

#include <otf2/OTF2_Archive.h>
#include <otf2/OTF2_GlobalDefWriter.h>

#include <pearl/pearl_types.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class CallbackData;
class CallbackManager;
class Event;
class GlobalDefs;
class LocalTrace;
class String;


/*-------------------------------------------------------------------------*/
/**
 * @class   Otf2Writer
 * @ingroup PEARL_writer
 * @brief   Provides a writer class to generate OTF2 traces.
 *
 * The Otf2Writer class can be used to write in-memory trace data to an OTF2
 * archive on disk.
 *
 * @todo Hide references to OTF2 data types so that PEARL users don't need
 *       to care about providing the OTF2 include path.
 */
/*-------------------------------------------------------------------------*/

class Otf2Writer
{
    public:
        /// @name Constructors & destructor
        /// @{

        Otf2Writer(const std::string& creator);

        ~Otf2Writer();

        /// @}
        /// @name Writing experiment data
        /// @{

        void
        write(const std::string& archive_name,
              const LocalTrace&  trace,
              const GlobalDefs&  defs);

        /// @}


    private:
        /// Creator string
        std::string mCreator;

        /// OTF2 archive handle
        OTF2_Archive* mArchive;

        /// Identifier of the next string definition record
        uint32_t mNextStringId;

        /// Global MPI rank
        int mRank;

        /// Lookup table string definition |-@> string ID
        std::map< std::string, uint32_t > mStringMap;

        /// %Callback manager instance for the replay
        CallbackManager* mCbManager;


        /// @name Replay callback routines
        /// @{

        void
        enter(const CallbackManager& cbmanager,
              int                    user_event,
              const Event&           event,
              CallbackData*          cdata);

        void
        enterProgram(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

        void
        leave(const CallbackManager& cbmanager,
              int                    user_event,
              const Event&           event,
              CallbackData*          cdata);

        void
        leaveProgram(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

        void
        mpisend(const CallbackManager& cbmanager,
                int                    user_event,
                const Event&           event,
                CallbackData*          cdata);

        void
        mpirecv(const CallbackManager& cbmanager,
                int                    user_event,
                const Event&           event,
                CallbackData*          cdata);

        void
        mpisendreq(const CallbackManager& cbmanager,
                   int                    user_event,
                   const Event&           event,
                   CallbackData*          cdata);

        void
        mpisendcomp(const CallbackManager& cbmanager,
                    int                    user_event,
                    const Event&           event,
                    CallbackData*          cdata);

        void
        mpirecvreq(const CallbackManager& cbmanager,
                   int                    user_event,
                   const Event&           event,
                   CallbackData*          cdata);

        void
        mpirecvcomp(const CallbackManager& cbmanager,
                    int                    user_event,
                    const Event&           event,
                    CallbackData*          cdata);

        void
        mpireqtested(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

        void
        mpicancelled(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

        void
        mpicollexit(const CallbackManager& cbmanager,
                    int                    user_event,
                    const Event&           event,
                    CallbackData*          cdata);

        void
        mpicollbegin(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

        void
        mpicollend(const CallbackManager& cbmanager,
                   int                    user_event,
                   const Event&           event,
                   CallbackData*          cdata);

        void
        threadBegin(const CallbackManager& cbmanager,
                    int                    user_event,
                    const Event&           event,
                    CallbackData*          cdata);

        void
        threadEnd(const CallbackManager& cbmanager,
                  int                    user_event,
                  const Event&           event,
                  CallbackData*          cdata);

        void
        threadWait(const CallbackManager& cbmanager,
                   int                    user_event,
                   const Event&           event,
                   CallbackData*          cdata);

        void
        threadCreate(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

        void
        threadTeamBegin(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        threadTeamEnd(const CallbackManager& cbmanager,
                      int                    user_event,
                      const Event&           event,
                      CallbackData*          cdata);

        void
        threadFork(const CallbackManager& cbmanager,
                   int                    user_event,
                   const Event&           event,
                   CallbackData*          cdata);

        void
        threadJoin(const CallbackManager& cbmanager,
                   int                    user_event,
                   const Event&           event,
                   CallbackData*          cdata);

        void
        threadAcquireLock(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

        void
        threadReleaseLock(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

        void
        threadTaskCreate(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

        void
        threadTaskComplete(const CallbackManager& cbmanager,
                           int                    user_event,
                           const Event&           event,
                           CallbackData*          cdata);

        void
        threadTaskSwitch(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

        /// @}
        /// @name Internal I/O routines
        /// @{

        void
        writeStrings(OTF2_GlobalDefWriter* writer,
                     const GlobalDefs&     defs);

        void
        writeSystemTree(OTF2_GlobalDefWriter* writer,
                        const GlobalDefs&     defs);

        void
        writeLocationGroups(OTF2_GlobalDefWriter* writer,
                            const GlobalDefs&     defs);

        void
        writeLocations(OTF2_GlobalDefWriter* writer,
                       const GlobalDefs&     defs);

        void
        writeRegions(OTF2_GlobalDefWriter* writer,
                     const GlobalDefs&     defs);

        void
        writeGroups(OTF2_GlobalDefWriter* writer,
                    const GlobalDefs&     defs);

        void
        writeMetrics(OTF2_GlobalDefWriter* writer,
                     const GlobalDefs&     defs);

        void
        writeCallpaths(OTF2_GlobalDefWriter* writer,
                       const GlobalDefs&     defs);

        void
        writeCommunicators(OTF2_GlobalDefWriter* writer,
                           const GlobalDefs&     defs);

        void
        writeTopologies(OTF2_GlobalDefWriter* writer,
                        const GlobalDefs&     defs);

        uint32_t
        writeString(OTF2_GlobalDefWriter* writer,
                    const String&         str);

        uint32_t
        writeString(OTF2_GlobalDefWriter* writer,
                    const std::string&    str);

        void
        writeMetricValues(OTF2_EvtWriter*   writer,
                          const GlobalDefs& defs,
                          const Event&      event);

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_OTF2WRITER_H
