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
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declarations of OTF2 callback functions for definition records.
 *
 *  This header file provides the declarations of OTF2 callback functions
 *  and related data types for handling global definition records.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_OTF2DEFCALLBACKS_H
#define PEARL_OTF2DEFCALLBACKS_H


#include <string>

#include <otf2/OTF2_AttributeValue.h>
#include <otf2/OTF2_Definitions.h>


namespace pearl
{
namespace detail
{
// --- Forward declarations -------------------------------------------------

class MutableGlobalDefs;


/*-------------------------------------------------------------------------*/
/**
 *  @struct  Otf2DefCbData
 *  @ingroup PEARL_base
 *  @brief   OTF2 definition callback data
 *
 *  The Otf2DefCbData struct combines all data elements which are necessary
 *  to process the global definition records of an OTF2 trace archive.
 **/
/*-------------------------------------------------------------------------*/

struct Otf2DefCbData
{
    // --- Public methods ----------------------------------

    /// @name Constructors & destructor
    /// @{

    /// @brief Constructor.
    ///
    /// Creates a new instance and initializes it with the given global
    /// definition object @a defs as well as default values for all other
    /// data members.
    ///
    /// @param defs
    ///     Global definitions object
    ///
    Otf2DefCbData(MutableGlobalDefs& defs)
        : mDefinitions(defs),
          mTimerResolution(0),
          mGlobalOffset(0)
    {
    }

    /// @}


    // --- Data members ------------------------------------

    /// Global definitions object
    MutableGlobalDefs& mDefinitions;

    /// %Error message of exceptions thrown (and caught) within callbacks
    std::string mErrorMessage;

    /// %Timer resolution in ticks per second
    uint64_t mTimerResolution;

    /// Estimated timestamp of the first event
    uint64_t mGlobalOffset;
};


// --- Function prototypes --------------------------------------------------

/// @name OTF2: Global definition callbacks
/// @{

/// @brief %Attribute definition callback
///
/// %Callback function that is triggered whenever an Attribute definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this Attribute definition
/// @param name
///     Name of the attribute (reference to String definition)
/// @param description
///     Description of the attribute (reference to String definition)
/// @param type
///     Data type of the attribute
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbAttribute(void*             userData,
                   OTF2_AttributeRef self,
                   OTF2_StringRef    name,
                   OTF2_StringRef    description,
                   OTF2_Type         type);

/// @brief %CallingContext definition callback
///
/// %Callback function that is triggered whenever a CallingContext definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this CallingContext definition
/// @param region
///     Corresponding source-code region (reference to Region definition)
/// @param scl
///     Source-code location of this calling context (reference to
///     SourceCodeLocation definition)
/// @param parent
///     Parent context (reference to CallingContext definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbCallingContext(void*                      userData,
                        OTF2_CallingContextRef     self,
                        OTF2_RegionRef             region,
                        OTF2_SourceCodeLocationRef scl,
                        OTF2_CallingContextRef     parent);

/// @brief %CallingContextProperty definition callback
///
/// %Callback function that is triggered whenever a CallingContextProperty
/// definition record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param callingContext
///     %CallingContext this property refers to (reference to CallingContext
///     definition)
/// @param name
///     Property name (reference to String definition)
/// @param type
///     Type of the property
/// @param value
///     Property value
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbCallingContextProperty(void*                  userData,
                                OTF2_CallingContextRef callingContext,
                                OTF2_StringRef         name,
                                OTF2_Type              type,
                                OTF2_AttributeValue    value);

/// @brief %Callpath definition callback
///
/// %Callback function that is triggered whenever a Callpath definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this Callpath definition
/// @param parent
///     Parent callpath (reference to Callpath definition)
/// @param region
///     %Region entered via this callpath (reference to Region definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbCallpath(void*            userData,
                  OTF2_CallpathRef self,
                  OTF2_CallpathRef parent,
                  OTF2_RegionRef   region);

/// @brief %Callsite definition callback
///
/// %Callback function that is triggered whenever a Callsite definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this Callsite definition
/// @param sourceFile
///     Source code file of the callsite (reference to String definition)
/// @param lineNumber
///     Line number of the callsite within @a sourceFile
/// @param enteredRegion
///     %Region which is called (reference to Region definition)
/// @param leftRegion
///     %Region in which the call is made (reference to Region definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbCallsite(void*            userData,
                  OTF2_CallsiteRef self,
                  OTF2_StringRef   sourceFile,
                  uint32_t         lineNumber,
                  OTF2_RegionRef   enteredRegion,
                  OTF2_RegionRef   leftRegion);

/// @brief %CartCoordinate definition callback
///
/// %Callback function that is triggered whenever a CartCoordinate definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param cartTopology
///     Topology this coordinate refers to (reference to CartTopology
///     definition)
/// @param rank
///     Rank w.r.t. the communicator associated to @a cartTopology this
///     coordinate refers to
/// @param numberOfDimensions
///     Number of coordinate dimensions
/// @param coordinates
///     Coordinates, indexed by dimensions
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbCartCoordinate(void*                userData,
                        OTF2_CartTopologyRef cartTopology,
                        uint32_t             rank,
                        uint8_t              numberOfDimensions,
                        const uint32_t*      coordinates);

/// @brief %CartDimension definition callback
///
/// %Callback function that is triggered whenever a CartDimension definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this CartDimension definition
/// @param name
///     Name of the topology dimension (reference to String definition)
/// @param size
///     Size of the dimension (number of elements)
/// @param cartPeriodicity
///     Periodicity of the dimension
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbCartDimension(void*                 userData,
                       OTF2_CartDimensionRef self,
                       OTF2_StringRef        name,
                       uint32_t              size,
                       OTF2_CartPeriodicity  cartPeriodicity);

/// @brief %CartTopology definition callback
///
/// %Callback function that is triggered whenever a CartTopology definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this CartTopology definition
/// @param name
///     Name of the topology (reference to String definition)
/// @param communicator
///     Associated communicator (reference to Communicator definition)
/// @param numberOfDimensions
///     Number of topology dimensions
/// @param cartDimensions
///     Dimensions of this topology (references to CartDimension definitions)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbCartTopology(void*                        userData,
                      OTF2_CartTopologyRef         self,
                      OTF2_StringRef               name,
                      OTF2_CommRef                 communicator,
                      uint8_t                      numberOfDimensions,
                      const OTF2_CartDimensionRef* cartDimensions);

/// @brief %ClockProperties definition callback
///
/// %Callback function that is triggered whenever a ClockProperties definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param timerResolution
///     %Timer resolution in ticks per second
/// @param globalOffset
///     Estimated timestamp of the first event (guaranteed to be less than or
///     equal to the real timestamp)
/// @param traceLength
///     Estimated timespan of the trace (guaranteed to be larger than or equal
///     to the real timespan)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbClockProperties(void*    userData,
                         uint64_t timerResolution,
                         uint64_t globalOffset,
                         uint64_t traceLength);

/// @brief %Communicator definition callback
///
/// %Callback function that is triggered whenever a Communicator definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this Communicator definition
/// @param name
///     Name of the communicator (reference to String definition)
/// @param group
///     Associated group (reference to Group definition)
/// @param parent
///     Parent communicator (reference to Communicator definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbComm(void*          userData,
              OTF2_CommRef   self,
              OTF2_StringRef name,
              OTF2_GroupRef  group,
              OTF2_CommRef   parent);

/// @brief %Group definition callback
///
/// %Callback function that is triggered whenever a Group definition record is
/// read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this Group definition
/// @param name
///     Name of the group (reference to String definition)
/// @param groupType
///     Type of this group
/// @param paradigm
///     Paradigm of this communication group
/// @param groupFlags
///     Additional flags characterizing this group
/// @param numberOfMembers
///     Number of group members
/// @param members
///     %Group members
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbGroup(void*           userData,
               OTF2_GroupRef   self,
               OTF2_StringRef  name,
               OTF2_GroupType  groupType,
               OTF2_Paradigm   paradigm,
               OTF2_GroupFlag  groupFlags,
               uint32_t        numberOfMembers,
               const uint64_t* members);

/// @brief %InterruptGenerator definition callback
///
/// %Callback function that is triggered whenever an InterruptGenerator
/// definition record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this InterruptGenerator definition
/// @param name
///     Name of this interrupt generator (reference to String definition)
/// @param mode
///     Mode of the interrupt generator (e.g., time or count based)
/// @param base
///     Base for the period calculation (binary, decimal)
/// @param exponent
///     Exponent for the period calculation
/// @param period
///     Periodicity of the generated interrupts
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbInterruptGenerator(void*                       userData,
                            OTF2_InterruptGeneratorRef  self,
                            OTF2_StringRef              name,
                            OTF2_InterruptGeneratorMode mode,
                            OTF2_Base                   base,
                            int64_t                     exponent,
                            uint64_t                    period);

/// @brief %Location definition callback
///
/// %Callback function that is triggered whenever a Location definition record
/// is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this Location definition
/// @param name
///     Name of the location (reference to String definition)
/// @param locationType
///     Type of this location
/// @param numberOfEvents
///     Number of events recorded for this location
/// @param locationGroup
///     Parent location group (reference to LocationGroup definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbLocation(void*                 userData,
                  OTF2_LocationRef      self,
                  OTF2_StringRef        name,
                  OTF2_LocationType     locationType,
                  uint64_t              numberOfEvents,
                  OTF2_LocationGroupRef locationGroup);

/// @brief %LocationGroup definition callback
///
/// %Callback function that is triggered whenever a LocationGroup definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this LocationGroup definition
/// @param name
///     Name of the location group (reference to String definition)
/// @param locationGroupType
///     Type of this location group
/// @param systemTreeParent
///     Parent node in system tree (reference to SystemTreeNode definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbLocationGroup(void*                  userData,
                       OTF2_LocationGroupRef  self,
                       OTF2_StringRef         name,
                       OTF2_LocationGroupType locationGroupType,
                       OTF2_SystemTreeNodeRef systemTreeParent);

/// @brief %LocationGroupProperty definition callback
///
/// %Callback function that is triggered whenever a LocationGroupProperty
/// definition record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param locationGroup
///     %Location group this property refers to (reference to LocationGroup
///     definition)
/// @param name
///     Property name (reference to String definition)
/// @param type
///     Type of the property
/// @param value
///     Property value
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbLocationGroupProperty(void*                 userData,
                               OTF2_LocationGroupRef locationGroup,
                               OTF2_StringRef        name,
                               OTF2_Type             type,
                               OTF2_AttributeValue   value);

/// @brief %LocationProperty definition callback
///
/// %Callback function that is triggered whenever a LocationProperty definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param location
///     %Location this property refers to (reference to Location definition)
/// @param name
///     Property name (reference to String definition)
/// @param type
///     Type of the property
/// @param value
///     Property value
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbLocationProperty(void*               userData,
                          OTF2_LocationRef    location,
                          OTF2_StringRef      name,
                          OTF2_Type           type,
                          OTF2_AttributeValue value);

/// @brief %MetricClass definition callback
///
/// %Callback function that is triggered whenever a MetricClass definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this MetricClass definition
/// @param numberOfMetrics
///     Number of metrics within the class
/// @param metricMembers
///     %Metric class members (references to MetricMember definitions)
/// @param metricOccurrence
///     Defines where metric event records for this metric class may occur
/// @param recorderKind
///     Defines what kind of locations recorded metrics of this class
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbMetricClass(void*                       userData,
                     OTF2_MetricRef              self,
                     uint8_t                     numberOfMetrics,
                     const OTF2_MetricMemberRef* metricMembers,
                     OTF2_MetricOccurrence       metricOccurrence,
                     OTF2_RecorderKind           recorderKind);

/// @brief %MetricClassRecorder definition callback
///
/// %Callback function that is triggered whenever a MetricClassRecorder
/// definition record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param metricClass
///     %Metric class this recorder definition refers to (reference to
///     MetricClass definition)
/// @param recorder
///     %Location which recorded the metric data (reference to Location
///     definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbMetricClassRecorder(void*            userData,
                             OTF2_MetricRef   metricClass,
                             OTF2_LocationRef recorder);

/// @brief %MetricInstance definition callback
///
/// %Callback function that is triggered whenever a MetricInstance definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this MetricInstance definition
/// @param metricClass
///     Associated metric class (reference to MetricClass definition)
/// @param recorder
///     %Location which recorded the metric data (reference to Location
///     definition)
/// @param metricScope
///     Type of the metric scope
/// @param scope
///     %Metric scope (reference to Location, LocationGroup, SystemTreeNode,
///     or generic Group definition, depending on @a metricScope)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbMetricInstance(void*            userData,
                        OTF2_MetricRef   self,
                        OTF2_MetricRef   metricClass,
                        OTF2_LocationRef recorder,
                        OTF2_MetricScope metricScope,
                        uint64_t         scope);

/// @brief %MetricMember definition callback
///
/// %Callback function that is triggered whenever a MetricMember definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this MetricMember definition
/// @param name
///     Name of the metric (reference to String definition)
/// @param description
///     Description of the metric (reference to String definition)
/// @param metricType
///     Type of the metric
/// @param metricMode
///     Measurement mode of the metric
/// @param valueType
///     Data type of the metric
/// @param base
///     %Metric base (binary, decimal)
/// @param exponent
///     Exponent of scaling factor w.r.t. @a metricBase
/// @param unit
///     Unit of the metric (reference to String definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbMetricMember(void*                userData,
                      OTF2_MetricMemberRef self,
                      OTF2_StringRef       name,
                      OTF2_StringRef       description,
                      OTF2_MetricType      metricType,
                      OTF2_MetricMode      metricMode,
                      OTF2_Type            valueType,
                      OTF2_Base            base,
                      int64_t              exponent,
                      OTF2_StringRef       unit);

/// @brief %Paradigm definition callback
///
/// %Callback function that is triggered whenever a Paradigm definition record
/// is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param paradigm
///     %Paradigm this definition refers to
/// @param name
///     Name of the paradigm (reference to String definition)
/// @param paradigmClass
///     Classification of this paradigm
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbParadigm(void*              userData,
                  OTF2_Paradigm      paradigm,
                  OTF2_StringRef     name,
                  OTF2_ParadigmClass paradigmClass);

/// @brief %ParadigmProperty definition callback
///
/// %Callback function that gets triggered whenever a ParadigmProperty
/// definition record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided data
/// @param paradigm
///     %Paradigm this property refers to (reference to Paradigm definition)
/// @param property
///     Property of this paradigm
/// @param type
///     Type of the property
/// @param value
///     Property value
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbParadigmProperty(void*                 userData,
                          OTF2_Paradigm         paradigm,
                          OTF2_ParadigmProperty property,
                          OTF2_Type             type,
                          OTF2_AttributeValue   value);

/// @brief %Parameter definition callback
///
/// %Callback function that is triggered whenever a Parameter definition record
/// is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this Parameter definition
/// @param name
///     Name of the parameter (reference to String definition)
/// @param parameterType
///     Data type of the parameter
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbParameter(void*              userData,
                   OTF2_ParameterRef  self,
                   OTF2_StringRef     name,
                   OTF2_ParameterType parameterType);

/// @brief %Region definition callback
///
/// %Callback function that is triggered whenever a Region definition record is
/// read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this Region definition
/// @param name
///     Name of the region, demangled if available (reference to String
///     definition)
/// @param canonicalName
///     Name of the region, typically mangled (reference to String definition)
/// @param description
///     Description of the region (reference to String definition)
/// @param regionRole
///     Role of the region
/// @param paradigm
///     Paradigm of the region
/// @param regionFlags
///     Additional flags characterizing this region
/// @param sourceFile
///     Source code file in which the region is defined (reference to String
///     definition)
/// @param beginLineNumber
///     Line number where the region starts w.r.t. @a sourceFile
/// @param endLineNumber
///     Line number where the region ends w.r.t. @a sourceFile
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbRegion(void*           userData,
                OTF2_RegionRef  self,
                OTF2_StringRef  name,
                OTF2_StringRef  canonicalName,
                OTF2_StringRef  description,
                OTF2_RegionRole regionRole,
                OTF2_Paradigm   paradigm,
                OTF2_RegionFlag regionFlags,
                OTF2_StringRef  sourceFile,
                uint32_t        beginLineNumber,
                uint32_t        endLineNumber);

/// @brief %RmaWin definition callback
///
/// %Callback function that is triggered whenever a RmaWin definition record is
/// read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this RmaWin definition
/// @param name
///     Name of the RMA window (reference to String definition)
/// @param comm
///     Associated communicator (reference to Communicator definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbRmaWin(void*          userData,
                OTF2_RmaWinRef self,
                OTF2_StringRef name,
                OTF2_CommRef   comm);

/// @brief %SourceCodeLocation definition callback
///
/// %Callback function that is triggered whenever a SourceCodeLocation
/// definition record is read from the global definition file.
///
/// @param userData
///      Pointer to user-provided callback data
/// @param self
///     Unique identifier for this SourceCodeLocation definition
/// @param file
///     Source code file (reference to String definition)
/// @param lineNumber
///     Line number within @a file
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbSourceCodeLocation(void*                      userData,
                            OTF2_SourceCodeLocationRef self,
                            OTF2_StringRef             file,
                            uint32_t                   lineNumber);

/// @brief %String definition callback
///
/// %Callback function that is triggered whenever a String definition record is
/// read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this String definition
/// @param str
///     Null-terminated character string
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbString(void*          userData,
                OTF2_StringRef self,
                const char*    str);

/// @brief %SystemTreeNode definition callback
///
/// %Callback function that is triggered whenever a SystemTreeNode definition
/// record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param self
///     Unique identifier for this SystemTreeNode definition
/// @param name
///     Name of this system tree node (reference to String definition)
/// @param className
///     Free-form class name (reference to String definition)
/// @param parent
///     Parent node in system tree (reference to SystemTreeNode definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbSystemTreeNode(void*                  userData,
                        OTF2_SystemTreeNodeRef self,
                        OTF2_StringRef         name,
                        OTF2_StringRef         className,
                        OTF2_SystemTreeNodeRef parent);

/// @brief %SystemTreeNodeDomain definition callback
///
/// %Callback function that is triggered whenever a SystemTreeNodeDomain
/// definition record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param systemTreeNode
///     System tree node this domain refers to (reference to SystemTreeNode
///     definition)
/// @param systemTreeDomain
///     Domain specification
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbSystemTreeNodeDomain(void*                  userData,
                              OTF2_SystemTreeNodeRef systemTreeNode,
                              OTF2_SystemTreeDomain  systemTreeDomain);

/// @brief %SystemTreeNodeProperty definition callback
///
/// %Callback function that is triggered whenever a SystemTreeNodeProperty
/// definition record is read from the global definition file.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param systemTreeNode
///     System tree node this property refers to (reference to SystemTreeNode
///     definition)
/// @param name
///     Property name (reference to String definition)
/// @param type
///     Type of the property
/// @param value
///     Property value
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2DefCbSystemTreeNodeProperty(void*                  userData,
                                OTF2_SystemTreeNodeRef systemTreeNode,
                                OTF2_StringRef         name,
                                OTF2_Type              type,
                                OTF2_AttributeValue    value);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_OTF2DEFCALLBACKS_H
