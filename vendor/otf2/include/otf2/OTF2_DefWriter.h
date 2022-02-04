/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_DEF_WRITER_H
#define OTF2_DEF_WRITER_H


/**
 *  @file
 *  @source     templates/OTF2_DefWriter.tmpl.h
 *
 *  @brief      This file provides all routines that write definition records
 *              of a single location.
 */


#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>
#include <otf2/OTF2_AttributeValue.h>
#include <otf2/OTF2_Definitions.h>
#include <otf2/OTF2_IdMap.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Handle definition for the external definition writer. */
typedef struct OTF2_DefWriter_struct OTF2_DefWriter;


/** @brief
 *  Returns the location ID of the location which is related to
 *  the writer object.
 *
 *  @param writer   Writer object.
 *  @param location Return location reference.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_GetLocationID( const OTF2_DefWriter* writer,
                              OTF2_LocationRef*     location );


/** @brief Writes a MappingTable definition record into the DefWriter.
 *
 *  Mapping tables are needed for situations where an ID is not globally
 *  known at measurement time. They are applied automatically at
 *  reading.
 *
 *  @param writer      Writer object.
 *  @param mappingType Says to what type of ID the mapping table has to be
 *                     applied.
 *  @param idMap       Mapping table.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteMappingTable( OTF2_DefWriter*   writer,
                                  OTF2_MappingType  mappingType,
                                  const OTF2_IdMap* idMap );


/** @brief Writes a ClockOffset definition record into the DefWriter.
 *
 *  Clock offsets are used for clock corrections.
 *
 *  @param writer            Writer object.
 *  @param time              Time when this offset was determined.
 *  @param offset            The offset to the global clock which was determined
 *                           at @p time.
 *  @param standardDeviation A possible standard deviation, which can be used as
 *                           a metric for the quality of the offset.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteClockOffset( OTF2_DefWriter* writer,
                                 OTF2_TimeStamp  time,
                                 int64_t         offset,
                                 double          standardDeviation );


/** @brief Writes a String definition record into the DefWriter.
 *
 *  The string definition.
 *
 *  @param writer Writer object.
 *  @param self   The unique identifier for this @eref{String} definition.
 *  @param string The string, null terminated.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteString( OTF2_DefWriter* writer,
                            OTF2_StringRef  self,
                            const char*     string );


/** @brief Writes a Attribute definition record into the DefWriter.
 *
 *  The attribute definition.
 *
 *  @param writer      Writer object.
 *  @param self        The unique identifier for this @eref{Attribute}
 *                     definition.
 *  @param name        Name of the attribute. References a @eref{String}
 *                     definition.
 *  @param description Description of the attribute. References a @eref{String}
 *                     definition. Since version 1.4.
 *  @param type        Type of the attribute value.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteAttribute( OTF2_DefWriter*   writer,
                               OTF2_AttributeRef self,
                               OTF2_StringRef    name,
                               OTF2_StringRef    description,
                               OTF2_Type         type );


/** @brief Writes a SystemTreeNode definition record into the DefWriter.
 *
 *  The system tree node definition.
 *
 *  @param writer    Writer object.
 *  @param self      The unique identifier for this @eref{SystemTreeNode}
 *                   definition.
 *  @param name      Free form instance name of this node. References a
 *                   @eref{String} definition.
 *  @param className Free form class name of this node References a @eref{String}
 *                   definition.
 *  @param parent    Parent id of this node. May be
 *                   @eref{OTF2_UNDEFINED_SYSTEM_TREE_NODE} to indicate that
 *                   there is no parent. References a @eref{SystemTreeNode}
 *                   definition.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteSystemTreeNode( OTF2_DefWriter*        writer,
                                    OTF2_SystemTreeNodeRef self,
                                    OTF2_StringRef         name,
                                    OTF2_StringRef         className,
                                    OTF2_SystemTreeNodeRef parent );


/** @brief Writes a LocationGroup definition record into the DefWriter.
 *
 *  The location group definition.
 *
 *  @param writer            Writer object.
 *  @param self              The unique identifier for this @eref{LocationGroup}
 *                           definition.
 *  @param name              Name of the group. References a @eref{String}
 *                           definition.
 *  @param locationGroupType Type of this group.
 *  @param systemTreeParent  Parent of this location group in the system tree.
 *                           References a @eref{SystemTreeNode} definition.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteLocationGroup( OTF2_DefWriter*        writer,
                                   OTF2_LocationGroupRef  self,
                                   OTF2_StringRef         name,
                                   OTF2_LocationGroupType locationGroupType,
                                   OTF2_SystemTreeNodeRef systemTreeParent );


/** @brief Writes a Location definition record into the DefWriter.
 *
 *  The location definition.
 *
 *  @param writer         Writer object.
 *  @param self           The unique identifier for this @eref{Location}
 *                        definition.
 *  @param name           Name of the location References a @eref{String}
 *                        definition.
 *  @param locationType   Location type.
 *  @param numberOfEvents Number of events this location has recorded.
 *  @param locationGroup  Location group which includes this location. References
 *                        a @eref{LocationGroup} definition.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteLocation( OTF2_DefWriter*       writer,
                              OTF2_LocationRef      self,
                              OTF2_StringRef        name,
                              OTF2_LocationType     locationType,
                              uint64_t              numberOfEvents,
                              OTF2_LocationGroupRef locationGroup );


/** @brief Writes a Region definition record into the DefWriter.
 *
 *  The region definition.
 *
 *  @param writer          Writer object.
 *  @param self            The unique identifier for this @eref{Region}
 *                         definition.
 *  @param name            Name of the region (demangled name if available).
 *                         References a @eref{String} definition.
 *  @param canonicalName   Alternative name of the region (e.g. mangled name).
 *                         References a @eref{String} definition. Since
 *                         version 1.1.
 *  @param description     A more detailed description of this region. References
 *                         a @eref{String} definition.
 *  @param regionRole      Region role. Since version 1.1.
 *  @param paradigm        Paradigm. Since version 1.1.
 *  @param regionFlags     Region flags. Since version 1.1.
 *  @param sourceFile      The source file where this region was declared.
 *                         References a @eref{String} definition.
 *  @param beginLineNumber Starting line number of this region in the source
 *                         file.
 *  @param endLineNumber   Ending line number of this region in the source file.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteRegion( OTF2_DefWriter* writer,
                            OTF2_RegionRef  self,
                            OTF2_StringRef  name,
                            OTF2_StringRef  canonicalName,
                            OTF2_StringRef  description,
                            OTF2_RegionRole regionRole,
                            OTF2_Paradigm   paradigm,
                            OTF2_RegionFlag regionFlags,
                            OTF2_StringRef  sourceFile,
                            uint32_t        beginLineNumber,
                            uint32_t        endLineNumber );


/** @brief Writes a Callsite definition record into the DefWriter.
 *
 *  The callsite definition.
 *
 *  @param writer        Writer object.
 *  @param self          The unique identifier for this @eref{Callsite}
 *                       definition.
 *  @param sourceFile    The source file where this call was made. References a
 *                       @eref{String} definition.
 *  @param lineNumber    Line number in the source file where this call was made.
 *  @param enteredRegion The region which was called. References a @eref{Region}
 *                       definition.
 *  @param leftRegion    The region which made the call. References a
 *                       @eref{Region} definition.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 2.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteCallsite( OTF2_DefWriter*  writer,
                              OTF2_CallsiteRef self,
                              OTF2_StringRef   sourceFile,
                              uint32_t         lineNumber,
                              OTF2_RegionRef   enteredRegion,
                              OTF2_RegionRef   leftRegion );


/** @brief Writes a Callpath definition record into the DefWriter.
 *
 *  The callpath definition.
 *
 *  @param writer Writer object.
 *  @param self   The unique identifier for this @eref{Callpath} definition.
 *  @param parent The parent of this callpath. References a @eref{Callpath}
 *                definition.
 *  @param region The region of this callpath. References a @eref{Region}
 *                definition.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteCallpath( OTF2_DefWriter*  writer,
                              OTF2_CallpathRef self,
                              OTF2_CallpathRef parent,
                              OTF2_RegionRef   region );


/** @brief Writes a Group definition record into the DefWriter.
 *
 *  The group definition.
 *
 *  @param writer          Writer object.
 *  @param self            The unique identifier for this @eref{Group}
 *                         definition.
 *  @param name            Name of this group References a @eref{String}
 *                         definition.
 *  @param groupType       The type of this group. Since version 1.2.
 *  @param paradigm        The paradigm of this communication group. Since
 *                         version 1.2.
 *  @param groupFlags      Flags for this group. Since version 1.2.
 *  @param numberOfMembers The number of members in this group.
 *  @param members         The identifiers of the group members.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteGroup( OTF2_DefWriter* writer,
                           OTF2_GroupRef   self,
                           OTF2_StringRef  name,
                           OTF2_GroupType  groupType,
                           OTF2_Paradigm   paradigm,
                           OTF2_GroupFlag  groupFlags,
                           uint32_t        numberOfMembers,
                           const uint64_t* members );


/** @brief Writes a MetricMember definition record into the DefWriter.
 *
 *  A metric is defined by a metric member definition. A metric member is
 *  always a member of a metric class. Therefore, a single metric is a
 *  special case of a metric class with only one member. It is not
 *  allowed to reference a metric member id in a metric event, but
 *  only metric class IDs.
 *
 *  @param writer      Writer object.
 *  @param self        The unique identifier for this @eref{MetricMember}
 *                     definition.
 *  @param name        Name of the metric. References a @eref{String} definition.
 *  @param description Description of the metric. References a @eref{String}
 *                     definition.
 *  @param metricType  Metric type: PAPI, etc.
 *  @param metricMode  Metric mode: accumulative, fix, relative, etc.
 *  @param valueType   Type of the value. Only @eref{OTF2_TYPE_INT64},
 *                     @eref{OTF2_TYPE_UINT64}, and @eref{OTF2_TYPE_DOUBLE}
 *                     are valid types. If this metric member is recorded in
 *                     a @eref{Metric} event, than this type and the type in
 *                     the event must match.
 *  @param base        The recorded values should be handled in this given base,
 *                     either binary or decimal. This information can be used
 *                     if the value needs to be scaled.
 *  @param exponent    The values inside the Metric events should be scaled by
 *                     the factor base^exponent, to get the value in its base
 *                     unit. For example, if the metric values come in as
 *                     KiBi, than the base should be @eref{OTF2_BASE_BINARY}
 *                     and the exponent 10. Than the writer does not need to
 *                     scale the values up to bytes, but can directly write
 *                     the KiBi values into the Metric event. At reading
 *                     time, the reader can apply the scaling factor to get
 *                     the value in its base unit, ie. in bytes.
 *  @param unit        Unit of the metric. This needs to be the scale free base
 *                     unit, ie. "bytes", "operations", or "seconds". In
 *                     particular this unit should not have any scale prefix.
 *                     References a @eref{String} definition.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteMetricMember( OTF2_DefWriter*      writer,
                                  OTF2_MetricMemberRef self,
                                  OTF2_StringRef       name,
                                  OTF2_StringRef       description,
                                  OTF2_MetricType      metricType,
                                  OTF2_MetricMode      metricMode,
                                  OTF2_Type            valueType,
                                  OTF2_Base            base,
                                  int64_t              exponent,
                                  OTF2_StringRef       unit );


/** @brief Writes a MetricClass definition record into the DefWriter.
 *
 *  For a metric class it is implicitly given that the event stream that
 *  records the metric is also the scope. A metric class can contain
 *  multiple different metrics.
 *
 *  @param writer           Writer object.
 *  @param self             The unique identifier for this @eref{MetricClass}
 *                          definition.
 *  @param numberOfMetrics  Number of metrics within the set.
 *  @param metricMembers    List of metric members. References a
 *                          @eref{MetricMember} definition.
 *  @param metricOccurrence Defines occurrence of a metric set.
 *  @param recorderKind     What kind of locations will record this metric class,
 *                          or will this metric class only be recorded by
 *                          metric instances. Since version 1.2.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteMetricClass( OTF2_DefWriter*             writer,
                                 OTF2_MetricRef              self,
                                 uint8_t                     numberOfMetrics,
                                 const OTF2_MetricMemberRef* metricMembers,
                                 OTF2_MetricOccurrence       metricOccurrence,
                                 OTF2_RecorderKind           recorderKind );


/** @brief Writes a MetricInstance definition record into the DefWriter.
 *
 *  A metric instance is used to define metrics that are recorded at one
 *  location for multiple locations or for another location. The
 *  occurrence of a metric instance is implicitly of type
 *  @eref{OTF2_METRIC_ASYNCHRONOUS}.
 *
 *  @param writer      Writer object.
 *  @param self        The unique identifier for this @eref{MetricClass}
 *                     definition.
 *  @param metricClass The instanced @eref{MetricClass}. This metric class must
 *                     be of kind @eref{OTF2_RECORDER_KIND_ABSTRACT}.
 *                     References a @eref{MetricClass}, or a
 *                     @eref{MetricInstance} definition.
 *  @param recorder    Recorder of the metric: location ID. References a
 *                     @eref{Location} definition.
 *  @param metricScope Defines type of scope: location, location group, system
 *                     tree node, or a generic group of locations.
 *  @param scope       Scope of metric: ID of a location, location group, system
 *                     tree node, or a generic group of locations.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteMetricInstance( OTF2_DefWriter*  writer,
                                    OTF2_MetricRef   self,
                                    OTF2_MetricRef   metricClass,
                                    OTF2_LocationRef recorder,
                                    OTF2_MetricScope metricScope,
                                    uint64_t         scope );


/** @brief Writes a Comm definition record into the DefWriter.
 *
 *  The communicator definition.
 *
 *  @param writer Writer object.
 *  @param self   The unique identifier for this @eref{Comm} definition.
 *  @param name   The name given by calling MPI_Comm_set_name on this
 *                communicator. Or the empty name to indicate that no name
 *                was given. References a @eref{String} definition.
 *  @param group  The describing MPI group of this MPI communicator  The group
 *                needs to be of type @eref{OTF2_GROUP_TYPE_COMM_GROUP} or
 *                @eref{OTF2_GROUP_TYPE_COMM_SELF}. References a @eref{Group}
 *                definition.
 *  @param parent The parent MPI communicator from which this communicator was
 *                created, if any. Use @eref{OTF2_UNDEFINED_COMM} to indicate
 *                no parent. References a @eref{Comm} definition.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteComm( OTF2_DefWriter* writer,
                          OTF2_CommRef    self,
                          OTF2_StringRef  name,
                          OTF2_GroupRef   group,
                          OTF2_CommRef    parent );


/** @brief Writes a Parameter definition record into the DefWriter.
 *
 *  The parameter definition.
 *
 *  @param writer        Writer object.
 *  @param self          The unique identifier for this @eref{Parameter}
 *                       definition.
 *  @param name          Name of the parameter (variable name etc.) References a
 *                       @eref{String} definition.
 *  @param parameterType Type of the parameter, @eref{OTF2_ParameterType} for
 *                       possible types.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteParameter( OTF2_DefWriter*    writer,
                               OTF2_ParameterRef  self,
                               OTF2_StringRef     name,
                               OTF2_ParameterType parameterType );


/** @brief Writes a RmaWin definition record into the DefWriter.
 *
 *  A window defines the communication context for any remote-memory
 *  access operation.
 *
 *  @param writer Writer object.
 *  @param self   The unique identifier for this @eref{RmaWin} definition.
 *  @param name   Name, e.g. 'GASPI Queue 1', 'NVidia Card 2', etc.. References a
 *                @eref{String} definition.
 *  @param comm   Communicator object used to create the window. References a
 *                @eref{Comm} definition.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteRmaWin( OTF2_DefWriter* writer,
                            OTF2_RmaWinRef  self,
                            OTF2_StringRef  name,
                            OTF2_CommRef    comm );


/** @brief Writes a MetricClassRecorder definition record into the DefWriter.
 *
 *  The metric class recorder definition.
 *
 *  @param writer   Writer object.
 *  @param metric   Parent @eref{MetricClass}, or @eref{MetricInstance}
 *                  definition to which this one is a supplementary
 *                  definition. References a @eref{MetricClass}, or a
 *                  @eref{MetricInstance} definition.
 *  @param recorder The location which recorded the referenced metric class.
 *                  References a @eref{Location} definition.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteMetricClassRecorder( OTF2_DefWriter*  writer,
                                         OTF2_MetricRef   metric,
                                         OTF2_LocationRef recorder );


/** @brief Writes a SystemTreeNodeProperty definition record into the DefWriter.
 *
 *  An arbitrary key/value property for a @eref{SystemTreeNode}
 *  definition.
 *
 *  @param writer         Writer object.
 *  @param systemTreeNode Parent @eref{SystemTreeNode} definition to which this
 *                        one is a supplementary definition. References a
 *                        @eref{SystemTreeNode} definition.
 *  @param name           Name of the property. References a @eref{String}
 *                        definition.
 *  @param type           The type of this property. Since version 2.0.
 *  @param value          The value of this property. Since version 2.0.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteSystemTreeNodeProperty( OTF2_DefWriter*        writer,
                                            OTF2_SystemTreeNodeRef systemTreeNode,
                                            OTF2_StringRef         name,
                                            OTF2_Type              type,
                                            OTF2_AttributeValue    value );


/** @brief Writes a SystemTreeNodeDomain definition record into the DefWriter.
 *
 *  The system tree node domain definition.
 *
 *  @param writer           Writer object.
 *  @param systemTreeNode   Parent @eref{SystemTreeNode} definition to which this
 *                          one is a supplementary definition. References a
 *                          @eref{SystemTreeNode} definition.
 *  @param systemTreeDomain The domain in which the referenced
 *                          @eref{SystemTreeNode} operates in.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteSystemTreeNodeDomain( OTF2_DefWriter*        writer,
                                          OTF2_SystemTreeNodeRef systemTreeNode,
                                          OTF2_SystemTreeDomain  systemTreeDomain );


/** @brief Writes a LocationGroupProperty definition record into the DefWriter.
 *
 *  An arbitrary key/value property for a @eref{LocationGroup} definition.
 *
 *  @param writer        Writer object.
 *  @param locationGroup Parent @eref{LocationGroup} definition to which this one
 *                       is a supplementary definition. References a
 *                       @eref{LocationGroup} definition.
 *  @param name          Name of the property. References a @eref{String}
 *                       definition.
 *  @param type          The type of this property. Since version 2.0.
 *  @param value         The value of this property. Since version 2.0.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteLocationGroupProperty( OTF2_DefWriter*       writer,
                                           OTF2_LocationGroupRef locationGroup,
                                           OTF2_StringRef        name,
                                           OTF2_Type             type,
                                           OTF2_AttributeValue   value );


/** @brief Writes a LocationProperty definition record into the DefWriter.
 *
 *  An arbitrary key/value property for a @eref{Location} definition.
 *
 *  @param writer      Writer object.
 *  @param location    Parent @eref{Location} definition to which this one is a
 *                     supplementary definition. References a @eref{Location}
 *                     definition.
 *  @param name        Name of the property. References a @eref{String}
 *                     definition.
 *  @param type        The type of this property. Since version 2.0.
 *  @param value       The value of this property. Since version 2.0.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteLocationProperty( OTF2_DefWriter*     writer,
                                      OTF2_LocationRef    location,
                                      OTF2_StringRef      name,
                                      OTF2_Type           type,
                                      OTF2_AttributeValue value );


/** @brief Writes a CartDimension definition record into the DefWriter.
 *
 *  Each dimension in a Cartesian topology is composed of a global id, a
 *  name, its size, and whether it is periodic or not.
 *
 *  @param writer          Writer object.
 *  @param self            The unique identifier for this @eref{CartDimension}
 *                         definition.
 *  @param name            The name of the cartesian topology dimension.
 *                         References a @eref{String} definition.
 *  @param size            The size of the cartesian topology dimension.
 *  @param cartPeriodicity Periodicity of the cartesian topology dimension.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteCartDimension( OTF2_DefWriter*       writer,
                                   OTF2_CartDimensionRef self,
                                   OTF2_StringRef        name,
                                   uint32_t              size,
                                   OTF2_CartPeriodicity  cartPeriodicity );


/** @brief Writes a CartTopology definition record into the DefWriter.
 *
 *  Each topology is described by a global id, a reference to its name, a
 *  reference to a communicator, the number of dimensions, and
 *  references to those dimensions. The topology type is defined by
 *  the paradigm of the group referenced by the associated
 *  communicator.
 *
 *  @param writer             Writer object.
 *  @param self               The unique identifier for this @eref{CartTopology}
 *                            definition.
 *  @param name               The name of the topology. References a
 *                            @eref{String} definition.
 *  @param communicator       Communicator object used to create the topology.
 *                            References a @eref{Comm} definition.
 *  @param numberOfDimensions Number of dimensions.
 *  @param cartDimensions     The dimensions of this topology. References a
 *                            @eref{CartDimension} definition.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteCartTopology( OTF2_DefWriter*              writer,
                                  OTF2_CartTopologyRef         self,
                                  OTF2_StringRef               name,
                                  OTF2_CommRef                 communicator,
                                  uint8_t                      numberOfDimensions,
                                  const OTF2_CartDimensionRef* cartDimensions );


/** @brief Writes a CartCoordinate definition record into the DefWriter.
 *
 *  Defines the coordinate of the location referenced by the given rank
 *  (w.r.t. the communicator associated to the topology) in the
 *  referenced topology.
 *
 *  @param writer             Writer object.
 *  @param cartTopology       Parent @eref{CartTopology} definition to which this
 *                            one is a supplementary definition. References a
 *                            @eref{CartTopology} definition.
 *  @param rank               The rank w.r.t. the communicator associated to the
 *                            topology referencing this coordinate.
 *  @param numberOfDimensions Number of dimensions.
 *  @param coordinates        Coordinates, indexed by dimension.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteCartCoordinate( OTF2_DefWriter*      writer,
                                    OTF2_CartTopologyRef cartTopology,
                                    uint32_t             rank,
                                    uint8_t              numberOfDimensions,
                                    const uint32_t*      coordinates );


/** @brief Writes a SourceCodeLocation definition record into the DefWriter.
 *
 *  The definition of a source code location as tuple of the corresponding
 *  file name and line number.
 *
 *  When used to attach source code annotations to events, use the
 *  @eref{OTF2_AttributeList} with a @eref{Attribute} definition named
 *  @c "SOURCE_CODE_LOCATION" and typed
 *  @eref{OTF2_TYPE_SOURCE_CODE_LOCATION}.
 *
 *  @param writer     Writer object.
 *  @param self       The unique identifier for this @eref{SourceCodeLocation}
 *                    definition.
 *  @param file       The name of the file for the source code location.
 *                    References a @eref{String} definition.
 *  @param lineNumber The line number for the source code location.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteSourceCodeLocation( OTF2_DefWriter*            writer,
                                        OTF2_SourceCodeLocationRef self,
                                        OTF2_StringRef             file,
                                        uint32_t                   lineNumber );


/** @brief Writes a CallingContext definition record into the DefWriter.
 *
 *  Defines a node in the calling context tree. These nodes are referenced
 *  in the @eref{CallingContextSample}, @eref{CallingContextEnter},
 *  and @eref{CallingContextLeave} events.
 *
 *  The referenced @eref{CallingContext} node in these events form a path
 *  which represents the calling context at this time.  This path will
 *  be partitioned into at most three sub-paths by the @a
 *  unwindDistance attribute.  For the @eref{CallingContextLeave}
 *  event, the @a unwindDistance is defined to be 1.
 *
 *  Starting from the referenced @eref{CallingContext} node, the first @f$
 *  N \ge 0 @f$ nodes were newly entered regions since the previous
 *  calling context event.  The next node is a region which was not
 *  left but made progress since the previous calling context event.
 *  All other nodes did not make progress at all, and thus the regions
 *  were neither left nor entered again.  The @a unwindDistance is
 *  then @f$ N + 1 @f$. In case the @p unwindDistance is @f$ 0 @f$,
 *  there are neither newly entered regions nor regions which made
 *  progress.
 *
 *  It is guaranteed, that the node referenced by the @a unwindDistance
 *  exists in the previous and current calling context.  All
 *  descendants of this node's child in the previous calling context
 *  were left since the previous calling context event.
 *
 *  It is valid that this node is the
 *  @eref{OTF2_UNDEFINED_CALLING_CONTEXT} node and that this node is
 *  already reached after @a unwindDistance @f$ - 1 @f$ steps.  In the
 *  latter case, there exists no region which made progress, all
 *  regions in the previous calling context were left and all regions
 *  in the current calling context were newly entered.
 *
 *  Note that for @eref{CallingContextLeave} events, the parent of the
 *  referenced @eref{CallingContext} must be used as the previous
 *  calling context for the next event.
 *
 *  Regions which were entered with a @eref{CallingContextEnter} event
 *  form an upper bound for the unwind distance, i.e., the @a
 *  unwindDistance points either to the parent of the last such
 *  entered region, or a node which is a descendant to this parent.
 *
 *  To summarize, an @a unwindDistance of @f$ 0 @f$ means that no regions
 *  were left, newly entered, or made any progress.  An @a
 *  unwindDistance of @f$ 1 @f$ means that some regions were left
 *  regarding the previous calling context, no regions were newly
 *  entered, and there was progress in the region of the first node.
 *  An @a unwindDistance greater than @f$ 1 @f$ means that some
 *  regions were left regarding the previous calling context, there
 *  was progress in one region, and the first @a unwindDistance @f$ -
 *  1 @f$ regions were newly entered.
 *
 *  @param writer             Writer object.
 *  @param self               The unique identifier for this
 *                            @eref{CallingContext} definition.
 *  @param region             The region. References a @eref{Region} definition.
 *  @param sourceCodeLocation The absolute source code location of this calling
 *                            context. References a @eref{SourceCodeLocation}
 *                            definition.
 *  @param parent             Parent id of this context. References a
 *                            @eref{CallingContext} definition.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteCallingContext( OTF2_DefWriter*            writer,
                                    OTF2_CallingContextRef     self,
                                    OTF2_RegionRef             region,
                                    OTF2_SourceCodeLocationRef sourceCodeLocation,
                                    OTF2_CallingContextRef     parent );


/** @brief Writes a CallingContextProperty definition record into the DefWriter.
 *
 *  An arbitrary key/value property for a @eref{CallingContext}
 *  definition.
 *
 *  @param writer         Writer object.
 *  @param callingContext Parent @eref{CallingContext} definition to which this
 *                        one is a supplementary definition. References a
 *                        @eref{CallingContext} definition.
 *  @param name           Property name. References a @eref{String} definition.
 *  @param type           The type of this property. Must match with the defined
 *                        type of the @a property.
 *  @param value          The value of this property.
 *
 *  @since Version 2.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteCallingContextProperty( OTF2_DefWriter*        writer,
                                            OTF2_CallingContextRef callingContext,
                                            OTF2_StringRef         name,
                                            OTF2_Type              type,
                                            OTF2_AttributeValue    value );


/** @brief Writes a InterruptGenerator definition record into the DefWriter.
 *
 *  Defines an interrupt generator which periodically triggers
 *  @eref{CallingContextSample} events.  If the mode of the interrupt
 *  generator is set to @eref{OTF2_INTERRUPT_GENERATOR_MODE_TIME}, the
 *  generator produces interrupts which are uniformly distributed over
 *  time, and the unit of the period is implicitly in seconds.  If the
 *  mode is @eref{OTF2_INTERRUPT_GENERATOR_MODE_COUNT}, the interrupt
 *  is triggered if a specific counter threshold is reached in the
 *  system.  Therefore these samples are unlikely to be uniformly
 *  distributed over time.  The unit of the period is then implicitly
 *  a number (threshold value).
 *
 *  The interrupts period in base unit (which is implicitly seconds or
 *  number, based on the @p mode) is derived out of the @p base, the
 *  @p exponent, and the @p period attributes by this formula:
 *
 *  base-period = period x base^exponent
 *
 *  @param writer                 Writer object.
 *  @param self                   The unique identifier for this
 *                                @eref{InterruptGenerator} definition.
 *  @param name                   The name of this interrupt generator.
 *                                References a @eref{String} definition.
 *  @param interruptGeneratorMode Mode of the interrupt generator.
 *  @param base                   The base for the period calculation.
 *  @param exponent               The exponent for the period calculation.
 *  @param period                 The period this interrupt generator generates
 *                                interrupts.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteInterruptGenerator( OTF2_DefWriter*             writer,
                                        OTF2_InterruptGeneratorRef  self,
                                        OTF2_StringRef              name,
                                        OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                        OTF2_Base                   base,
                                        int64_t                     exponent,
                                        uint64_t                    period );


/** @brief Writes a IoFileProperty definition record into the DefWriter.
 *
 *  Extensible annotation for the polymorphic @eref{IoFile} definition
 *  class.
 *
 *  The tuple (@a ioFile, @a name) must be unique.
 *
 *  @param writer Writer object.
 *  @param ioFile Parent @eref{IoRegularFile} definition to which this one is a
 *                supplementary definition. References a @eref{IoRegularFile}
 *                definition.
 *  @param name   Property name. References a @eref{String} definition.
 *  @param type   The type of this property.
 *  @param value  The value of this property.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteIoFileProperty( OTF2_DefWriter*     writer,
                                    OTF2_IoFileRef      ioFile,
                                    OTF2_StringRef      name,
                                    OTF2_Type           type,
                                    OTF2_AttributeValue value );


/** @brief Writes a IoRegularFile definition record into the DefWriter.
 *
 *  Defines a regular file from which an @eref{IoHandle} can be created.
 *
 *  This definition is member of the polymorphic @eref{IoFile} definition
 *  class. All definitions of this polymorphic definition class share
 *  the same global identifier namespace.
 *
 *  @param writer Writer object.
 *  @param self   The unique identifier for this @eref{IoRegularFile} definition.
 *  @param name   Name of the file. References a @eref{String} definition.
 *  @param scope  Defines the physical scope of this @eref{IoRegularFile} in the
 *                system tree. E.g., two @eref{IoRegularFile} definitions
 *                with the same name but different @p scope values are
 *                physically different, thus I/O operations through
 *                @eref{IoHandle}s do not operate on the same file.
 *                References a @eref{SystemTreeNode} definition.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteIoRegularFile( OTF2_DefWriter*        writer,
                                   OTF2_IoFileRef         self,
                                   OTF2_StringRef         name,
                                   OTF2_SystemTreeNodeRef scope );


/** @brief Writes a IoDirectory definition record into the DefWriter.
 *
 *  Defines a directory from which an @eref{IoHandle} can be created.
 *
 *  This definition is member of the polymorphic @eref{IoFile} definition
 *  class. All definitions of this polymorphic definition class share
 *  the same global identifier namespace.
 *
 *  @param writer Writer object.
 *  @param self   The unique identifier for this @eref{IoRegularFile} definition.
 *  @param name   Name of the directory. References a @eref{String} definition.
 *  @param scope  Defines the physical scope of this @eref{IoDirectory} in the
 *                system tree. E.g., two @eref{IoDirectory} definitions with
 *                the same name but different @p scope values are physically
 *                different, thus I/O operations through @eref{IoHandle}s do
 *                not operate on the same directory. References a
 *                @eref{SystemTreeNode} definition.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteIoDirectory( OTF2_DefWriter*        writer,
                                 OTF2_IoFileRef         self,
                                 OTF2_StringRef         name,
                                 OTF2_SystemTreeNodeRef scope );


/** @brief Writes a IoHandle definition record into the DefWriter.
 *
 *  Defines an I/O handle which will be used by subsequent I/O operations.
 *  I/O operations can only be applied to @emph{active} I/O handles.
 *  An I/O handle gets @emph{active} either if it was marked with the
 *  @eref{OTF2_IO_HANDLE_FLAG_PRE_CREATED} flag, after it was
 *  referenced in an @eref{IoCreateHandle} event, or it was referenced
 *  in the @a newHandle attribute of an @eref{IoDuplicateHandle}
 *  event. It gets @emph{inactive} if it was referenced in an
 *  @eref{IoDestroyHandle} event. This life cycle can be repeated
 *  indefinitely. Though the @eref{OTF2_IO_HANDLE_FLAG_PRE_CREATED}
 *  flag is unset after a @eref{IoDuplicateHandle} event. All
 *  @eref{Location}s of a @eref{LocationGroup} have access to an
 *  @emph{active} @eref{IoHandle}, regardless which @eref{Location} of
 *  the @eref{LocationGroup} activated the @eref{IoHandle}.
 *
 *  @param writer        Writer object.
 *  @param self          The unique identifier for this @eref{IoHandle}
 *                       definition.
 *  @param name          Handle name. References a @eref{String} definition.
 *  @param file          File identifier. References a @eref{IoRegularFile}, or a
 *                       @eref{IoDirectory} definition.
 *  @param ioParadigm    The I/O paradigm. References a @eref{IoParadigm}
 *                       definition.
 *  @param ioHandleFlags Special characteristics of this handle.
 *  @param comm          Scope of the file handle. This scope defines which
 *                       process can access this file via this handle and
 *                       also defines the collective context for this handle.
 *                       References a @eref{Comm} definition.
 *  @param parent        Parent, in case this I/O handle was created and operated
 *                       by an higher-level I/O paradigm. References a
 *                       @eref{IoHandle} definition.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteIoHandle( OTF2_DefWriter*    writer,
                              OTF2_IoHandleRef   self,
                              OTF2_StringRef     name,
                              OTF2_IoFileRef     file,
                              OTF2_IoParadigmRef ioParadigm,
                              OTF2_IoHandleFlag  ioHandleFlags,
                              OTF2_CommRef       comm,
                              OTF2_IoHandleRef   parent );


/** @brief Writes a IoPreCreatedHandleState definition record into the DefWriter.
 *
 *  Provide the I/O access mode and status flags for @emph{pre-created}
 *  @eref{IoHandle}s.
 *
 *  Only allowed once for a @eref{IoHandle} definition with the
 *  @eref{OTF2_IO_HANDLE_FLAG_PRE_CREATED} flag set.
 *
 *  @param writer      Writer object.
 *  @param ioHandle    Parent @eref{IoHandle} definition to which this one is a
 *                     supplementary definition. References a @eref{IoHandle}
 *                     definition.
 *  @param mode        The access mode of the @emph{pre-created} @eref{IoHandle}.
 *  @param statusFlags The status flags of the @emph{pre-created}
 *                     @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteIoPreCreatedHandleState( OTF2_DefWriter*   writer,
                                             OTF2_IoHandleRef  ioHandle,
                                             OTF2_IoAccessMode mode,
                                             OTF2_IoStatusFlag statusFlags );


/** @brief Writes a CallpathParameter definition record into the DefWriter.
 *
 *  A parameter for a callpath definition.
 *
 *  @param writer    Writer object.
 *  @param callpath  Parent @eref{Callpath} definition to which this one is a
 *                   supplementary definition. References a @eref{Callpath}
 *                   definition.
 *  @param parameter The parameter of this callpath. References a
 *                   @eref{Parameter} definition.
 *  @param type      The type of the attribute value. Must match the type of the
 *                   parameter.
 *  @param value     The value of the parameter for this callpath.
 *
 *  @since Version 2.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_WriteCallpathParameter( OTF2_DefWriter*     writer,
                                       OTF2_CallpathRef    callpath,
                                       OTF2_ParameterRef   parameter,
                                       OTF2_Type           type,
                                       OTF2_AttributeValue value );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_DEF_WRITER_H */
