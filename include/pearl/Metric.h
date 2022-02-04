/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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


#ifndef PEARL_METRIC_H
#define PEARL_METRIC_H


#include <stdint.h>

#include <string>


/*-------------------------------------------------------------------------*/
/**
 *  @file    Metric.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Metric.
 *
 *  This header file provides the declaration of the class Metric.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   Metric
 *  @ingroup PEARL_base
 *  @brief   Stores information related to additional performance metrics.
 *
 *  The instances of the Metric class provide information about additional
 *  performance metrics, such as event counts, event rates, or sample
 *  values. Please also read the section on performance metrics in the
 *  EPILOG specification for further information.
 *
 *  The numerical identifiers of the individual metrics are globally defined
 *  and continuously enumerated, i.e., the ID is element of [0,@#metrics-1].
 **/
/*-------------------------------------------------------------------------*/

class Metric
{
    public:
        /// %Metric definition identifier type.
        typedef uint32_t IdType;


        /// @name Constructors & destructor
        /// @{

        Metric(IdType             id,
               const std::string& name,
               const std::string& description,
               const std::string& type,
               const std::string& mode,
               const std::string& interval);

        /// @}
        /// @name Get metric information
        /// @{

        IdType
        getId() const;

        std::string
        get_name() const;

        std::string
        get_description() const;

        std::string
        get_type() const;

        std::string
        get_mode() const;

        std::string
        get_interval() const;

        /// @}


    private:
        /// Global metric identifier
        IdType m_id;

        /// %Metric name
        std::string m_name;

        /// %Metric description
        std::string m_description;

        /// %Metric type, either "INTEGER" or "FLOAT"
        std::string m_type;

        /// %Metric mode, either "COUNTER", "RATE", or "SAMPLE"
        std::string m_mode;

        /// Measurement interval semantics, either "START", "LAST", "NEXT",
        /// or "NONE"
        std::string m_interval;
};
}    // namespace pearl


#endif    // !PEARL_METRIC_H
