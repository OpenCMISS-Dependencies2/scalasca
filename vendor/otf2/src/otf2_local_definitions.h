/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
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


#ifndef OTF2_INTERNAL_LOCAL_DEFINITIONS_H
#define OTF2_INTERNAL_LOCAL_DEFINITIONS_H


/**
 *  @internal
 *
 *  @file
 *  @source     templates/otf2_local_definitions.tmpl.h
 */


/** @brief Wrapper for the OTF2_LocalDefinitionType_enum
 */
typedef uint8_t OTF2_LocalDefinitionType;


/** @brief An enum which holds all values that are possible global definition types.
 */
enum OTF2_LocalDefinitionType_enum
{
    /* First five entries are reserved for OTF2 internals */

    /* OTF2 Records */
    OTF2_LOCAL_DEF_MAPPING_TABLE               = 5,  /**< */
    OTF2_LOCAL_DEF_CLOCK_OFFSET                = 6,  /**< */
    OTF2_LOCAL_DEF_STRING                      = 10, /**< */
    OTF2_LOCAL_DEF_ATTRIBUTE                   = 11, /**< */
    OTF2_LOCAL_DEF_SYSTEM_TREE_NODE            = 12, /**< */
    OTF2_LOCAL_DEF_LOCATION_GROUP              = 13, /**< */
    OTF2_LOCAL_DEF_LOCATION                    = 14, /**< */
    OTF2_LOCAL_DEF_REGION                      = 15, /**< */
    OTF2_LOCAL_DEF_CALLSITE                    = 16, /**< */
    OTF2_LOCAL_DEF_CALLPATH                    = 17, /**< */
    OTF2_LOCAL_DEF_GROUP                       = 18, /**< */
    OTF2_LOCAL_DEF_METRIC_MEMBER               = 19, /**< */
    OTF2_LOCAL_DEF_METRIC_CLASS                = 20, /**< */
    OTF2_LOCAL_DEF_METRIC_INSTANCE             = 21, /**< */
    OTF2_LOCAL_DEF_COMM                        = 22, /**< */
    OTF2_LOCAL_DEF_PARAMETER                   = 23, /**< */
    OTF2_LOCAL_DEF_RMA_WIN                     = 24, /**< */
    OTF2_LOCAL_DEF_METRIC_CLASS_RECORDER       = 25, /**< */
    OTF2_LOCAL_DEF_SYSTEM_TREE_NODE_PROPERTY   = 26, /**< */
    OTF2_LOCAL_DEF_SYSTEM_TREE_NODE_DOMAIN     = 27, /**< */
    OTF2_LOCAL_DEF_LOCATION_GROUP_PROPERTY     = 28, /**< */
    OTF2_LOCAL_DEF_LOCATION_PROPERTY           = 29, /**< */
    OTF2_LOCAL_DEF_CART_DIMENSION              = 30, /**< */
    OTF2_LOCAL_DEF_CART_TOPOLOGY               = 31, /**< */
    OTF2_LOCAL_DEF_CART_COORDINATE             = 32, /**< */
    OTF2_LOCAL_DEF_SOURCE_CODE_LOCATION        = 33, /**< */
    OTF2_LOCAL_DEF_CALLING_CONTEXT             = 34, /**< */
    OTF2_LOCAL_DEF_CALLING_CONTEXT_PROPERTY    = 35, /**< */
    OTF2_LOCAL_DEF_INTERRUPT_GENERATOR         = 36, /**< */
    OTF2_LOCAL_DEF_IO_FILE_PROPERTY            = 37, /**< */
    OTF2_LOCAL_DEF_IO_REGULAR_FILE             = 38, /**< */
    OTF2_LOCAL_DEF_IO_DIRECTORY                = 39, /**< */
    OTF2_LOCAL_DEF_IO_HANDLE                   = 40, /**< */
    OTF2_LOCAL_DEF_IO_PRE_CREATED_HANDLE_STATE = 41, /**< */
    OTF2_LOCAL_DEF_CALLPATH_PARAMETER          = 42, /**< */

    OTF2_LOCAL_DEF_MAX_ID
};


/** @brief MappingTable definition.
 */
typedef struct OTF2_LocalDefMappingTable_struct
{
    OTF2_MappingType  mapping_type;
    const OTF2_IdMap* id_map;
} OTF2_LocalDefMappingTable;


/** @brief ClockOffset definition.
 */
typedef struct OTF2_LocalDefClockOffset_struct
{
    OTF2_TimeStamp time;
    int64_t        offset;
    double         standard_deviation;
} OTF2_LocalDefClockOffset;


/** @brief String definition.
 */
typedef struct OTF2_LocalDefString_struct
{
    OTF2_StringRef self;
    const char*    string;
} OTF2_LocalDefString;


/** @brief Attribute definition.
 */
typedef struct OTF2_LocalDefAttribute_struct
{
    OTF2_AttributeRef self;
    OTF2_StringRef    name;
    OTF2_Type         type;
    OTF2_StringRef    description;
} OTF2_LocalDefAttribute;


/** @brief SystemTreeNode definition.
 */
typedef struct OTF2_LocalDefSystemTreeNode_struct
{
    OTF2_SystemTreeNodeRef self;
    OTF2_StringRef         name;
    OTF2_StringRef         class_name;
    OTF2_SystemTreeNodeRef parent;
} OTF2_LocalDefSystemTreeNode;


/** @brief LocationGroup definition.
 */
typedef struct OTF2_LocalDefLocationGroup_struct
{
    OTF2_LocationGroupRef  self;
    OTF2_StringRef         name;
    OTF2_LocationGroupType location_group_type;
    OTF2_SystemTreeNodeRef system_tree_parent;
} OTF2_LocalDefLocationGroup;


/** @brief Location definition.
 */
typedef struct OTF2_LocalDefLocation_struct
{
    OTF2_LocationRef      self;
    OTF2_StringRef        name;
    OTF2_LocationType     location_type;
    uint64_t              number_of_events;
    OTF2_LocationGroupRef location_group;
} OTF2_LocalDefLocation;


/** @brief Region definition.
 */
typedef struct OTF2_LocalDefRegion_struct
{
    OTF2_RegionRef  self;
    OTF2_StringRef  name;
    OTF2_StringRef  description;
    OTF2_RegionType region_type;
    OTF2_StringRef  source_file;
    uint32_t        begin_line_number;
    uint32_t        end_line_number;
    OTF2_StringRef  canonical_name;
    OTF2_RegionRole region_role;
    OTF2_Paradigm   paradigm;
    OTF2_RegionFlag region_flags;
} OTF2_LocalDefRegion;


/** @brief Callsite definition.
 */
typedef struct OTF2_LocalDefCallsite_struct
{
    OTF2_CallsiteRef self;
    OTF2_StringRef   source_file;
    uint32_t         line_number;
    OTF2_RegionRef   entered_region;
    OTF2_RegionRef   left_region;
} OTF2_LocalDefCallsite;


/** @brief Callpath definition.
 */
typedef struct OTF2_LocalDefCallpath_struct
{
    OTF2_CallpathRef self;
    OTF2_CallpathRef parent;
    OTF2_RegionRef   region;
} OTF2_LocalDefCallpath;


/** @brief Group definition.
 */
typedef struct OTF2_LocalDefGroup_struct
{
    OTF2_GroupRef       self;
    OTF2_StringRef      name;
    OTF2_GroupTypePre12 group_type_pre12;
    uint32_t            number_of_members;
    uint64_t*           members;
    OTF2_GroupType      group_type;
    OTF2_Paradigm       paradigm;
    OTF2_GroupFlag      group_flags;
} OTF2_LocalDefGroup;


/** @brief MetricMember definition.
 */
typedef struct OTF2_LocalDefMetricMember_struct
{
    OTF2_MetricMemberRef self;
    OTF2_StringRef       name;
    OTF2_StringRef       description;
    OTF2_MetricType      metric_type;
    OTF2_MetricMode      metric_mode;
    OTF2_Type            value_type;
    OTF2_Base            base;
    int64_t              exponent;
    OTF2_StringRef       unit;
} OTF2_LocalDefMetricMember;


/** @brief MetricClass definition.
 */
typedef struct OTF2_LocalDefMetricClass_struct
{
    OTF2_MetricRef        self;
    uint8_t               number_of_metrics;
    OTF2_MetricMemberRef* metric_members;
    OTF2_MetricOccurrence metric_occurrence;
    OTF2_RecorderKind     recorder_kind;
} OTF2_LocalDefMetricClass;


/** @brief MetricInstance definition.
 */
typedef struct OTF2_LocalDefMetricInstance_struct
{
    OTF2_MetricRef   self;
    OTF2_MetricRef   metric_class;
    OTF2_LocationRef recorder;
    OTF2_MetricScope metric_scope;
    uint64_t         scope;
} OTF2_LocalDefMetricInstance;


/** @brief Comm definition.
 */
typedef struct OTF2_LocalDefComm_struct
{
    OTF2_CommRef   self;
    OTF2_StringRef name;
    OTF2_GroupRef  group;
    OTF2_CommRef   parent;
} OTF2_LocalDefComm;


/** @brief Parameter definition.
 */
typedef struct OTF2_LocalDefParameter_struct
{
    OTF2_ParameterRef  self;
    OTF2_StringRef     name;
    OTF2_ParameterType parameter_type;
} OTF2_LocalDefParameter;


/** @brief RmaWin definition.
 */
typedef struct OTF2_LocalDefRmaWin_struct
{
    OTF2_RmaWinRef self;
    OTF2_StringRef name;
    OTF2_CommRef   comm;
} OTF2_LocalDefRmaWin;


/** @brief MetricClassRecorder definition.
 */
typedef struct OTF2_LocalDefMetricClassRecorder_struct
{
    OTF2_MetricRef   metric;
    OTF2_LocationRef recorder;
} OTF2_LocalDefMetricClassRecorder;


/** @brief SystemTreeNodeProperty definition.
 */
typedef struct OTF2_LocalDefSystemTreeNodeProperty_struct
{
    OTF2_SystemTreeNodeRef system_tree_node;
    OTF2_StringRef         name;
    OTF2_StringRef         string_value;
    OTF2_Type              type;
    OTF2_AttributeValue    value;
} OTF2_LocalDefSystemTreeNodeProperty;


/** @brief SystemTreeNodeDomain definition.
 */
typedef struct OTF2_LocalDefSystemTreeNodeDomain_struct
{
    OTF2_SystemTreeNodeRef system_tree_node;
    OTF2_SystemTreeDomain  system_tree_domain;
} OTF2_LocalDefSystemTreeNodeDomain;


/** @brief LocationGroupProperty definition.
 */
typedef struct OTF2_LocalDefLocationGroupProperty_struct
{
    OTF2_LocationGroupRef location_group;
    OTF2_StringRef        name;
    OTF2_StringRef        string_value;
    OTF2_Type             type;
    OTF2_AttributeValue   value;
} OTF2_LocalDefLocationGroupProperty;


/** @brief LocationProperty definition.
 */
typedef struct OTF2_LocalDefLocationProperty_struct
{
    OTF2_LocationRef    location;
    OTF2_StringRef      name;
    OTF2_StringRef      string_value;
    OTF2_Type           type;
    OTF2_AttributeValue value;
} OTF2_LocalDefLocationProperty;


/** @brief CartDimension definition.
 */
typedef struct OTF2_LocalDefCartDimension_struct
{
    OTF2_CartDimensionRef self;
    OTF2_StringRef        name;
    uint32_t              size;
    OTF2_CartPeriodicity  cart_periodicity;
} OTF2_LocalDefCartDimension;


/** @brief CartTopology definition.
 */
typedef struct OTF2_LocalDefCartTopology_struct
{
    OTF2_CartTopologyRef   self;
    OTF2_StringRef         name;
    OTF2_CommRef           communicator;
    uint8_t                number_of_dimensions;
    OTF2_CartDimensionRef* cart_dimensions;
} OTF2_LocalDefCartTopology;


/** @brief CartCoordinate definition.
 */
typedef struct OTF2_LocalDefCartCoordinate_struct
{
    OTF2_CartTopologyRef cart_topology;
    uint32_t             rank;
    uint8_t              number_of_dimensions;
    uint32_t*            coordinates;
} OTF2_LocalDefCartCoordinate;


/** @brief SourceCodeLocation definition.
 */
typedef struct OTF2_LocalDefSourceCodeLocation_struct
{
    OTF2_SourceCodeLocationRef self;
    OTF2_StringRef             file;
    uint32_t                   line_number;
} OTF2_LocalDefSourceCodeLocation;


/** @brief CallingContext definition.
 */
typedef struct OTF2_LocalDefCallingContext_struct
{
    OTF2_CallingContextRef     self;
    OTF2_RegionRef             region;
    OTF2_SourceCodeLocationRef source_code_location;
    OTF2_CallingContextRef     parent;
} OTF2_LocalDefCallingContext;


/** @brief CallingContextProperty definition.
 */
typedef struct OTF2_LocalDefCallingContextProperty_struct
{
    OTF2_CallingContextRef calling_context;
    OTF2_StringRef         name;
    OTF2_Type              type;
    OTF2_AttributeValue    value;
} OTF2_LocalDefCallingContextProperty;


/** @brief InterruptGenerator definition.
 */
typedef struct OTF2_LocalDefInterruptGenerator_struct
{
    OTF2_InterruptGeneratorRef  self;
    OTF2_StringRef              name;
    OTF2_InterruptGeneratorMode interrupt_generator_mode;
    OTF2_Base                   base;
    int64_t                     exponent;
    uint64_t                    period;
} OTF2_LocalDefInterruptGenerator;


/** @brief IoFileProperty definition.
 */
typedef struct OTF2_LocalDefIoFileProperty_struct
{
    OTF2_IoFileRef      io_file;
    OTF2_StringRef      name;
    OTF2_Type           type;
    OTF2_AttributeValue value;
} OTF2_LocalDefIoFileProperty;


/** @brief IoRegularFile definition.
 */
typedef struct OTF2_LocalDefIoRegularFile_struct
{
    OTF2_IoFileRef         self;
    OTF2_StringRef         name;
    OTF2_SystemTreeNodeRef scope;
} OTF2_LocalDefIoRegularFile;


/** @brief IoDirectory definition.
 */
typedef struct OTF2_LocalDefIoDirectory_struct
{
    OTF2_IoFileRef         self;
    OTF2_StringRef         name;
    OTF2_SystemTreeNodeRef scope;
} OTF2_LocalDefIoDirectory;


/** @brief IoHandle definition.
 */
typedef struct OTF2_LocalDefIoHandle_struct
{
    OTF2_IoHandleRef   self;
    OTF2_StringRef     name;
    OTF2_IoFileRef     file;
    OTF2_IoParadigmRef io_paradigm;
    OTF2_IoHandleFlag  io_handle_flags;
    OTF2_CommRef       comm;
    OTF2_IoHandleRef   parent;
} OTF2_LocalDefIoHandle;


/** @brief IoPreCreatedHandleState definition.
 */
typedef struct OTF2_LocalDefIoPreCreatedHandleState_struct
{
    OTF2_IoHandleRef  io_handle;
    OTF2_IoAccessMode mode;
    OTF2_IoStatusFlag status_flags;
} OTF2_LocalDefIoPreCreatedHandleState;


/** @brief CallpathParameter definition.
 */
typedef struct OTF2_LocalDefCallpathParameter_struct
{
    OTF2_CallpathRef    callpath;
    OTF2_ParameterRef   parameter;
    OTF2_Type           type;
    OTF2_AttributeValue value;
} OTF2_LocalDefCallpathParameter;


struct OTF2_DefReaderCallbacks_struct
{
    OTF2_DefReaderCallback_Unknown                 unknown;
    OTF2_DefReaderCallback_MappingTable            mapping_table;
    OTF2_DefReaderCallback_ClockOffset             clock_offset;
    OTF2_DefReaderCallback_String                  string;
    OTF2_DefReaderCallback_Attribute               attribute;
    OTF2_DefReaderCallback_SystemTreeNode          system_tree_node;
    OTF2_DefReaderCallback_LocationGroup           location_group;
    OTF2_DefReaderCallback_Location                location;
    OTF2_DefReaderCallback_Region                  region;
    OTF2_DefReaderCallback_Callsite                callsite;
    OTF2_DefReaderCallback_Callpath                callpath;
    OTF2_DefReaderCallback_Group                   group;
    OTF2_DefReaderCallback_MetricMember            metric_member;
    OTF2_DefReaderCallback_MetricClass             metric_class;
    OTF2_DefReaderCallback_MetricInstance          metric_instance;
    OTF2_DefReaderCallback_Comm                    comm;
    OTF2_DefReaderCallback_Parameter               parameter;
    OTF2_DefReaderCallback_RmaWin                  rma_win;
    OTF2_DefReaderCallback_MetricClassRecorder     metric_class_recorder;
    OTF2_DefReaderCallback_SystemTreeNodeProperty  system_tree_node_property;
    OTF2_DefReaderCallback_SystemTreeNodeDomain    system_tree_node_domain;
    OTF2_DefReaderCallback_LocationGroupProperty   location_group_property;
    OTF2_DefReaderCallback_LocationProperty        location_property;
    OTF2_DefReaderCallback_CartDimension           cart_dimension;
    OTF2_DefReaderCallback_CartTopology            cart_topology;
    OTF2_DefReaderCallback_CartCoordinate          cart_coordinate;
    OTF2_DefReaderCallback_SourceCodeLocation      source_code_location;
    OTF2_DefReaderCallback_CallingContext          calling_context;
    OTF2_DefReaderCallback_CallingContextProperty  calling_context_property;
    OTF2_DefReaderCallback_InterruptGenerator      interrupt_generator;
    OTF2_DefReaderCallback_IoFileProperty          io_file_property;
    OTF2_DefReaderCallback_IoRegularFile           io_regular_file;
    OTF2_DefReaderCallback_IoDirectory             io_directory;
    OTF2_DefReaderCallback_IoHandle                io_handle;
    OTF2_DefReaderCallback_IoPreCreatedHandleState io_pre_created_handle_state;
    OTF2_DefReaderCallback_CallpathParameter       callpath_parameter;
};

#endif /* OTF2_INTERNAL_LOCAL_DEFINITIONS_H */
