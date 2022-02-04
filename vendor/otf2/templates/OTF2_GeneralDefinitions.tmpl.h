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
 * Copyright (c) 2009-2013,
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


#ifndef OTF2_GENERAL_DEFINITIONS_H
#define OTF2_GENERAL_DEFINITIONS_H


/**
 *  @file
 *  @source     templates/OTF2_GeneralDefinitions.tmpl.h
 *
 *  @brief      This header file provides general definitions which should be
 *              accessible in all internal and external modules.
 */


#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ___ OTF2 library version information _____________________________________ */


/** @name OTF2 library version.
 *  @{ */
@otf2 with v = version
/** @brief Major version number of this OTF2 version. */
#define OTF2_VERSION_MAJOR  @@v.major@@
/** @brief Minor version number of this OTF2 version. */
#define OTF2_VERSION_MINOR  @@v.minor@@
/** @brief Bugfix version number of this OTF2 version. */
#define OTF2_VERSION_BUGFIX @@v.bugfix@@
/** @brief Any string suffix of this OTF2 version. */
#define OTF2_VERSION_SUFFIX "@@v.suffix@@"
/** @brief The OTF2 version as string. */
#define OTF2_VERSION        "@@v@@"
@otf2 endwith
/** @} */


/* ___ Symbols for undefined values _________________________________________ */


/** @name Standard undefined values for basic data types.
 *  @{ */
/** @brief Undefined value for type @a uint8_t. */
#define OTF2_UNDEFINED_UINT8  ( ( uint8_t )( ~( ( uint8_t )0u ) ) )
/** @brief Undefined value for type @a int8_t. */
#define OTF2_UNDEFINED_INT8  ( ( int8_t )( ~( OTF2_UNDEFINED_UINT8 >> 1 ) ) )
/** @brief Undefined value for type @a uint16_t. */
#define OTF2_UNDEFINED_UINT16 ( ( uint16_t )( ~( ( uint16_t )0u ) ) )
/** @brief Undefined value for type @a int16_t. */
#define OTF2_UNDEFINED_INT16  ( ( int16_t )( ~( OTF2_UNDEFINED_UINT16 >> 1 ) ) )
/** @brief Undefined value for type @a uint32_t. */
#define OTF2_UNDEFINED_UINT32 ( ( uint32_t )( ~( ( uint32_t )0u ) ) )
/** @brief Undefined value for type @a int32_t. */
#define OTF2_UNDEFINED_INT32  ( ( int32_t )( ~( OTF2_UNDEFINED_UINT32 >> 1 ) ) )
/** @brief Undefined value for type @a uint64_t. */
#define OTF2_UNDEFINED_UINT64 ( ( uint64_t )( ~( ( uint64_t )0u ) ) )
/** @brief Undefined value for type @a int64_t. */
#define OTF2_UNDEFINED_INT64  ( ( int64_t )( ~( OTF2_UNDEFINED_UINT64 >> 1 ) ) )
/** @} */


/** @brief Undefined value for enums */
#define OTF2_UNDEFINED_TYPE OTF2_UNDEFINED_UINT8


/** @brief OTF2 time stamp. */
typedef uint64_t OTF2_TimeStamp;
/** @brief Undefined value for @eref{OTF2_TimeStamp}. */
#define OTF2_UNDEFINED_TIMESTAMP OTF2_UNDEFINED_UINT64

@otf2 for def in defs|referable_defs|first_born_defs:

/** @brief Type used to indicate a reference to a @eref{@@def.siblings|join('}, or a @eref{')@@} definition */
typedef @@def.self_attr.type.c_orig@@ @@def.self_attr.type@@;
/** @brief The invalid value for a reference to a @eref{@@def.siblings|join('}, or a @eref{')@@} definition */
#define OTF2_UNDEFINED_@@def.ref_upper@@ ( ( @@def.self_attr.type@@ )OTF2_UNDEFINED_@@def.self_attr.type.otf2|upper@@ )

@otf2 endfor

/* ___ Declarations of opaque OTF2 objects __________________________________ */


/** @brief OTF2 local event reader handle. */
typedef struct OTF2_EvtReader_struct OTF2_EvtReader;


/** @brief OTF2 global event reader handle. */
typedef struct OTF2_GlobalEvtReader_struct OTF2_GlobalEvtReader;


/** @brief OTF2 local definition reader handle. */
typedef struct OTF2_DefReader_struct OTF2_DefReader;


/** @brief OTF2 global definition reader handle. */
typedef struct OTF2_GlobalDefReader_struct OTF2_GlobalDefReader;


/** @brief OTF2 local snap reader handle. */
typedef struct OTF2_SnapReader_struct OTF2_SnapReader;


/** @brief OTF2 global snap reader handle. */
typedef struct OTF2_GlobalSnapReader_struct OTF2_GlobalSnapReader;


/** @brief OTF2 marker reader handle. */
typedef struct OTF2_MarkerReader_struct OTF2_MarkerReader;


/* ___ Definitions related to the file interaction __________________________ */


/** @brief Defines the minimum size of a chunk.*/
#define OTF2_CHUNK_SIZE_MIN ( uint64_t )( 256 * 1024 )


/** @brief Defines the maximum size of a chunk. */
#define OTF2_CHUNK_SIZE_MAX ( uint64_t )( 1024 * 1024 * 16 )


/** @brief Defines how to interact with files.
 *  Please see @eref{OTF2_FileMode_enum} for a detailed description. */
typedef uint8_t OTF2_FileMode;


/** @brief Defines how to interact with files. */
enum OTF2_FileMode_enum
{
    /** Open a file in write-only mode. */
    OTF2_FILEMODE_WRITE  = 0,
    /** Open a file in read-only mode. */
    OTF2_FILEMODE_READ   = 1,
    /** Open a file in write-read mode. */
    OTF2_FILEMODE_MODIFY = 2
};


/** @brief Defines which compression is used.
 *  Please see @eref{OTF2_Compression_enum} for a detailed description. */
typedef uint8_t OTF2_Compression;


/** @brief Defines which compression is used. */
enum OTF2_Compression_enum
{
    /** @brief Undefined. */
    OTF2_COMPRESSION_UNDEFINED = 0,
    /** @brief No compression is used. */
    OTF2_COMPRESSION_NONE      = 1,
    /** @brief Use zlib compression. */
    OTF2_COMPRESSION_ZLIB      = 2
};


/** @brief Return value to indicate that the record reading should be
 *         interrupted.
 *
 * Returning @eref{OTF2_CALLBACK_INTERRUPT} will stop reading more events, if functions
 * like:
 *
 * - @eref{OTF2_Reader_ReadLocalEvents}
 * - @eref{OTF2_Reader_ReadAllLocalEvents}
 * - @eref{OTF2_Reader_ReadLocalEventsBackward}
 * - @eref{OTF2_Reader_ReadGlobalEvents}
 * - @eref{OTF2_Reader_ReadAllGlobalEvents}
 * - @eref{OTF2_Reader_ReadLocalDefinitions}
 * - @eref{OTF2_Reader_ReadAllLocalDefinitions}
 * - @eref{OTF2_Reader_ReadGlobalDefinitions}
 * - @eref{OTF2_Reader_ReadAllGlobalDefinitions}
 *
 * where called. The return value for these functions is
 * @eref{OTF2_ERROR_INTERRUPTED_BY_CALLBACK} in this case. It is valid
 * to call any reader functions in such a condition again.
 *
 * This type is also used as return type in the collective and locking callbacks
 * (see @ref callbacks_collective and @ref callbacks_locking). Any value
 * different than @eref{OTF2_CALLBACK_SUCCESS} is treated as an error and the
 * calling function will return @eref{OTF2_ERROR_COLLECTIVE_CALLBACK} or
 * @eref{OTF2_ERROR_LOCKING_CALLBACK} to its caller, respectively. As the name
 * @eref{OTF2_CALLBACK_INTERRUPT} does not really fit in this context, the alias
 * @eref{OTF2_CALLBACK_ERROR} is provided for these callbacks.
 */
typedef enum OTF2_CallbackCode_enum
{
    /** @brief Record reading can continue. */
    OTF2_CALLBACK_SUCCESS   = 0,

    /** @brief Interrupt record reading.
     *
     *  Control returns to the caller of the read function
     *  with error OTF2_ERROR_INTERRUPTED_BY_CALLBACK to signal this.
     *  The actual value can be any except OTF2_CALLBACK_SUCCESS.
     */
    OTF2_CALLBACK_INTERRUPT = !OTF2_CALLBACK_SUCCESS,

    /** @brief Signaling an error in the callback. */
    OTF2_CALLBACK_ERROR     = !OTF2_CALLBACK_SUCCESS
} OTF2_CallbackCode;


/* ___ Definitions related to the memory buffer ______________________________ */


/** @brief Defines whether the recorded data is flushed to a file or not. Please
    see OTF2_FlushType_enum for a detailed description. */
typedef uint8_t OTF2_FlushType;


/** @brief Defines whether the recorded data is flushed to a file or not. */
enum OTF2_FlushType_enum
{
    /** @brief Flushing will be suppressed when running out of memory. */
    OTF2_NO_FLUSH = 0,
    /** @brief Recorded data is flushed when running out of memory. */
    OTF2_FLUSH    = 1
};


/** @brief Wrapper for enum @eref{OTF2_Hint_enum}. */
typedef uint8_t OTF2_Hint;

/**
 * @brief List of possible hints.
 *
 * @since Version 1.5
 */
enum OTF2_Hint_enum
{
    /** Hint the reader that the user will use the global reader to read
     *  per-location data (e.g., event and snapshot data).
     *
     *  In case of the SIONlib substrate that means the SION handles of the
     *  per-location local reader are not duplicated and thus not thread safe.
     *
     *  Datatype @eref{OTF2_Boolean} with default value @eref{OTF2_FALSE}.
     *
     *  This is for an @eref{OTF2_Archive} only valid if the file mode equals to
     *  @eref{OTF2_FILEMODE_READ}.
     *
     *  The hint will be locked when opening any of the per-location data files.
     */
    OTF2_HINT_GLOBAL_READER = 0
};

@otf2 for enum in enums|general_enums:

/** @brief Wrapper for enum @eref{@@enum.type@@_enum}. */
typedef @@enum.type.c_orig@@ @@enum.type@@;

/**
 * @brief @@enum.doxygendoc(prefix=' * ')@@
 *
 * @since Version @@enum.since@@
 */
enum @@enum.type@@_enum
{
    @otf2 for entry in enum.entries:
    /** @brief @@entry.doxygendoc(prefix='     *  ')@@
     */
    @@entry.name@@ = @@entry.value@@@@enum.need_comma(entry)@@
    @otf2 endfor
    @otf2 if enum is enum_with_terminator:

    /** @brief @@enum.terminator.doxygendoc(prefix='     *  ')@@
     */
    @@enum.terminator.name@@ = @@enum.terminator.value@@
    @otf2 endif
};

@otf2 for alias in enum.aliases

/** @brief Alias for @eref{@@enum.type@@}. */
typedef @@enum.type@@ @@alias@@;

@otf2 endfor
@otf2 endfor

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_GENERAL_DEFINITIONS_H */
