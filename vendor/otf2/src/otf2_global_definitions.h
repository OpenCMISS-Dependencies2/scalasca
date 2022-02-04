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


#ifndef OTF2_INTERNAL_GLOBAL_DEFINITIONS_H
#define OTF2_INTERNAL_GLOBAL_DEFINITIONS_H


/**
 *  @internal
 *
 *  @file
 *  @source     templates/otf2_global_definitions.tmpl.h
 */


/** @brief Wrapper for the OTF2_GlobalDefinitionType_enum
 */
typedef uint8_t OTF2_GlobalDefinitionType;


/** @brief An enum which holds all values that are possible global definition types.
 */
enum OTF2_GlobalDefinitionType_enum
{
    /* First five entries are reserved for OTF2 internals */

    /* OTF2 Records */
    OTF2_GLOBAL_DEF_CLOCK_PROPERTIES            = 5,  /**< */
    OTF2_GLOBAL_DEF_PARADIGM                    = 6,  /**< */
    OTF2_GLOBAL_DEF_PARADIGM_PROPERTY           = 7,  /**< */
    OTF2_GLOBAL_DEF_IO_PARADIGM                 = 8,  /**< */
    OTF2_GLOBAL_DEF_STRING                      = 10, /**< */
    OTF2_GLOBAL_DEF_ATTRIBUTE                   = 11, /**< */
    OTF2_GLOBAL_DEF_SYSTEM_TREE_NODE            = 12, /**< */
    OTF2_GLOBAL_DEF_LOCATION_GROUP              = 13, /**< */
    OTF2_GLOBAL_DEF_LOCATION                    = 14, /**< */
    OTF2_GLOBAL_DEF_REGION                      = 15, /**< */
    OTF2_GLOBAL_DEF_CALLSITE                    = 16, /**< */
    OTF2_GLOBAL_DEF_CALLPATH                    = 17, /**< */
    OTF2_GLOBAL_DEF_GROUP                       = 18, /**< */
    OTF2_GLOBAL_DEF_METRIC_MEMBER               = 19, /**< */
    OTF2_GLOBAL_DEF_METRIC_CLASS                = 20, /**< */
    OTF2_GLOBAL_DEF_METRIC_INSTANCE             = 21, /**< */
    OTF2_GLOBAL_DEF_COMM                        = 22, /**< */
    OTF2_GLOBAL_DEF_PARAMETER                   = 23, /**< */
    OTF2_GLOBAL_DEF_RMA_WIN                     = 24, /**< */
    OTF2_GLOBAL_DEF_METRIC_CLASS_RECORDER       = 25, /**< */
    OTF2_GLOBAL_DEF_SYSTEM_TREE_NODE_PROPERTY   = 26, /**< */
    OTF2_GLOBAL_DEF_SYSTEM_TREE_NODE_DOMAIN     = 27, /**< */
    OTF2_GLOBAL_DEF_LOCATION_GROUP_PROPERTY     = 28, /**< */
    OTF2_GLOBAL_DEF_LOCATION_PROPERTY           = 29, /**< */
    OTF2_GLOBAL_DEF_CART_DIMENSION              = 30, /**< */
    OTF2_GLOBAL_DEF_CART_TOPOLOGY               = 31, /**< */
    OTF2_GLOBAL_DEF_CART_COORDINATE             = 32, /**< */
    OTF2_GLOBAL_DEF_SOURCE_CODE_LOCATION        = 33, /**< */
    OTF2_GLOBAL_DEF_CALLING_CONTEXT             = 34, /**< */
    OTF2_GLOBAL_DEF_CALLING_CONTEXT_PROPERTY    = 35, /**< */
    OTF2_GLOBAL_DEF_INTERRUPT_GENERATOR         = 36, /**< */
    OTF2_GLOBAL_DEF_IO_FILE_PROPERTY            = 37, /**< */
    OTF2_GLOBAL_DEF_IO_REGULAR_FILE             = 38, /**< */
    OTF2_GLOBAL_DEF_IO_DIRECTORY                = 39, /**< */
    OTF2_GLOBAL_DEF_IO_HANDLE                   = 40, /**< */
    OTF2_GLOBAL_DEF_IO_PRE_CREATED_HANDLE_STATE = 41, /**< */
    OTF2_GLOBAL_DEF_CALLPATH_PARAMETER          = 42, /**< */

    OTF2_GLOBAL_DEF_MAX_ID
};


/** @brief ClockProperties definition.
 */
typedef struct OTF2_GlobalDefClockProperties_struct
{
    uint64_t timer_resolution;
    uint64_t global_offset;
    uint64_t trace_length;
} OTF2_GlobalDefClockProperties;


/** @brief Paradigm definition.
 */
typedef struct OTF2_GlobalDefParadigm_struct
{
    OTF2_Paradigm      paradigm;
    OTF2_StringRef     name;
    OTF2_ParadigmClass paradigm_class;
} OTF2_GlobalDefParadigm;


/** @brief ParadigmProperty definition.
 */
typedef struct OTF2_GlobalDefParadigmProperty_struct
{
    OTF2_Paradigm         paradigm;
    OTF2_ParadigmProperty property;
    OTF2_Type             type;
    OTF2_AttributeValue   value;
} OTF2_GlobalDefParadigmProperty;


/** @brief IoParadigm definition.
 */
typedef struct OTF2_GlobalDefIoParadigm_struct
{
    OTF2_IoParadigmRef       self;
    OTF2_StringRef           identification;
    OTF2_StringRef           name;
    OTF2_IoParadigmClass     io_paradigm_class;
    OTF2_IoParadigmFlag      io_paradigm_flags;
    uint8_t                  number_of_properties;
    OTF2_IoParadigmProperty* properties;
    OTF2_Type*               types;
    OTF2_AttributeValue*     values;
} OTF2_GlobalDefIoParadigm;


/** @brief String definition.
 */
typedef struct OTF2_GlobalDefString_struct
{
    OTF2_StringRef self;
    const char*    string;
} OTF2_GlobalDefString;


/** @brief Attribute definition.
 */
typedef struct OTF2_GlobalDefAttribute_struct
{
    OTF2_AttributeRef self;
    OTF2_StringRef    name;
    OTF2_Type         type;
    OTF2_StringRef    description;
} OTF2_GlobalDefAttribute;


/** @brief SystemTreeNode definition.
 */
typedef struct OTF2_GlobalDefSystemTreeNode_struct
{
    OTF2_SystemTreeNodeRef self;
    OTF2_StringRef         name;
    OTF2_StringRef         class_name;
    OTF2_SystemTreeNodeRef parent;
} OTF2_GlobalDefSystemTreeNode;


/** @brief LocationGroup definition.
 */
typedef struct OTF2_GlobalDefLocationGroup_struct
{
    OTF2_LocationGroupRef  self;
    OTF2_StringRef         name;
    OTF2_LocationGroupType location_group_type;
    OTF2_SystemTreeNodeRef system_tree_parent;
} OTF2_GlobalDefLocationGroup;


/** @brief Location definition.
 */
typedef struct OTF2_GlobalDefLocation_struct
{
    OTF2_LocationRef      self;
    OTF2_StringRef        name;
    OTF2_LocationType     location_type;
    uint64_t              number_of_events;
    OTF2_LocationGroupRef location_group;
} OTF2_GlobalDefLocation;


/** @brief Region definition.
 */
typedef struct OTF2_GlobalDefRegion_struct
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
} OTF2_GlobalDefRegion;


/** @brief Callsite definition.
 */
typedef struct OTF2_GlobalDefCallsite_struct
{
    OTF2_CallsiteRef self;
    OTF2_StringRef   source_file;
    uint32_t         line_number;
    OTF2_RegionRef   entered_region;
    OTF2_RegionRef   left_region;
} OTF2_GlobalDefCallsite;


/** @brief Callpath definition.
 */
typedef struct OTF2_GlobalDefCallpath_struct
{
    OTF2_CallpathRef self;
    OTF2_CallpathRef parent;
    OTF2_RegionRef   region;
} OTF2_GlobalDefCallpath;


/** @brief Group definition.
 */
typedef struct OTF2_GlobalDefGroup_struct
{
    OTF2_GroupRef       self;
    OTF2_StringRef      name;
    OTF2_GroupTypePre12 group_type_pre12;
    uint32_t            number_of_members;
    uint64_t*           members;
    OTF2_GroupType      group_type;
    OTF2_Paradigm       paradigm;
    OTF2_GroupFlag      group_flags;
} OTF2_GlobalDefGroup;


/** @brief MetricMember definition.
 */
typedef struct OTF2_GlobalDefMetricMember_struct
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
} OTF2_GlobalDefMetricMember;


/** @brief MetricClass definition.
 */
typedef struct OTF2_GlobalDefMetricClass_struct
{
    OTF2_MetricRef        self;
    uint8_t               number_of_metrics;
    OTF2_MetricMemberRef* metric_members;
    OTF2_MetricOccurrence metric_occurrence;
    OTF2_RecorderKind     recorder_kind;
} OTF2_GlobalDefMetricClass;


/** @brief MetricInstance definition.
 */
typedef struct OTF2_GlobalDefMetricInstance_struct
{
    OTF2_MetricRef   self;
    OTF2_MetricRef   metric_class;
    OTF2_LocationRef recorder;
    OTF2_MetricScope metric_scope;
    uint64_t         scope;
} OTF2_GlobalDefMetricInstance;


/** @brief Comm definition.
 */
typedef struct OTF2_GlobalDefComm_struct
{
    OTF2_CommRef   self;
    OTF2_StringRef name;
    OTF2_GroupRef  group;
    OTF2_CommRef   parent;
} OTF2_GlobalDefComm;


/** @brief Parameter definition.
 */
typedef struct OTF2_GlobalDefParameter_struct
{
    OTF2_ParameterRef  self;
    OTF2_StringRef     name;
    OTF2_ParameterType parameter_type;
} OTF2_GlobalDefParameter;


/** @brief RmaWin definition.
 */
typedef struct OTF2_GlobalDefRmaWin_struct
{
    OTF2_RmaWinRef self;
    OTF2_StringRef name;
    OTF2_CommRef   comm;
} OTF2_GlobalDefRmaWin;


/** @brief MetricClassRecorder definition.
 */
typedef struct OTF2_GlobalDefMetricClassRecorder_struct
{
    OTF2_MetricRef   metric;
    OTF2_LocationRef recorder;
} OTF2_GlobalDefMetricClassRecorder;


/** @brief SystemTreeNodeProperty definition.
 */
typedef struct OTF2_GlobalDefSystemTreeNodeProperty_struct
{
    OTF2_SystemTreeNodeRef system_tree_node;
    OTF2_StringRef         name;
    OTF2_StringRef         string_value;
    OTF2_Type              type;
    OTF2_AttributeValue    value;
} OTF2_GlobalDefSystemTreeNodeProperty;


/** @brief SystemTreeNodeDomain definition.
 */
typedef struct OTF2_GlobalDefSystemTreeNodeDomain_struct
{
    OTF2_SystemTreeNodeRef system_tree_node;
    OTF2_SystemTreeDomain  system_tree_domain;
} OTF2_GlobalDefSystemTreeNodeDomain;


/** @brief LocationGroupProperty definition.
 */
typedef struct OTF2_GlobalDefLocationGroupProperty_struct
{
    OTF2_LocationGroupRef location_group;
    OTF2_StringRef        name;
    OTF2_StringRef        string_value;
    OTF2_Type             type;
    OTF2_AttributeValue   value;
} OTF2_GlobalDefLocationGroupProperty;


/** @brief LocationProperty definition.
 */
typedef struct OTF2_GlobalDefLocationProperty_struct
{
    OTF2_LocationRef    location;
    OTF2_StringRef      name;
    OTF2_StringRef      string_value;
    OTF2_Type           type;
    OTF2_AttributeValue value;
} OTF2_GlobalDefLocationProperty;


/** @brief CartDimension definition.
 */
typedef struct OTF2_GlobalDefCartDimension_struct
{
    OTF2_CartDimensionRef self;
    OTF2_StringRef        name;
    uint32_t              size;
    OTF2_CartPeriodicity  cart_periodicity;
} OTF2_GlobalDefCartDimension;


/** @brief CartTopology definition.
 */
typedef struct OTF2_GlobalDefCartTopology_struct
{
    OTF2_CartTopologyRef   self;
    OTF2_StringRef         name;
    OTF2_CommRef           communicator;
    uint8_t                number_of_dimensions;
    OTF2_CartDimensionRef* cart_dimensions;
} OTF2_GlobalDefCartTopology;


/** @brief CartCoordinate definition.
 */
typedef struct OTF2_GlobalDefCartCoordinate_struct
{
    OTF2_CartTopologyRef cart_topology;
    uint32_t             rank;
    uint8_t              number_of_dimensions;
    uint32_t*            coordinates;
} OTF2_GlobalDefCartCoordinate;


/** @brief SourceCodeLocation definition.
 */
typedef struct OTF2_GlobalDefSourceCodeLocation_struct
{
    OTF2_SourceCodeLocationRef self;
    OTF2_StringRef             file;
    uint32_t                   line_number;
} OTF2_GlobalDefSourceCodeLocation;


/** @brief CallingContext definition.
 */
typedef struct OTF2_GlobalDefCallingContext_struct
{
    OTF2_CallingContextRef     self;
    OTF2_RegionRef             region;
    OTF2_SourceCodeLocationRef source_code_location;
    OTF2_CallingContextRef     parent;
} OTF2_GlobalDefCallingContext;


/** @brief CallingContextProperty definition.
 */
typedef struct OTF2_GlobalDefCallingContextProperty_struct
{
    OTF2_CallingContextRef calling_context;
    OTF2_StringRef         name;
    OTF2_Type              type;
    OTF2_AttributeValue    value;
} OTF2_GlobalDefCallingContextProperty;


/** @brief InterruptGenerator definition.
 */
typedef struct OTF2_GlobalDefInterruptGenerator_struct
{
    OTF2_InterruptGeneratorRef  self;
    OTF2_StringRef              name;
    OTF2_InterruptGeneratorMode interrupt_generator_mode;
    OTF2_Base                   base;
    int64_t                     exponent;
    uint64_t                    period;
} OTF2_GlobalDefInterruptGenerator;


/** @brief IoFileProperty definition.
 */
typedef struct OTF2_GlobalDefIoFileProperty_struct
{
    OTF2_IoFileRef      io_file;
    OTF2_StringRef      name;
    OTF2_Type           type;
    OTF2_AttributeValue value;
} OTF2_GlobalDefIoFileProperty;


/** @brief IoRegularFile definition.
 */
typedef struct OTF2_GlobalDefIoRegularFile_struct
{
    OTF2_IoFileRef         self;
    OTF2_StringRef         name;
    OTF2_SystemTreeNodeRef scope;
} OTF2_GlobalDefIoRegularFile;


/** @brief IoDirectory definition.
 */
typedef struct OTF2_GlobalDefIoDirectory_struct
{
    OTF2_IoFileRef         self;
    OTF2_StringRef         name;
    OTF2_SystemTreeNodeRef scope;
} OTF2_GlobalDefIoDirectory;


/** @brief IoHandle definition.
 */
typedef struct OTF2_GlobalDefIoHandle_struct
{
    OTF2_IoHandleRef   self;
    OTF2_StringRef     name;
    OTF2_IoFileRef     file;
    OTF2_IoParadigmRef io_paradigm;
    OTF2_IoHandleFlag  io_handle_flags;
    OTF2_CommRef       comm;
    OTF2_IoHandleRef   parent;
} OTF2_GlobalDefIoHandle;


/** @brief IoPreCreatedHandleState definition.
 */
typedef struct OTF2_GlobalDefIoPreCreatedHandleState_struct
{
    OTF2_IoHandleRef  io_handle;
    OTF2_IoAccessMode mode;
    OTF2_IoStatusFlag status_flags;
} OTF2_GlobalDefIoPreCreatedHandleState;


/** @brief CallpathParameter definition.
 */
typedef struct OTF2_GlobalDefCallpathParameter_struct
{
    OTF2_CallpathRef    callpath;
    OTF2_ParameterRef   parameter;
    OTF2_Type           type;
    OTF2_AttributeValue value;
} OTF2_GlobalDefCallpathParameter;


struct OTF2_GlobalDefReaderCallbacks_struct
{
    OTF2_GlobalDefReaderCallback_Unknown                 unknown;
    OTF2_GlobalDefReaderCallback_ClockProperties         clock_properties;
    OTF2_GlobalDefReaderCallback_Paradigm                paradigm;
    OTF2_GlobalDefReaderCallback_ParadigmProperty        paradigm_property;
    OTF2_GlobalDefReaderCallback_IoParadigm              io_paradigm;
    OTF2_GlobalDefReaderCallback_String                  string;
    OTF2_GlobalDefReaderCallback_Attribute               attribute;
    OTF2_GlobalDefReaderCallback_SystemTreeNode          system_tree_node;
    OTF2_GlobalDefReaderCallback_LocationGroup           location_group;
    OTF2_GlobalDefReaderCallback_Location                location;
    OTF2_GlobalDefReaderCallback_Region                  region;
    OTF2_GlobalDefReaderCallback_Callsite                callsite;
    OTF2_GlobalDefReaderCallback_Callpath                callpath;
    OTF2_GlobalDefReaderCallback_Group                   group;
    OTF2_GlobalDefReaderCallback_MetricMember            metric_member;
    OTF2_GlobalDefReaderCallback_MetricClass             metric_class;
    OTF2_GlobalDefReaderCallback_MetricInstance          metric_instance;
    OTF2_GlobalDefReaderCallback_Comm                    comm;
    OTF2_GlobalDefReaderCallback_Parameter               parameter;
    OTF2_GlobalDefReaderCallback_RmaWin                  rma_win;
    OTF2_GlobalDefReaderCallback_MetricClassRecorder     metric_class_recorder;
    OTF2_GlobalDefReaderCallback_SystemTreeNodeProperty  system_tree_node_property;
    OTF2_GlobalDefReaderCallback_SystemTreeNodeDomain    system_tree_node_domain;
    OTF2_GlobalDefReaderCallback_LocationGroupProperty   location_group_property;
    OTF2_GlobalDefReaderCallback_LocationProperty        location_property;
    OTF2_GlobalDefReaderCallback_CartDimension           cart_dimension;
    OTF2_GlobalDefReaderCallback_CartTopology            cart_topology;
    OTF2_GlobalDefReaderCallback_CartCoordinate          cart_coordinate;
    OTF2_GlobalDefReaderCallback_SourceCodeLocation      source_code_location;
    OTF2_GlobalDefReaderCallback_CallingContext          calling_context;
    OTF2_GlobalDefReaderCallback_CallingContextProperty  calling_context_property;
    OTF2_GlobalDefReaderCallback_InterruptGenerator      interrupt_generator;
    OTF2_GlobalDefReaderCallback_IoFileProperty          io_file_property;
    OTF2_GlobalDefReaderCallback_IoRegularFile           io_regular_file;
    OTF2_GlobalDefReaderCallback_IoDirectory             io_directory;
    OTF2_GlobalDefReaderCallback_IoHandle                io_handle;
    OTF2_GlobalDefReaderCallback_IoPreCreatedHandleState io_pre_created_handle_state;
    OTF2_GlobalDefReaderCallback_CallpathParameter       callpath_parameter;
};


#endif /* OTF2_INTERNAL_GLOBAL_DEFINITIONS_H */
