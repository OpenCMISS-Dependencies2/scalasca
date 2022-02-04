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
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_PATTERN_H
#define SCOUT_PATTERN_H


#include <map>
#include <string>

#include <cubew_metric.h>

#include "scout_types.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file  Pattern.h
 *  @brief Declaration of the Pattern class.
 *
 *  This header file provides the declaration of the Pattern class which
 *  serves as an abstract base class for all performance properties calculated
 *  by SCOUT.
 **/
/*-------------------------------------------------------------------------*/


// --- Forward declarations -------------------------------------------------

namespace pearl
{
class CallbackManager;
class Callpath;
class GlobalDefs;
class LocalTrace;
}    // namespace pearl


namespace scout
{
// --- Forward declarations -------------------------------------------------

struct ReportData;


/*-------------------------------------------------------------------------*/
/**
 *  @class Pattern
 *  @brief Abstract base class for all properties calculated by SCOUT.
 *
 *  This class serves as an abstract base class for all performance properties
 *  calculated by SCOUT. This includes basic patterns such as Time and Visits,
 *  as well as the inefficiency patterns (i.e., wait states) occuring at
 *  synchronization points.
 **/
/*-------------------------------------------------------------------------*/

class Pattern
{
    public:
        /// @name Constructors & destructor
        /// @{

        virtual
        ~Pattern();

        /// @}
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const = 0;

        virtual long
        get_parent() const;

        virtual std::string
        get_name() const = 0;

        virtual std::string
        get_unique_name() const = 0;

        virtual std::string
        get_descr() const = 0;

        virtual std::string
        get_unit() const = 0;

        virtual CubeMetricType
        get_mode() const = 0;

        std::string
        get_url() const;

        virtual bool
        is_hidden() const;

        /// @}
        /// @name Writing severity values
        /// @{

        virtual void
        gen_severities(ReportData&              data,
                       int                      rank,
                       const pearl::LocalTrace& trace);

        /// @}


    protected:
        /// Local severity map. Stores a cnode @f$\times@f$ severity value matrix.
        std::map< pearl::Callpath*, double > m_severity;


        /// @name Pattern management
        /// @{

        virtual void
        init();

        /// @}
        /// @name Data collation control
        /// @{

        virtual bool
        skip_cnode(const pearl::Callpath& cnode) const;

        /// @}
};
}    // namespace scout


#endif    // !SCOUT_PATTERN_H
