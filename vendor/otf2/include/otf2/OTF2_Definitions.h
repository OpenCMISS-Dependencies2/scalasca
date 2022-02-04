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


#ifndef OTF2_DEFINITIONS_H
#define OTF2_DEFINITIONS_H


/**
 *  @file
 *  @source     templates/OTF2_Definitions.tmpl.h
 *
 *  @brief      Data types used in the definition records.
 */


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Wrapper for enum @eref{OTF2_SystemTreeDomain_enum}. */
typedef uint8_t OTF2_SystemTreeDomain;

/**
 * @brief List of available system tree node domains.
 *
 * @since Version 1.2
 */
enum OTF2_SystemTreeDomain_enum
{
    /** @brief All nodes below a node with this attribute encompass a tightly coupled
     *  HPC system.
     */
    OTF2_SYSTEM_TREE_DOMAIN_MACHINE       = 0,
    /** @brief All nodes below a node with this attribute encompass a system where
     *  processes can communicate via hardware shared memory.
     */
    OTF2_SYSTEM_TREE_DOMAIN_SHARED_MEMORY = 1,
    /** @brief A numa domain. A set of processors around memory which the processors
     *  can directly access.
     */
    OTF2_SYSTEM_TREE_DOMAIN_NUMA          = 2,
    /** @brief Socket, physical package, or chip. In the physical meaning, i.e. that
     *  you can add or remove physically.
     */
    OTF2_SYSTEM_TREE_DOMAIN_SOCKET        = 3,
    /** @brief Cache. Can be L1i, L1d, L2, L3, ...
     */
    OTF2_SYSTEM_TREE_DOMAIN_CACHE         = 4,
    /** @brief Core. A computation unit (may be shared by several logical
     *  processors).
     */
    OTF2_SYSTEM_TREE_DOMAIN_CORE          = 5,
    /** @brief Processing Unit (A non-shared ALU, FPU, ...)
     */
    OTF2_SYSTEM_TREE_DOMAIN_PU            = 6
};


/** @brief Wrapper for enum @eref{OTF2_LocationGroupType_enum}. */
typedef uint8_t OTF2_LocationGroupType;

/**
 * @brief List of possible definitions of type LocationGroup.
 *
 * @since Version 1.0
 */
enum OTF2_LocationGroupType_enum
{
    /** @brief A location group of unknown type.
     */
    OTF2_LOCATION_GROUP_TYPE_UNKNOWN = 0,
    /** @brief A process.
     */
    OTF2_LOCATION_GROUP_TYPE_PROCESS = 1
};


/** @brief Wrapper for enum @eref{OTF2_LocationType_enum}. */
typedef uint8_t OTF2_LocationType;

/**
 * @brief List of possible definitions of type Location.
 *
 * @since Version 1.0
 */
enum OTF2_LocationType_enum
{
    /** @brief A location of unknown type.
     */
    OTF2_LOCATION_TYPE_UNKNOWN    = 0,
    /** @brief A CPU thread.
     */
    OTF2_LOCATION_TYPE_CPU_THREAD = 1,
    /** @brief A GPU location.
     */
    OTF2_LOCATION_TYPE_GPU        = 2,
    /** @brief A metric only location e.g. an external device.
     */
    OTF2_LOCATION_TYPE_METRIC     = 3
};


/** @brief Wrapper for enum @eref{OTF2_RegionRole_enum}. */
typedef uint8_t OTF2_RegionRole;

/**
 * @brief List of possible roles of a Region.
 *
 * @since Version 1.1
 */
enum OTF2_RegionRole_enum
{
    /** @brief A region of unknown role.
     */
    OTF2_REGION_ROLE_UNKNOWN          = 0,
    /** @brief An entire function/subroutine.
     */
    OTF2_REGION_ROLE_FUNCTION         = 1,
    /** @brief An API function wrapped by Score-P.
     */
    OTF2_REGION_ROLE_WRAPPER          = 2,
    /** @brief A loop in the code.
     */
    OTF2_REGION_ROLE_LOOP             = 3,
    /** @brief An arbitrary section of code.
     */
    OTF2_REGION_ROLE_CODE             = 4,
    /** @brief E.g. OpenMP "parallel" construct (structured block)
     */
    OTF2_REGION_ROLE_PARALLEL         = 5,
    /** @brief E.g. OpenMP "sections" construct
     */
    OTF2_REGION_ROLE_SECTIONS         = 6,
    /** @brief Individual "section" inside an OpenMP "sections" construct
     */
    OTF2_REGION_ROLE_SECTION          = 7,
    /** @brief E.g. OpenMP "workshare" construct
     */
    OTF2_REGION_ROLE_WORKSHARE        = 8,
    /** @brief E.g. OpenMP "single" construct
     */
    OTF2_REGION_ROLE_SINGLE           = 9,
    /** @brief E.g. OpenMP "single" construct (structured block)
     */
    OTF2_REGION_ROLE_SINGLE_SBLOCK    = 10,
    /** @brief E.g. OpenMP "master" construct
     */
    OTF2_REGION_ROLE_MASTER           = 11,
    /** @brief E.g. OpenMP "critical" construct
     */
    OTF2_REGION_ROLE_CRITICAL         = 12,
    /** @brief E.g. OpenMP "critical" construct (structured block)
     */
    OTF2_REGION_ROLE_CRITICAL_SBLOCK  = 13,
    /** @brief E.g. OpenMP "atomic" construct
     */
    OTF2_REGION_ROLE_ATOMIC           = 14,
    /** @brief Explicit barrier
     */
    OTF2_REGION_ROLE_BARRIER          = 15,
    /** @brief Implicit barrier
     */
    OTF2_REGION_ROLE_IMPLICIT_BARRIER = 16,
    /** @brief E.g. OpenMP "flush" construct
     */
    OTF2_REGION_ROLE_FLUSH            = 17,
    /** @brief E.g. OpenMP "ordered" construct
     */
    OTF2_REGION_ROLE_ORDERED          = 18,
    /** @brief E.g. OpenMP "ordered" construct (structured block)
     */
    OTF2_REGION_ROLE_ORDERED_SBLOCK   = 19,
    /** @brief "task" construct (structured block)
     */
    OTF2_REGION_ROLE_TASK             = 20,
    /** @brief "task" construct (creation)
     */
    OTF2_REGION_ROLE_TASK_CREATE      = 21,
    /** @brief "taskwait" construct
     */
    OTF2_REGION_ROLE_TASK_WAIT        = 22,
    /** @brief Collective 1:N communication operation
     */
    OTF2_REGION_ROLE_COLL_ONE2ALL     = 23,
    /** @brief Collective N:1 communication operation
     */
    OTF2_REGION_ROLE_COLL_ALL2ONE     = 24,
    /** @brief Collective N:N communication operation
     */
    OTF2_REGION_ROLE_COLL_ALL2ALL     = 25,
    /** @brief Collective M:N communication operation
     */
    OTF2_REGION_ROLE_COLL_OTHER       = 26,
    /** @brief A data file I/O operation.
     */
    OTF2_REGION_ROLE_FILE_IO          = 27,
    /** @brief A point-to-point communication function.
     */
    OTF2_REGION_ROLE_POINT2POINT      = 28,
    /** @brief A remote memory access communication operation.
     */
    OTF2_REGION_ROLE_RMA              = 29,
    /** @brief A data transfer operation in memory.
     */
    OTF2_REGION_ROLE_DATA_TRANSFER    = 30,
    /** @brief An artificial region, mostly used by the monitor software.
     *
     *  @since Version 1.2.
     */
    OTF2_REGION_ROLE_ARTIFICIAL       = 31,
    /** @brief A function which creates one thread.
     *
     *  @since Version 1.3.
     */
    OTF2_REGION_ROLE_THREAD_CREATE    = 32,
    /** @brief A function which waits for the completion of one thread.
     *
     *  @since Version 1.3.
     */
    OTF2_REGION_ROLE_THREAD_WAIT      = 33,
    /** @brief "untied task" construct (structured block)
     *
     *  @since Version 1.5.
     */
    OTF2_REGION_ROLE_TASK_UNTIED      = 34,
    /** @brief A function which allocates memory.
     *
     *  @since Version 2.0.
     */
    OTF2_REGION_ROLE_ALLOCATE         = 35,
    /** @brief A function which deallocates memory.
     *
     *  @since Version 2.0.
     */
    OTF2_REGION_ROLE_DEALLOCATE       = 36,
    /** @brief A function which changes a previous memory allocation to either
     *  increase or decrease the amount.
     *
     *  @since Version 2.0.
     */
    OTF2_REGION_ROLE_REALLOCATE       = 37,
    /** @brief A metadata-only file I/O operation (e.g., @eref{IoSeek},
     *  @eref{IoChangeStatusFlags} event).
     *
     *  @since Version 2.1.
     */
    OTF2_REGION_ROLE_FILE_IO_METADATA = 38
};


/** @brief Wrapper for enum @eref{OTF2_RegionFlag_enum}. */
typedef uint32_t OTF2_RegionFlag;

/**
 * @brief List of possible flags to specify special characteristics of a Region.
 *
 * @since Version 1.1
 */
enum OTF2_RegionFlag_enum
{
    /** @brief A region without special characterization.
     */
    OTF2_REGION_FLAG_NONE    = 0,
    /** @brief Each time this region is entered it will get an individual call path
     *  in the profile.
     */
    OTF2_REGION_FLAG_DYNAMIC = ( 1 << 0 ),
    /** @brief Each time this region is entered it will get an individual root node
     *  in the profile.
     */
    OTF2_REGION_FLAG_PHASE   = ( 1 << 1 )
};


/** @brief Wrapper for enum @eref{OTF2_GroupType_enum}. */
typedef uint8_t OTF2_GroupType;

/**
 * @brief List of available group types.
 *
 * @since Version 1.2
 */
enum OTF2_GroupType_enum
{
    /** @brief Group of unknown type.
     */
    OTF2_GROUP_TYPE_UNKNOWN        = 0,
    /** @brief Group of locations.
     */
    OTF2_GROUP_TYPE_LOCATIONS      = 1,
    /** @brief Group of regions.
     */
    OTF2_GROUP_TYPE_REGIONS        = 2,
    /** @brief Group of metrics.
     */
    OTF2_GROUP_TYPE_METRIC         = 3,
    /** @brief List of locations which participated in the paradigm specified by the
     *  group definition.
     *
     *  For example: In case of MPI, the size of this group should match the
     *  size of @a MPI_COMM_WORLD. Each entry in the list is a
     *  @eref{Location} reference, where the index of the entry is
     *  equal to the rank in @a MPI_COMM_WORLD (i.e., rank @a i
     *  corresponds to location <em>members[i]</em>).
     *
     *  Also, if this definition is present, the location group ids of
     *  locations with type @eref{OTF2_LOCATION_TYPE_CPU_THREAD}
     *  should match the MPI rank.
     *
     *  This group needs to be defined, before any group of type
     *  @eref{OTF2_GROUP_TYPE_COMM_GROUP} and the same paradigm.
     *
     *  Note: This does not makes sense in local definitions.
     */
    OTF2_GROUP_TYPE_COMM_LOCATIONS = 4,
    /** @brief A sub-group of the corresponding group definition with type
     *  @eref{OTF2_GROUP_TYPE_COMM_LOCATIONS} and the same paradigm.
     *  The sub-group is formed by listing the indexes of the
     *  @eref{OTF2_GROUP_TYPE_COMM_LOCATIONS} group.
     */
    OTF2_GROUP_TYPE_COMM_GROUP     = 5,
    /** @brief Special group type to efficiently handle self-like communicators
     *  (i.e., MPI_COMM_SELF and friends). At most one definition of
     *  this type is allowed to exist per paradigm.
     */
    OTF2_GROUP_TYPE_COMM_SELF      = 6
};


/** @brief Wrapper for enum @eref{OTF2_GroupFlag_enum}. */
typedef uint32_t OTF2_GroupFlag;

/**
 * @brief List of possible flags to specify special characteristics of a Group.
 *
 * @since Version 1.2
 */
enum OTF2_GroupFlag_enum
{
    /** @brief A group without special characterization.
     */
    OTF2_GROUP_FLAG_NONE           = 0,
    /** @brief No translation of ranks in event records needs to be done when a group
     *  of type @eref{OTF2_GROUP_TYPE_COMM_GROUP} has this flag. I.e.,
     *  the ranks are indexes into the
     *  @eref{OTF2_GROUP_TYPE_COMM_LOCATIONS} group.
     */
    OTF2_GROUP_FLAG_GLOBAL_MEMBERS = ( 1 << 0 )
};


/** @brief Wrapper for enum @eref{OTF2_Base_enum}. */
typedef uint8_t OTF2_Base;

/**
 * @brief Metric base types.
 *
 * @since Version 1.0
 */
enum OTF2_Base_enum
{
    /** @brief Binary base.
     */
    OTF2_BASE_BINARY  = 0,
    /** @brief Decimal base.
     */
    OTF2_BASE_DECIMAL = 1
};


/** @brief Wrapper for enum @eref{OTF2_MetricOccurrence_enum}. */
typedef uint8_t OTF2_MetricOccurrence;

/**
 * @brief Metric occurrence.
 *
 * @since Version 1.0
 */
enum OTF2_MetricOccurrence_enum
{
    /** @brief Metric occurs at every region enter and leave.
     */
    OTF2_METRIC_SYNCHRONOUS_STRICT = 0,
    /** @brief Metric occurs only at a region enter and leave, but does not need to
     *  occur at every enter/leave.
     */
    OTF2_METRIC_SYNCHRONOUS        = 1,
    /** @brief Metric can occur at any place i.e. it is not related to region enter
     *  and leaves.
     */
    OTF2_METRIC_ASYNCHRONOUS       = 2
};


/** @brief Wrapper for enum @eref{OTF2_MetricType_enum}. */
typedef uint8_t OTF2_MetricType;

/**
 * @brief List of available metric types.
 *
 * @since Version 1.0
 */
enum OTF2_MetricType_enum
{
    /** @brief Any metric of a type not explicitly listed below.
     */
    OTF2_METRIC_TYPE_OTHER  = 0,
    /** @brief PAPI counter.
     */
    OTF2_METRIC_TYPE_PAPI   = 1,
    /** @brief Resource usage counter.
     */
    OTF2_METRIC_TYPE_RUSAGE = 2,
    /** @brief User metrics.
     */
    OTF2_METRIC_TYPE_USER   = 3
};


/** @brief Wrapper for enum @eref{OTF2_MetricValueProperty_enum}. */
typedef uint8_t OTF2_MetricValueProperty;

/**
 * @brief Information about whether the metric value is accumulated, absolute,
 * or relative. Used for the lower half-byte of OTF2_MetricMode.
 *
 * @since Version 1.0
 */
enum OTF2_MetricValueProperty_enum
{
    /** @brief Accumulated metric is monotonously increasing (i.e., PAPI counter for
     *  number of executed floating point operations).
     */
    OTF2_METRIC_VALUE_ACCUMULATED = 0,
    /** @brief Absolute metric (i.e., temperature, rate, mean value, etc.).
     */
    OTF2_METRIC_VALUE_ABSOLUTE    = 1,
    /** @brief Relative metric.
     */
    OTF2_METRIC_VALUE_RELATIVE    = 2,

    /** @brief This mask can be used to get lower half-byte in OTF2_MetricMode that
     *  is used to indicate metric value property. xxxx1111
     */
    OTF2_METRIC_VALUE_MASK        = 15
};


/** @brief Wrapper for enum @eref{OTF2_MetricTiming_enum}. */
typedef uint8_t OTF2_MetricTiming;

/**
 * @brief Determines when the values have been collected or for which interval
 * of time they are valid. Used for the upper half-byte of
 * OTF2_MetricMode.
 *
 * @since Version 1.0
 */
enum OTF2_MetricTiming_enum
{
    /** @brief Metric value belongs to the time interval since the beginning of the
     *  measurement.
     */
    OTF2_METRIC_TIMING_START = 0,
    /** @brief Metric value is only valid at a point in time but not necessarily for
     *  any interval of time.
     */
    OTF2_METRIC_TIMING_POINT = 1 << 4,
    /** @brief Metric value is related to the time interval since the last counter
     *  sample of the same metric, i.e. the immediate past.
     */
    OTF2_METRIC_TIMING_LAST  = 2 << 4,
    /** @brief Metric value is valid from now until the next counter sample, i.e. the
     *  future right ahead.
     */
    OTF2_METRIC_TIMING_NEXT  = 3 << 4,

    /** @brief This mask can be used to get the upper half-byte in OTF2_MetricMode
     *  that is used to indicate metric timing information. 1111xxxx
     */
    OTF2_METRIC_TIMING_MASK  = 240
};


/** @brief Wrapper for enum @eref{OTF2_MetricMode_enum}. */
typedef uint8_t OTF2_MetricMode;

/**
 * @brief Metric mode is a combination of value property and timing information.
 *
 * @since Version 1.0
 */
enum OTF2_MetricMode_enum
{
    /** @brief Accumulated metric, 'START' timing.
     */
    OTF2_METRIC_ACCUMULATED_START = OTF2_METRIC_VALUE_ACCUMULATED | OTF2_METRIC_TIMING_START,
    /** @brief Accumulated metric, 'POINT' timing.
     */
    OTF2_METRIC_ACCUMULATED_POINT = OTF2_METRIC_VALUE_ACCUMULATED | OTF2_METRIC_TIMING_POINT,
    /** @brief Accumulated metric, 'LAST' timing.
     */
    OTF2_METRIC_ACCUMULATED_LAST  = OTF2_METRIC_VALUE_ACCUMULATED | OTF2_METRIC_TIMING_LAST,
    /** @brief Accumulated metric, 'NEXT' timing.
     */
    OTF2_METRIC_ACCUMULATED_NEXT  = OTF2_METRIC_VALUE_ACCUMULATED | OTF2_METRIC_TIMING_NEXT,
    /** @brief Absolute metric, 'POINT' timing.
     */
    OTF2_METRIC_ABSOLUTE_POINT    = OTF2_METRIC_VALUE_ABSOLUTE | OTF2_METRIC_TIMING_POINT,
    /** @brief Absolute metric, 'LAST' timing.
     */
    OTF2_METRIC_ABSOLUTE_LAST     = OTF2_METRIC_VALUE_ABSOLUTE | OTF2_METRIC_TIMING_LAST,
    /** @brief Absolute metric, 'NEXT' timing.
     */
    OTF2_METRIC_ABSOLUTE_NEXT     = OTF2_METRIC_VALUE_ABSOLUTE | OTF2_METRIC_TIMING_NEXT,
    /** @brief Relative metric, 'POINT' timing.
     */
    OTF2_METRIC_RELATIVE_POINT    = OTF2_METRIC_VALUE_RELATIVE | OTF2_METRIC_TIMING_POINT,
    /** @brief Relative metric, 'LAST' timing.
     */
    OTF2_METRIC_RELATIVE_LAST     = OTF2_METRIC_VALUE_RELATIVE | OTF2_METRIC_TIMING_LAST,
    /** @brief Relative metric, 'NEXT' timing.
     */
    OTF2_METRIC_RELATIVE_NEXT     = OTF2_METRIC_VALUE_RELATIVE | OTF2_METRIC_TIMING_NEXT
};


/** @brief Wrapper for enum @eref{OTF2_MetricScope_enum}. */
typedef uint8_t OTF2_MetricScope;

/**
 * @brief List of available metric scopes.
 *
 * @since Version 1.0
 */
enum OTF2_MetricScope_enum
{
    /** @brief Scope of a metric is another location.
     */
    OTF2_SCOPE_LOCATION         = 0,
    /** @brief Scope of a metric is a location group.
     */
    OTF2_SCOPE_LOCATION_GROUP   = 1,
    /** @brief Scope of a metric is a system tree node.
     */
    OTF2_SCOPE_SYSTEM_TREE_NODE = 2,
    /** @brief Scope of a metric is a generic group of locations.
     */
    OTF2_SCOPE_GROUP            = 3
};


/** @brief Wrapper for enum @eref{OTF2_RecorderKind_enum}. */
typedef uint8_t OTF2_RecorderKind;

/**
 * @brief List of possible kinds a MetricClass can be recorded by.
 *
 * @since Version 1.2
 */
enum OTF2_RecorderKind_enum
{
    /** @brief No specific kind of recorder.
     */
    OTF2_RECORDER_KIND_UNKNOWN  = 0,
    /** @brief The metric class will only be recorded via a @eref{MetricInstance}
     *  definitions.
     */
    OTF2_RECORDER_KIND_ABSTRACT = 1,
    /** @brief This metric class will only be recorded by locations of type
     *  @eref{OTF2_LOCATION_TYPE_CPU_THREAD}.
     */
    OTF2_RECORDER_KIND_CPU      = 2,
    /** @brief This metric class will only be recorded by locations of type
     *  @eref{OTF2_LOCATION_TYPE_GPU}.
     */
    OTF2_RECORDER_KIND_GPU      = 3
};


/** @brief Wrapper for enum @eref{OTF2_ParameterType_enum}. */
typedef uint8_t OTF2_ParameterType;

/**
 * @brief List of possible for definitions of type Parameter.
 *
 * @since Version 1.0
 */
enum OTF2_ParameterType_enum
{
    /** @brief Parameter is of type string.
     */
    OTF2_PARAMETER_TYPE_STRING = 0,
    /** @brief Parameter is of type signed 8-byte integer.
     */
    OTF2_PARAMETER_TYPE_INT64  = 1,
    /** @brief Parameter is of type unsigned 8-byte integer.
     */
    OTF2_PARAMETER_TYPE_UINT64 = 2
};


/** @brief Wrapper for enum @eref{OTF2_CartPeriodicity_enum}. */
typedef uint8_t OTF2_CartPeriodicity;

/**
 * @brief Periodicity types of a cartesian topology dimension.
 *
 * @since Version 1.3
 */
enum OTF2_CartPeriodicity_enum
{
    /** @brief Dimension is not periodic.
     */
    OTF2_CART_PERIODIC_FALSE = 0,
    /** @brief Dimension is periodic.
     */
    OTF2_CART_PERIODIC_TRUE  = 1
};


/** @brief Wrapper for enum @eref{OTF2_InterruptGeneratorMode_enum}. */
typedef uint8_t OTF2_InterruptGeneratorMode;

/**
 * @brief List of possible modes of @eref{InterruptGenerator}.
 *
 * @since Version 2.0
 */
enum OTF2_InterruptGeneratorMode_enum
{
    /** @brief Denotes that the interrupts generated are time based.
     */
    OTF2_INTERRUPT_GENERATOR_MODE_TIME  = 0,
    /** @brief Denotes that the interrupts generated are count based.
     */
    OTF2_INTERRUPT_GENERATOR_MODE_COUNT = 1
};


/** @brief Wrapper for enum @eref{OTF2_IoParadigmClass_enum}. */
typedef uint8_t OTF2_IoParadigmClass;

/**
 * @brief List of I/O paradigm classes.
 *
 * @since Version 2.1
 */
enum OTF2_IoParadigmClass_enum
{
    /** @brief Serial.
     *
     *  No I/O operations allowed with the
     *  @eref{OTF2_IO_OPERATION_FLAG_COLLECTIVE} flag set and the
     *  @eref{Comm} attribute of the @eref{IoHandle} definition is
     *  always @eref{OTF2_UNDEFINED_COMM}.
     */
    OTF2_IO_PARADIGM_CLASS_SERIAL   = 0,
    /** @brief Parallel.
     *
     *  Collective I/O operations are allowed if the @eref{Comm} attribute of
     *  the @eref{IoHandle} definition is not
     *  @eref{OTF2_UNDEFINED_COMM}.
     */
    OTF2_IO_PARADIGM_CLASS_PARALLEL = 1
};


/** @brief Wrapper for enum @eref{OTF2_IoParadigmFlag_enum}. */
typedef uint32_t OTF2_IoParadigmFlag;

/**
 * @brief List of I/O paradigm flags.
 *
 * @since Version 2.1
 */
enum OTF2_IoParadigmFlag_enum
{
    /** @brief No flags set.
     */
    OTF2_IO_PARADIGM_FLAG_NONE = 0,
    /** @brief This I/O paradigm denotes an interface to the operating system. Thus,
     *  no further I/O paradigms below can exist.
     */
    OTF2_IO_PARADIGM_FLAG_OS   = ( 1 << 0 )
};


/** @brief Wrapper for enum @eref{OTF2_IoParadigmProperty_enum}. */
typedef uint8_t OTF2_IoParadigmProperty;

/**
 * @brief List of I/O paradigm properties.
 *
 * @since Version 2.1
 */
enum OTF2_IoParadigmProperty_enum
{
    /** @brief Version of the implementation.  Optional for all I/O paradigms.
     *
     *  Type: @eref{String}
     */
    OTF2_IO_PARADIGM_PROPERTY_VERSION = 0
};


/** @brief Wrapper for enum @eref{OTF2_IoHandleFlag_enum}. */
typedef uint32_t OTF2_IoHandleFlag;

/**
 * @brief List of possible flags to specify special characteristics of an
 * @eref{IoHandle} definition.
 *
 * @since Version 2.1
 */
enum OTF2_IoHandleFlag_enum
{
    /** @brief No flags set.
     */
    OTF2_IO_HANDLE_FLAG_NONE        = 0,
    /** @brief This I/O handle was already created by the system. No
     *  @eref{IoCreateHandle} event occurs with the handle to operate
     *  on it, thus it is already @emph{active} without such event.
     *
     *  The @eref{IoHandle} definition should be supplemented with an
     *  @eref{IoPreCreatedHandleState} definition to state the access
     *  mode and status flags for this @emph{pre-created}
     *  @eref{IoHandle}.
     */
    OTF2_IO_HANDLE_FLAG_PRE_CREATED = ( 1 << 0 ),
    /** @brief This I/O handle is a proxy to a group of I/O paradigm specific
     *  handles. The referenced @eref{IoFile} definition of such an
     *  @eref{IoHandle} should be @eref{OTF2_UNDEFINED_IO_FILE}.
     *
     *  No @eref{IoCreateHandle}, @eref{IoDestroyHandle},
     *  @eref{IoDuplicateHandle}, @eref{IoChangeStatusFlags} events
     *  are allowed with such an @eref{IoHandle}.
     *
     *  Examples are, a call to @p fflush() with a @p NULL argument, which
     *  means <em>all open output streams</em>; or a call to @p
     *  sync(), which means <em>commit all filesystem caches to
     *  disk</em>.
     */
    OTF2_IO_HANDLE_FLAG_ALL_PROXY   = ( 1 << 1 )
};


/** @brief Wrapper for enum @eref{OTF2_IoAccessMode_enum}. */
typedef uint8_t OTF2_IoAccessMode;

/**
 * @brief Determines how the resulting I/O handle can be accessed with
 * subsequent I/O operations.
 *
 * @since Version 2.1
 */
enum OTF2_IoAccessMode_enum
{
    /** @brief Read-only access.
     *
     *  @ioequivalent{POSIX,O_RDONLY}
     *  @ioequivalent{ISOC,"r"}
     *  @ioequivalent{MPI-IO,MPI_MODE_RDONLY}
     */
    OTF2_IO_ACCESS_MODE_READ_ONLY    = 0,
    /** @brief Write-only access.
     *
     *  @ioequivalent{POSIX,O_WRONLY}
     *  @ioequivalent{ISOC,"w"\, "a"}
     *  @ioequivalent{MPI-IO,MPI_MODE_WRONLY}
     */
    OTF2_IO_ACCESS_MODE_WRITE_ONLY   = 1,
    /** @brief Read-write access.
     *
     *  @ioequivalent{POSIX,O_RDWR}
     *  @ioequivalent{ISOC,"r+"\, "w+"\, "a+"}
     *  @ioequivalent{MPI-IO,MPI_MODE_RDWR}
     */
    OTF2_IO_ACCESS_MODE_READ_WRITE   = 2,
    /** @brief Execute only.
     *
     *  @ioequivalent{POSIX,O_EXEC}
     */
    OTF2_IO_ACCESS_MODE_EXECUTE_ONLY = 3,
    /** @brief Search only.
     *
     *  @ioequivalent{POSIX,O_SEARCH}
     */
    OTF2_IO_ACCESS_MODE_SEARCH_ONLY  = 4
};


/** @brief Wrapper for enum @eref{OTF2_IoStatusFlag_enum}. */
typedef uint32_t OTF2_IoStatusFlag;

/**
 * @brief Special characteristics associated to an @eref{IoHandle} at the time
 * the handle was created with the @eref{IoCreateHandle} event or
 * later changed with the @eref{IoChangeStatusFlags} event.
 *
 * @since Version 2.1
 */
enum OTF2_IoStatusFlag_enum
{
    /** @brief No flag is set.
     */
    OTF2_IO_STATUS_FLAG_NONE            = 0,
    /** @brief Enable close-on-exec flag.
     *
     *  @ioequivalentc{POSIX,O_CLOEXEC\, FD_CLOEXEC,
     *                 listed as a @emph{file descriptor flag}\, though
     *                 @eref{OTF2_IoStatusFlag}s will not be inherited\, thus
     *                 they all behave like @emph{file descriptor flags}}
     *  @ioequivalent{ISOC,"e" (GNU libc extension)}
     */
    OTF2_IO_STATUS_FLAG_CLOSE_ON_EXEC   = 1 << 0,
    /** @brief I/O write operations are automatically performed at the end of the
     *  file.
     *
     *  @ioequivalent{POSIX,O_APPEND}
     *  @ioequivalent{ISOC,"a"\, "a+"}
     *  @ioequivalent{MPI-IO,MPI_MODE_APPEND}
     */
    OTF2_IO_STATUS_FLAG_APPEND          = 1 << 1,
    /** @brief I/O operations (including the creation) will fail if they would block
     *  the issuing process.
     *
     *  @ioequivalent{POSIX,O_NONBLOCK\, O_NDELAY}
     */
    OTF2_IO_STATUS_FLAG_NON_BLOCKING    = 1 << 2,
    /** @brief Enable signal-driven I/O.
     *
     *  @ioequivalent{POSIX,O_ASYNC}
     */
    OTF2_IO_STATUS_FLAG_ASYNC           = 1 << 3,
    /** @brief Write operations on the file will complete according to the
     *  requirements of synchronized I/O file integrity completion
     *  (data and metadata).
     *
     *  @ioequivalent{POSIX,O_SYNC}
     */
    OTF2_IO_STATUS_FLAG_SYNC            = 1 << 4,
    /** @brief Write operations on the file will complete according to the
     *  requirements of synchronized I/O data integrity completion.
     *
     *  @ioequivalent{POSIX,O_DSYNC}
     */
    OTF2_IO_STATUS_FLAG_DATA_SYNC       = 1 << 5,
    /** @brief Instruct I/O operations to reduce caching effects.
     *
     *  @ioequivalent{POSIX,O_DIRECT}
     */
    OTF2_IO_STATUS_FLAG_AVOID_CACHING   = 1 << 6,
    /** @brief Read access to a file won't update its last access time.
     *
     *  @ioequivalent{POSIX,O_NOATIME}
     */
    OTF2_IO_STATUS_FLAG_NO_ACCESS_TIME  = 1 << 7,
    /** @brief Delete the file when closing the @eref{IoHandle}.
     *
     *  Though the @eref{IoDestroyHandle} event does not imply the deletion. A
     *  separate @eref{IoDeleteFile} event must be issued for this.
     *
     *  @ioequivalent{MPI-IO,MPI_MODE_DELETE_ON_CLOSE}
     */
    OTF2_IO_STATUS_FLAG_DELETE_ON_CLOSE = 1 << 8
};


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OTF2_DEFINITIONS_H */
