/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "epk_memory.h"


/*--- Header file documentation -------------------------------------------*/

/**
 * @addtogroup EPIK_utils_misc
 * @{
 */
/**
 * @file  epk_memory.h
 * @brief Memory-related routines
 *
 * This file provides function prototypes for memory-related routines,
 * such as determining the maximum memory usage of the application in an
 * platform-independent way.
 */
/** @} */


#if defined(__blrts__)
    /*---------------------------------------------------------------------*/
    /* IBM Blue Gene/L                                                     */
    /*---------------------------------------------------------------------*/

    #include <stddef.h>
    #include <unistd.h>


    int
    epk_memusage()
    {
        ptrdiff_t usage = (ptrdiff_t)sbrk(0);

        return (usage / 1024);
    }
#elif defined(__bgp__) || defined(_AIX) || defined(_SX)
    /*---------------------------------------------------------------------*/
    /* IBM Blue Gene/P, IBM AIX systems & NEC-SX                           */
    /*---------------------------------------------------------------------*/

    #include <sys/resource.h>
    #include <sys/times.h>


    int
    epk_memusage()
    {
        struct rusage usage;

        if (getrusage(RUSAGE_SELF, &usage) != 0)
        {
            return -1;
        }

        return usage.ru_maxrss;
    }
#elif defined(__bgq__)
    /*---------------------------------------------------------------------*/
    /* IBM Blue Gene/Q                                                     */
    /*---------------------------------------------------------------------*/

    #include <spi/include/kernel/memory.h>


    int
    epk_memusage()
    {
        uint64_t heap;
        uint64_t stack;

        Kernel_GetMemorySize(KERNEL_MEMSIZE_HEAP, &heap);
        Kernel_GetMemorySize(KERNEL_MEMSIZE_STACK, &stack);

        return ((heap + stack) / 1024);
    }
#elif defined(__LIBCATAMOUNT__)
    /*---------------------------------------------------------------------*/
    /* Cray XT (catamount)                                                 */
    /*---------------------------------------------------------------------*/

    #include <stddef.h>

    #include <catamount/catmalloc.h>


    int
    epk_memusage()
    {
        size_t        fragments;
        unsigned long total_free, largest_free, total_used;

        if (heap_info(&fragments, &total_free, &largest_free, &total_used) != 0)
        {
            return -1;
        }

        return (total_used / 1024);
    }
#elif defined(__sun) && defined(__SVR4)
    /*---------------------------------------------------------------------*/
    /* Sun Solaris systems                                                 */
    /*---------------------------------------------------------------------*/

    #include <fcntl.h>
    #include <procfs.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>


    int
    epk_memusage()
    {
        int           fd;
        ssize_t       bytes;
        struct psinfo info;

        /* Open "/proc" filesystem and read process information */
        fd = open("/proc/self/psinfo", O_RDONLY);
        if (fd == -1)
        {
            return -1;
        }
        bytes = read(fd, &info, sizeof(struct psinfo));
        close(fd);
        if (bytes < sizeof(struct psinfo))
        {
            return -1;
        }

        return info.pr_rssize;
    }
#elif defined(__linux__)
    /*---------------------------------------------------------------------*/
    /* Generic Linux-based systems                                         */
    /*---------------------------------------------------------------------*/

    #include <stdio.h>


    int
    epk_memusage()
    {
        FILE* fp;
        int   kbytes = -1;

        fp = fopen("/proc/self/status", "r");
        if (fp)
        {
            char line[1024];
            while (fgets(line, sizeof(line), fp))
            {
                if (sscanf(line, "VmHWM: %d", &kbytes) == 1)
                {
                    break;
                }
            }
            fclose(fp);
        }

        return kbytes;
    }
#elif defined(__MACH__) && defined(__APPLE__)
    /*---------------------------------------------------------------------*/
    /* Apple Mac OS X                                                      */
    /*---------------------------------------------------------------------*/

    #include <mach/mach.h>
    #include <mach/task_info.h>


    int
    epk_memusage()
    {
        struct task_basic_info info;
        mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;

        if (task_info(mach_task_self(), TASK_BASIC_INFO,
                      (task_info_t)&info, &count) != KERN_SUCCESS)
        {
            return -1;
        }

        return (info.resident_size / 1024);
    }
#else
    /*---------------------------------------------------------------------*/
    /* Unsupported architecture                                            */
    /*---------------------------------------------------------------------*/

    /**
     * Determines the maximum memory usage of the application in kilobytes. On
     * systems providing virtual memory, only the resident portion is considered.
     * Depending on the architecture, the value returned by this function is an
     * estimate rather than the exact value, with a granularity of usually a few
     * kilobytes.
     *
     * @return Memory usage in kilobytes upon successful completion. Otherwise,
     *         a value of -1 is returned.
     *
     * @todo On Mac, this function reports the current memory usage instead of
     *       the maximum. The value reported on Solairs is unclear and should be
     *       checked.
     */
    int
    epk_memusage()
    {
        return -1;
    }
#endif
