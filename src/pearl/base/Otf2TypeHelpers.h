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
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declarations of OTF2 helper and type conversion functions.
 *
 *  This header file provides the declarations of helper functions to
 *  convert OTF2 data types into equivalent PEARL data types.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_OTF2TYPEHELPERS_H
#define PEARL_OTF2TYPEHELPERS_H


#include <string>

#include <otf2/OTF2_Definitions.h>
#include <otf2/OTF2_Events.h>

#include <pearl/Location.h>
#include <pearl/LocationGroup.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/Paradigm.h>
#include <pearl/Region.h>

#include "pearl_ipc.h"


namespace pearl
{
namespace detail
{
// --- Forward declarations -------------------------------------------------

struct Otf2EvtCbData;


/// @name OTF2 type conversion helper functions
/// @{

/// @brief Conversion of a paradigm from OTF2 to PEARL
///
/// %Helper function that converts an OTF2 paradigm to a valid PEARL
/// paradigm.
///
/// @param paradigm
///     OTF2 paradigm
/// @returns
///     Corresponding PEARL paradigm if successful, Paradigm::UNKNOWN otherwise
///
Paradigm
otf2ConvertParadigm(OTF2_Paradigm paradigm);

/// @brief Conversion of a timestamp from OTF2 to PEARL
///
/// %Helper function that converts an OTF2 timestamp to a valid PEARL
/// timestamp.
///
/// @param timestamp
///     %Event timestamp in ticks
/// @param data
///     OTF2 event callback data providing timer information
/// @returns
///     Calculated time as pearl::timestamp_t in seconds
///
timestamp_t
otf2ConvertTime(uint64_t             timestamp,
                const Otf2EvtCbData* data);

/// @brief Conversion of a collective type from OTF2 to PEARL
///
/// %Helper function that converts an OTF2 collective type to a valid PEARL
/// collective type.
///
/// @param type
///     OTF2 collective operation type
/// @returns
///     Corresponding PEARL collective operation type if successful,
///     MpiCollEnd_rep::NUMBER_OF_COLL_TYPES otherwise
///
MpiCollEnd_rep::coll_type
otf2ConvertCollectiveType(OTF2_CollectiveOp type);

/// @brief Conversion of a location type from OTF2 to PEARL
///
/// %Helper function that converts an OTF2 location type to a valid PEARL
/// location type.
///
/// @param type
///     OTF2 location type
/// @returns
///     Corresponding PEARL location type if successful, Location::TYPE_UNKNOWN
///     otherwise
///
Location::Type
otf2ConvertLocationType(OTF2_LocationType type);

/// @brief Conversion of a location group type from OTF2 to PEARL
///
/// %Helper function that converts an OTF2 location group type to a valid
/// PEARL location group type.
///
/// @param type
///     OTF2 location group type
/// @returns
///     Corresponding PEARL location group type if successful,
///     LocationGroup::TYPE_UNKNOWN otherwise
///
LocationGroup::Type
otf2ConvertLocationGroupType(OTF2_LocationGroupType type);

/// @brief Conversion of a region paradigm from OTF2 to PEARL
///
/// %Helper function that converts an OTF2 region paradigm to a valid PEARL
/// paradigm.
///
/// @param paradigm
///     OTF2 paradigm
/// @param role
///     OTF2 region role
/// @param name
///     Region name
/// @returns
///     Corresponding PEARL paradigm if successful, Paradigm::UNKNOWN otherwise
///
Paradigm
otf2ConvertRegionParadigm(OTF2_Paradigm      paradigm,
                          OTF2_RegionRole    role,
                          const std::string& name);

/// @brief Conversion of a region role from OTF2 to PEARL
///
/// %Helper function that converts an OTF2 region role to a valid
/// PEARL region role.
///
/// @param paradigm
///     OTF2 paradigm
/// @param role
///     OTF2 region role
/// @param name
///     Region name
/// @returns
///     Corresponding PEARL region role if successful, Region::ROLE_UNKNOWN
///     otherwise
///
Region::Role
otf2ConvertRegionRole(OTF2_Paradigm      paradigm,
                      OTF2_RegionRole    role,
                      const std::string& name);

/// @brief Conversion of an OTF2 data type to a PEARL inter-process
///        communication type
///
/// %Helper function that converts an OTF2 data type to a valid PEARL
/// inter-process communication data type.
///
/// @param type
///     OTF2 type
/// @returns
///     Corresponding PEARL IpcDatatype if successful, PEARL_UINT8_T otherwise
///
IpcDatatype
otf2GetIpcDatatype(const OTF2_Type type);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_OTF2TYPEHELPERS_H
