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
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "elg_readcb.h"

#include <stdlib.h>
#include <string.h>

#define SCALASCA_DEBUG_MODULE_NAME    EPILOG
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include "elg_rw.h"


struct ElgRCB_struct
{
    ElgIn*   in;
    elg_ui1  metc;
    elg_ui4  single_lid;
    elg_ui8* metv;
    elg_ui1  activated[ELG_MAX_EVENT];
    elg_ui4  version;
};


ElgRCB*
elg_read_open(const char* path)
{
    int i;

    ElgRCB* handle = calloc(1, sizeof(ElgRCB));
    if (handle == NULL)
    {
        UTILS_WARNING("Can't allocate RCB handle");

        return NULL;
    }
    if ((handle->in = ElgIn_open(path)) == NULL)
    {
        UTILS_WARNING("Can't open file %s", path);
        free(handle);

        return NULL;
    }
    for (i = 0; i < ELG_ALL_EVENTS; i++)
    {
        handle->activated[i] = 1;
    }
    handle->metc    = 255;
    handle->version = ElgIn_get_version(handle->in);

    return handle;
}


void
elg_read_close(ElgRCB* handle)
{
    ElgIn_close(handle->in);
    if (handle->metc < 255)
    {
        free(handle->metv);
    }
    free(handle);
}


elg_ui4
elg_read_version(ElgRCB* handle)
{
    return ElgIn_get_version(handle->in);
}


int
elg_read_dispatch(ElgRCB* handle,
                  ElgRec* rec,
                  void*   userdata)
{
    elg_ui1 type, length;

    length = ElgRec_get_length(rec);
    type   = ElgRec_get_type(rec);

    elg_readcb_ALL(type, length, userdata);

    if (handle->activated[type])
    {
        switch (type)
        {
            /* -- Definition records -- */

            case ELG_STRING:
                {
                    elg_ui4 strid = ElgRec_read_ui4(rec);
                    elg_ui1 cntc  = ElgRec_read_ui1(rec);
                    char*   str   = ElgRec_read_string(rec);

                    elg_readcb_STRING(strid, cntc, str, userdata);
                    break;
                }

            case ELG_STRING_CNT:
                {
                    char* str = ElgRec_read_string(rec);

                    elg_readcb_STRING_CNT(str, userdata);
                    break;
                }

            case ELG_MACHINE:
                {
                    elg_ui4 mid   = ElgRec_read_ui4(rec);
                    elg_ui4 nodec = ElgRec_read_ui4(rec);
                    elg_ui4 mnid  = ElgRec_read_ui4(rec);

                    elg_readcb_MACHINE(mid, nodec, mnid, userdata);
                    break;
                }

            case ELG_NODE:
                {
                    elg_ui4 nid  = ElgRec_read_ui4(rec);
                    elg_ui4 mid  = ElgRec_read_ui4(rec);
                    elg_ui4 cpuc = ElgRec_read_ui4(rec);
                    elg_ui4 nnid = ElgRec_read_ui4(rec);
                    /* cr field was added with version 1.1 */
                    elg_d8 cr = (handle->version >= 1001
                                 ? ElgRec_read_d8(rec)
                                 : 1.0);

                    elg_readcb_NODE(nid, mid, cpuc, nnid, cr, userdata);
                    break;
                }

            case ELG_PROCESS:
                {
                    elg_ui4 pid  = ElgRec_read_ui4(rec);
                    elg_ui4 pnid = ElgRec_read_ui4(rec);

                    elg_readcb_PROCESS(pid, pnid, userdata);
                    break;
                }

            case ELG_THREAD:
                {
                    elg_ui4 tid  = ElgRec_read_ui4(rec);
                    elg_ui4 pid  = ElgRec_read_ui4(rec);
                    elg_ui4 tnid = ElgRec_read_ui4(rec);

                    elg_readcb_THREAD(tid, pid, tnid, userdata);
                    break;
                }

            case ELG_LOCATION:
                {
                    elg_ui4 lid = ElgRec_read_ui4(rec);
                    elg_ui4 mid = ElgRec_read_ui4(rec);
                    elg_ui4 nid = ElgRec_read_ui4(rec);
                    elg_ui4 pid = ElgRec_read_ui4(rec);
                    elg_ui4 tid = ElgRec_read_ui4(rec);

                    handle->single_lid = lid;

                    elg_readcb_LOCATION(lid, mid, nid, pid, tid, userdata);
                    break;
                }

            case ELG_FILE:
                {
                    elg_ui4 fid  = ElgRec_read_ui4(rec);
                    elg_ui4 fnid = ElgRec_read_ui4(rec);

                    elg_readcb_FILE(fid, fnid, userdata);
                    break;
                }

            case ELG_REGION:
                {
                    elg_ui4 rid   = ElgRec_read_ui4(rec);
                    elg_ui4 rnid  = ElgRec_read_ui4(rec);
                    elg_ui4 fid   = ElgRec_read_ui4(rec);
                    elg_ui4 begln = ElgRec_read_ui4(rec);
                    elg_ui4 endln = ElgRec_read_ui4(rec);
                    elg_ui4 rdid  = ElgRec_read_ui4(rec);
                    elg_ui1 rtype = ElgRec_read_ui1(rec);

                    elg_readcb_REGION(rid, rnid, fid, begln, endln, rdid, rtype, userdata);
                    break;
                }

            case ELG_CALL_SITE:
                {
                    elg_ui4 csid = ElgRec_read_ui4(rec);
                    elg_ui4 fid  = ElgRec_read_ui4(rec);
                    elg_ui4 lno  = ElgRec_read_ui4(rec);
                    elg_ui4 erid = ElgRec_read_ui4(rec);
                    elg_ui4 lrid = ElgRec_read_ui4(rec);

                    elg_readcb_CALL_SITE(csid, fid, lno, erid, lrid, userdata);
                    break;
                }

            case ELG_CALL_PATH:
                {
                    elg_ui4 cpid  = ElgRec_read_ui4(rec);
                    elg_ui4 rid   = ElgRec_read_ui4(rec);
                    elg_ui4 ppid  = ElgRec_read_ui4(rec);
                    elg_ui8 order = ElgRec_read_ui8(rec);

                    elg_readcb_CALL_PATH(cpid, rid, ppid, order, userdata);
                    break;
                }

            case ELG_METRIC:
                {
                    elg_ui4 metid    = ElgRec_read_ui4(rec);
                    elg_ui4 metnid   = ElgRec_read_ui4(rec);
                    elg_ui4 metdid   = ElgRec_read_ui4(rec);
                    elg_ui1 metdtype = ElgRec_read_ui1(rec);
                    elg_ui1 metmode  = ElgRec_read_ui1(rec);
                    elg_ui1 metiv    = ElgRec_read_ui1(rec);

                    elg_readcb_METRIC(metid, metnid, metdid, metdtype, metmode,
                                      metiv, userdata);
                    if (handle->metc == 255)
                    {
                        handle->metc = 0;
                    }
                    handle->metc++;
                    handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                    if (handle->metv == NULL)
                    {
                        UTILS_FATAL("Can't allocate metric data buffer");
                    }
                    break;
                }

            case ELG_MPI_GROUP:
                {
                    elg_ui4  i;
                    elg_ui4  pos       = 0;
                    elg_ui4  gid       = ElgRec_read_ui4(rec);
                    elg_ui1  mode      = ElgRec_read_ui1(rec);
                    elg_ui4  grpc      = ElgRec_read_ui4(rec);
                    elg_ui4* grpv      = NULL;
                    elg_ui4  maxLength = (255 - sizeof(gid) - sizeof(mode) - sizeof(grpc))
                                         / sizeof(elg_ui4);
                    elg_ui4 blockLength = (grpc > maxLength) ? maxLength : grpc;

                    /* Only allocate buffer if grpc is non-zero */
                    if (grpc > 0)
                    {
                        grpv = calloc(grpc, sizeof(elg_ui4));
                        if (grpv == NULL)
                        {
                            UTILS_FATAL("Can't allocate group data buffer");
                        }
                    }

                    /* Special case: GROUP_WORLD */
                    if (mode & ELG_GROUP_WORLD)
                    {
                        for (i = 0; i < grpc; i++)
                        {
                            grpv[i] = i;
                        }
                    }
                    else
                    {
                        /* Read rank mapping */
                        for (i = 0; i < blockLength; i++)
                        {
                            grpv[pos] = ElgRec_read_ui4(rec);
                            pos++;
                        }

                        /* Potentially read continuation records */
                        maxLength = 255 / sizeof(elg_ui4);
                        while (pos < grpc)
                        {
                            ElgRec* contRec = ElgIn_read_record(handle->in);

                            /* Validate record type */
                            if (ElgRec_get_type(contRec) != ELG_MPI_GROUP_CNT)
                            {
                                UTILS_FATAL("MPI_GROUP_CNT record expected.");
                            }

                            /* Determine number of entries in this record */
                            blockLength = grpc - pos;
                            if (blockLength > maxLength)
                            {
                                blockLength = maxLength;
                            }

                            for (i = 0; i < blockLength; i++)
                            {
                                grpv[pos] = ElgRec_read_ui4(contRec);
                                pos++;
                            }

                            ElgRec_free(contRec);
                        }
                    }

                    elg_readcb_MPI_GROUP(gid, mode, grpc, grpv, userdata);
                    free(grpv);
                    break;
                }

            case ELG_MPI_COMM_DIST:
                {
                    elg_ui4 cid   = ElgRec_read_ui4(rec);
                    elg_ui4 root  = ElgRec_read_ui4(rec);
                    elg_ui4 lcid  = ElgRec_read_ui4(rec);
                    elg_ui4 lrank = ElgRec_read_ui4(rec);
                    elg_ui4 size  = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_COMM_DIST(cid, root, lcid, lrank, size, userdata);
                    break;
                }

            case ELG_MPI_COMM_REF:
                {
                    elg_ui4 cid = ElgRec_read_ui4(rec);
                    elg_ui4 gid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_COMM_REF(cid, gid, userdata);
                    break;
                }

            case ELG_MPI_COMM:
                {
                    elg_ui4 cid = ElgRec_read_ui4(rec);
                    /* mode field (formerly 'uag flag') was added with version 1.3 */
                    elg_ui1 mode = (handle->version < 1003)
                                   ? 0
                                   : ElgRec_read_ui1(rec);
                    elg_ui4  grpc = ElgRec_read_ui4(rec);
                    elg_ui1* grpv = NULL;
                    elg_ui4  pos  = 0;
                    /* mode field (formerly 'uag flag') was added with version 1.3 */
                    elg_ui4 maxLength = 255 - sizeof(cid) - sizeof(grpc)
                                        - ((handle->version < 1003)
                                           ? 0
                                           : sizeof(mode));
                    elg_ui4 blockLength = (grpc > maxLength) ? maxLength : grpc;
                    elg_ui4 i;

                    if (grpc > 0)
                    {
                        /* only allocate buffer if grpc is non-zero */
                        /* NOTE: this ensures that grpv is NULL on all platforms, if
                         *       grpc is zero. */
                        grpv = calloc(grpc, sizeof(elg_ui1));

                        if (grpv == NULL)
                        {
                            UTILS_FATAL("Can't allocate communicator data buffer");
                        }
                    }

                    for (i = 0; i < blockLength; i++)
                    {
                        grpv[pos++] = ElgRec_read_ui1(rec);
                    }

                    /* Read continuation records */
                    while (pos < grpc)
                    {
                        ElgRec* contRec = ElgIn_read_record(handle->in);
                        if (ElgRec_get_type(contRec) != ELG_MPI_COMM_CNT)
                        {
                            UTILS_FATAL("MPI_COMM_CNT record expected.");
                        }

                        blockLength = grpc - pos;
                        if (blockLength > 255)
                        {
                            blockLength = 255;
                        }
                        for (i = 0; i < blockLength; i++)
                        {
                            grpv[pos++] = ElgRec_read_ui1(contRec);
                        }

                        ElgRec_free(contRec);
                    }

                    elg_readcb_MPI_COMM(cid, mode, grpc, grpv, userdata);
                    free(grpv);
                    break;
                }

            case ELG_CART_TOPOLOGY:
                {
                    elg_ui4  topid = ElgRec_read_ui4(rec);
                    elg_ui4  tnid  = ELG_NO_ID;
                    elg_ui4  cid   = ElgRec_read_ui4(rec);
                    elg_ui1  ndims = ElgRec_read_ui1(rec);
                    elg_ui4* dimv;
                    elg_ui1* periodv;
                    elg_ui4* dimids;
                    elg_ui4  i;

                    dimv = calloc(ndims, sizeof(elg_ui4));
                    if (dimv == NULL)
                    {
                        UTILS_FATAL("Can't allocate dimension vector data buffer");
                    }
                    for (i = 0; i < ndims; i++)
                    {
                        dimv[i] = ElgRec_read_ui4(rec);
                    }

                    periodv = calloc(ndims, sizeof(elg_ui1));
                    if (periodv == NULL)
                    {
                        UTILS_FATAL("Can't allocate period vector data buffer");
                    }
                    for (i = 0; i < ndims; i++)
                    {
                        periodv[i] = ElgRec_read_ui1(rec);
                    }


                    /*
                     * After 1.9, files have topology name id and dimensions names ids
                     */
                    dimids = calloc(ndims, sizeof(elg_ui4));
                    if (dimids == NULL)
                    {
                        UTILS_FATAL("Can't allocate dimension id names vector data buffer");
                    }
                    if (handle->version >= 1009)
                    {
                        tnid = ElgRec_read_ui4(rec);
                        for (i = 0; i < ndims; i++)
                        {
                            dimids[i] = ElgRec_read_ui4(rec);
                        }
                    }
                    else
                    {
                        for (i = 0; i < ndims; i++)
                        {
                            dimids[i] = ELG_NO_ID;
                        }
                    }
                    elg_readcb_CART_TOPOLOGY(topid, tnid, cid, ndims, dimv, periodv, dimids, userdata);
                    free(dimv);
                    free(periodv);
                    free(dimids);
                    break;
                }

            case ELG_CART_COORDS:
                {
                    elg_ui4  topid  = ElgRec_read_ui4(rec);
                    elg_ui4  lid    = ElgRec_read_ui4(rec);
                    elg_ui1  ndims  = ElgRec_read_ui1(rec);
                    elg_ui4* coordv = calloc(ndims, sizeof(elg_ui4));
                    elg_ui4  i;

                    if (coordv == NULL)
                    {
                        UTILS_FATAL("Can't allocate coordinate vector data buffer");
                    }
                    for (i = 0; i < ndims; i++)
                    {
                        coordv[i] = ElgRec_read_ui4(rec);
                    }

                    elg_readcb_CART_COORDS(topid, lid, ndims, coordv, userdata);
                    free(coordv);
                    break;
                }

            case ELG_OFFSET:
                {
                    elg_d8 ltime  = ElgRec_read_d8(rec);
                    elg_d8 offset = ElgRec_read_d8(rec);

                    elg_readcb_OFFSET(ltime, offset, userdata);
                    break;
                }

            case ELG_NUM_EVENTS:
                {
                    elg_ui4 eventc = ElgRec_read_ui4(rec);

                    elg_readcb_NUM_EVENTS(eventc, userdata);
                    break;
                }

            case ELG_EVENT_TYPES:
                {
                    elg_ui4  ntypes = ElgRec_read_ui4(rec);
                    elg_ui1* typev  = calloc(ntypes, sizeof(elg_ui1));
                    elg_ui4  i;

                    if (typev == NULL)
                    {
                        UTILS_FATAL("Can't allocate event types vector data buffer");
                    }
                    for (i = 0; i < ntypes; i++)
                    {
                        typev[i] = ElgRec_read_ui1(rec);
                    }

                    elg_readcb_EVENT_TYPES(ntypes, typev, userdata);
                    free(typev);
                    break;
                }

            case ELG_EVENT_COUNTS:
                {
                    elg_ui4  ntypes = ElgRec_read_ui4(rec);
                    elg_ui4* countv = calloc(ntypes, sizeof(elg_ui4));
                    elg_ui4  i;

                    if (countv == NULL)
                    {
                        UTILS_FATAL("Can't allocate event type counts vector data buffer");
                    }
                    for (i = 0; i < ntypes; i++)
                    {
                        countv[i] = ElgRec_read_ui4(rec);
                    }

                    elg_readcb_EVENT_COUNTS(ntypes, countv, userdata);
                    free(countv);
                    break;
                }

            case ELG_MAP_SECTION:
                {
                    elg_ui4 rank = ElgRec_read_ui4(rec);
                    elg_readcb_MAP_SECTION(rank, userdata);
                    break;
                }

            case ELG_MAP_OFFSET:
                {
                    elg_ui4 rank   = ElgRec_read_ui4(rec);
                    elg_ui4 offset = ElgRec_read_ui4(rec);
                    elg_readcb_MAP_OFFSET(rank, offset, userdata);
                    break;
                }

            case ELG_IDMAP:
                {
                    elg_ui1  type  = ElgRec_read_ui1(rec);
                    elg_ui1  mode  = ElgRec_read_ui1(rec);
                    elg_ui4  count = ElgRec_read_ui4(rec);
                    elg_ui4* mapv  = calloc(count, sizeof(elg_ui4));
                    elg_ui4  pos   = 0;
                    elg_ui4  max   = (255 - sizeof(type) - sizeof(mode) - sizeof(count))
                                     / sizeof(elg_ui4);
                    elg_ui4 block = (count > max) ? max : count;
                    elg_ui4 i;

                    if (mapv == NULL)
                    {
                        UTILS_FATAL("Can't allocate IDMAP buffer");
                    }

                    for (i = 0; i < block; i++)
                    {
                        mapv[pos++] = ElgRec_read_ui4(rec);
                    }

                    /* Read continuation records */
                    max = 255 / sizeof(elg_ui4);
                    while (pos < count)
                    {
                        ElgRec* contRec = ElgIn_read_record(handle->in);
                        if (ElgRec_get_type(contRec) != ELG_IDMAP_CNT)
                        {
                            UTILS_FATAL("IDMAP_CNT record expected.");
                        }

                        block = count - pos;
                        if (block > max)
                        {
                            block = max;
                        }
                        for (i = 0; i < block; i++)
                        {
                            mapv[pos++] = ElgRec_read_ui4(contRec);
                        }

                        ElgRec_free(contRec);
                    }

                    elg_readcb_IDMAP(type, mode, count, mapv, userdata);
                    free(mapv);
                    break;
                }

            case ELG_LAST_DEF:
                {
                    elg_readcb_LAST_DEF(userdata);
                    break;
                }

            case ELG_MPI_WIN:
                {
                    elg_ui4 wid = ElgRec_read_ui4(rec);
                    elg_ui4 cid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_WIN(wid, cid, userdata);
                    break;
                }

            case ELG_ATTR_UI1:
                {
                    elg_ui1 type = ElgRec_read_ui1(rec);
                    elg_ui1 val  = ElgRec_read_ui1(rec);

                    elg_readcb_ATTR_UI1(type, val, userdata);
                    break;
                }

            case ELG_ATTR_UI4:
                {
                    elg_ui1 type = ElgRec_read_ui1(rec);
                    elg_ui4 val  = ElgRec_read_ui4(rec);

                    elg_readcb_ATTR_UI4(type, val, userdata);
                    break;
                }

            /* -- Event records -- */

            /* -- Region -- */

            case ELG_ENTER:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 rid;
                    int     i;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time) - sizeof(rid))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(rid)
                                   + (handle->metc * sizeof(elg_ui8))))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    rid  = ElgRec_read_ui4(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }

                    elg_readcb_ENTER(lid, time, rid, handle->metc, handle->metv, userdata);
                    break;
                }

            case ELG_ENTER_CS:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 csid;
                    int     i;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time) - sizeof(csid))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(csid)
                                   + (handle->metc * sizeof(elg_ui8))))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    csid = ElgRec_read_ui4(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }

                    elg_readcb_ENTER_CS(lid, time, csid, handle->metc, handle->metv,
                                        userdata);
                    break;
                }

            case ELG_EXIT:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }

                    elg_readcb_EXIT(lid, time, handle->metc, handle->metv, userdata);
                    break;
                }

            /* -- MPI-1 -- */

            case ELG_MPI_SEND:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 cid;
                    elg_ui4 tag;
                    elg_ui4 sent;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(cid)
                                   + sizeof(tag)
                                   + sizeof(sent)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    dlid = ElgRec_read_ui4(rec);
                    cid  = ElgRec_read_ui4(rec);
                    tag  = ElgRec_read_ui4(rec);
                    sent = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_SEND(lid, time, dlid, cid, tag, sent, userdata);
                    break;
                }

            case ELG_MPI_RECV:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 slid;
                    elg_ui4 cid;
                    elg_ui4 tag;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(slid)
                                   + sizeof(cid)
                                   + sizeof(tag)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    slid = ElgRec_read_ui4(rec);
                    cid  = ElgRec_read_ui4(rec);
                    tag  = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_RECV(lid, time, slid, cid, tag, userdata);
                    break;
                }

            case ELG_MPI_COLLEXIT:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;
                    elg_ui4 rlid;
                    elg_ui4 cid;
                    elg_ui4 sent;
                    elg_ui4 recvd;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time) - sizeof(rlid) - sizeof(cid)
                                        - sizeof(sent) - sizeof(recvd))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))
                                   + sizeof(rlid)
                                   + sizeof(cid)
                                   + sizeof(sent)
                                   + sizeof(recvd)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }
                    rlid  = ElgRec_read_ui4(rec);
                    cid   = ElgRec_read_ui4(rec);
                    sent  = ElgRec_read_ui4(rec);
                    recvd = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_COLLEXIT(lid, time, handle->metc, handle->metv, rlid,
                                            cid, sent, recvd, userdata);
                    break;
                }

            /* -- MPI-1 Non-blocking -- */

            case ELG_MPI_SEND_COMPLETE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 reqid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(reqid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    reqid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_SEND_COMPLETE(lid, time, reqid, userdata);
                    break;
                }

            case ELG_MPI_RECV_REQUEST:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 reqid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(reqid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    reqid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_RECV_REQUEST(lid, time, reqid, userdata);
                    break;
                }

            case ELG_MPI_REQUEST_TESTED:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 reqid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(reqid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    reqid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_REQUEST_TESTED(lid, time, reqid, userdata);
                    break;
                }

            case ELG_MPI_CANCELLED:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 reqid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(reqid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    reqid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_CANCELLED(lid, time, reqid, userdata);
                    break;
                }

            /* -- MPI-2 One-sided Operations -- */

            case ELG_MPI_PUT_1TS:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 wid;
                    elg_ui4 rmaid;
                    elg_ui4 nbytes;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(wid)
                                   + sizeof(rmaid)
                                   + sizeof(nbytes)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time   = ElgRec_read_d8(rec);
                    dlid   = ElgRec_read_ui4(rec);
                    wid    = ElgRec_read_ui4(rec);
                    rmaid  = ElgRec_read_ui4(rec);
                    nbytes = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_PUT_1TS(lid, time, dlid, wid, rmaid, nbytes, userdata);
                    break;
                }

            case ELG_MPI_PUT_1TE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 slid;
                    elg_ui4 wid;
                    elg_ui4 rmaid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(slid)
                                   + sizeof(wid)
                                   + sizeof(rmaid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    slid  = ElgRec_read_ui4(rec);
                    wid   = ElgRec_read_ui4(rec);
                    rmaid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_PUT_1TE(lid, time, slid, wid, rmaid, userdata);
                    break;
                }

            case ELG_MPI_PUT_1TE_REMOTE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 wid;
                    elg_ui4 rmaid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(wid)
                                   + sizeof(rmaid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    dlid  = ElgRec_read_ui4(rec);
                    wid   = ElgRec_read_ui4(rec);
                    rmaid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_PUT_1TE_REMOTE(lid, time, dlid, wid, rmaid, userdata);
                    break;
                }

            case ELG_MPI_GET_1TO:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 rmaid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(rmaid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    rmaid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_GET_1TO(lid, time, rmaid, userdata);
                    break;
                }

            case ELG_MPI_GET_1TS:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 wid;
                    elg_ui4 rmaid;
                    elg_ui4 nbytes;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(wid)
                                   + sizeof(rmaid)
                                   + sizeof(nbytes)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time   = ElgRec_read_d8(rec);
                    dlid   = ElgRec_read_ui4(rec);
                    wid    = ElgRec_read_ui4(rec);
                    rmaid  = ElgRec_read_ui4(rec);
                    nbytes = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_GET_1TS(lid, time, dlid, wid, rmaid, nbytes, userdata);
                    break;
                }

            case ELG_MPI_GET_1TS_REMOTE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 wid;
                    elg_ui4 rmaid;
                    elg_ui4 nbytes;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(wid)
                                   + sizeof(rmaid)
                                   + sizeof(nbytes)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time   = ElgRec_read_d8(rec);
                    dlid   = ElgRec_read_ui4(rec);
                    wid    = ElgRec_read_ui4(rec);
                    rmaid  = ElgRec_read_ui4(rec);
                    nbytes = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_GET_1TS_REMOTE(lid, time, dlid, wid, rmaid, nbytes,
                                                  userdata);
                    break;
                }

            case ELG_MPI_GET_1TE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 slid;
                    elg_ui4 wid;
                    elg_ui4 rmaid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(slid)
                                   + sizeof(wid)
                                   + sizeof(rmaid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    slid  = ElgRec_read_ui4(rec);
                    wid   = ElgRec_read_ui4(rec);
                    rmaid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_GET_1TE(lid, time, slid, wid, rmaid, userdata);
                    break;
                }

            case ELG_MPI_WINEXIT:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;
                    elg_ui4 wid;
                    elg_ui4 cid;
                    elg_ui1 synex;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time) - sizeof(wid) - sizeof(cid)
                                        - sizeof(synex))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))
                                   + sizeof(wid)
                                   + sizeof(cid)
                                   + sizeof(synex)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }
                    wid   = ElgRec_read_ui4(rec);
                    cid   = ElgRec_read_ui4(rec);
                    synex = ElgRec_read_ui1(rec);

                    elg_readcb_MPI_WINEXIT(lid, time, handle->metc, handle->metv, wid, cid,
                                           synex, userdata);
                    break;
                }

            case ELG_MPI_WINCOLLEXIT:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;
                    elg_ui4 wid;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time) - sizeof(wid))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))
                                   + sizeof(wid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }
                    wid = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_WINCOLLEXIT(lid, time, handle->metc, handle->metv, wid,
                                               userdata);
                    break;
                }

            case ELG_MPI_WIN_LOCK:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 llid;
                    elg_ui4 wid;
                    elg_ui1 ltype;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(llid)
                                   + sizeof(wid)
                                   + sizeof(ltype)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    llid  = ElgRec_read_ui4(rec);
                    wid   = ElgRec_read_ui4(rec);
                    ltype = ElgRec_read_ui1(rec);

                    elg_readcb_MPI_WIN_LOCK(lid, time, llid, wid, ltype, userdata);
                    break;
                }

            case ELG_MPI_WIN_UNLOCK:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 llid;
                    elg_ui4 wid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(llid)
                                   + sizeof(wid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    llid = ElgRec_read_ui4(rec);
                    wid  = ElgRec_read_ui4(rec);

                    elg_readcb_MPI_WIN_UNLOCK(lid, time, llid, wid, userdata);
                    break;
                }

            /* -- Generic One-sided Operations -- */

            case ELG_PUT_1TS:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 rmaid;
                    elg_ui4 nbytes;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(rmaid)
                                   + sizeof(nbytes)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time   = ElgRec_read_d8(rec);
                    dlid   = ElgRec_read_ui4(rec);
                    rmaid  = ElgRec_read_ui4(rec);
                    nbytes = ElgRec_read_ui4(rec);

                    elg_readcb_PUT_1TS(lid, time, dlid, rmaid, nbytes, userdata);
                    break;
                }

            case ELG_PUT_1TE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 slid;
                    elg_ui4 rmaid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(slid)
                                   + sizeof(rmaid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    slid  = ElgRec_read_ui4(rec);
                    rmaid = ElgRec_read_ui4(rec);

                    elg_readcb_PUT_1TE(lid, time, slid, rmaid, userdata);
                    break;
                }

            case ELG_PUT_1TE_REMOTE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 rmaid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(rmaid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    dlid  = ElgRec_read_ui4(rec);
                    rmaid = ElgRec_read_ui4(rec);

                    elg_readcb_PUT_1TE_REMOTE(lid, time, dlid, rmaid, userdata);
                    break;
                }

            case ELG_GET_1TS:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 rmaid;
                    elg_ui4 nbytes;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(rmaid)
                                   + sizeof(nbytes)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time   = ElgRec_read_d8(rec);
                    dlid   = ElgRec_read_ui4(rec);
                    rmaid  = ElgRec_read_ui4(rec);
                    nbytes = ElgRec_read_ui4(rec);

                    elg_readcb_GET_1TS(lid, time, dlid, rmaid, nbytes, userdata);
                    break;
                }

            case ELG_GET_1TS_REMOTE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 dlid;
                    elg_ui4 rmaid;
                    elg_ui4 nbytes;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(dlid)
                                   + sizeof(rmaid)
                                   + sizeof(nbytes)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time   = ElgRec_read_d8(rec);
                    dlid   = ElgRec_read_ui4(rec);
                    rmaid  = ElgRec_read_ui4(rec);
                    nbytes = ElgRec_read_ui4(rec);

                    elg_readcb_GET_1TS_REMOTE(lid, time, dlid, rmaid, nbytes, userdata);
                    break;
                }

            case ELG_GET_1TE:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 slid;
                    elg_ui4 rmaid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(slid)
                                   + sizeof(rmaid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time  = ElgRec_read_d8(rec);
                    slid  = ElgRec_read_ui4(rec);
                    rmaid = ElgRec_read_ui4(rec);

                    elg_readcb_GET_1TE(lid, time, slid, rmaid, userdata);
                    break;
                }

            case ELG_COLLEXIT:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;
                    elg_ui4 rlid;
                    elg_ui4 cid;
                    elg_ui4 sent;
                    elg_ui4 recvd;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time) - sizeof(rlid) - sizeof(cid)
                                        - sizeof(sent) - sizeof(recvd))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))
                                   + sizeof(rlid)
                                   + sizeof(cid)
                                   + sizeof(sent)
                                   + sizeof(recvd)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }
                    rlid  = ElgRec_read_ui4(rec);
                    cid   = ElgRec_read_ui4(rec);
                    sent  = ElgRec_read_ui4(rec);
                    recvd = ElgRec_read_ui4(rec);

                    elg_readcb_COLLEXIT(lid, time, handle->metc, handle->metv, rlid, cid,
                                        sent, recvd, userdata);
                    break;
                }

            case ELG_ALOCK:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 lkid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(lkid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    lkid = ElgRec_read_ui4(rec);

                    elg_readcb_ALOCK(lid, time, lkid, userdata);
                    break;
                }

            case ELG_RLOCK:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 lkid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(lkid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    lkid = ElgRec_read_ui4(rec);

                    elg_readcb_RLOCK(lid, time, lkid, userdata);
                    break;
                }

            /* -- OpenMP -- */

            case ELG_OMP_FORK:
                {
                    elg_ui4 lid;
                    elg_d8  time;

                    if (length == (sizeof(lid)
                                   + sizeof(time)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);

                    elg_readcb_OMP_FORK(lid, time, userdata);
                    break;
                }

            case ELG_OMP_JOIN:
                {
                    elg_ui4 lid;
                    elg_d8  time;

                    if (length == (sizeof(lid)
                                   + sizeof(time)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);

                    elg_readcb_OMP_JOIN(lid, time, userdata);
                    break;
                }

            case ELG_OMP_ALOCK:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 lkid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(lkid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    lkid = ElgRec_read_ui4(rec);

                    elg_readcb_OMP_ALOCK(lid, time, lkid, userdata);
                    break;
                }

            case ELG_OMP_RLOCK:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    elg_ui4 lkid;

                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + sizeof(lkid)))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    lkid = ElgRec_read_ui4(rec);

                    elg_readcb_OMP_RLOCK(lid, time, lkid, userdata);
                    break;
                }

            case ELG_OMP_COLLEXIT:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }

                    elg_readcb_OMP_COLLEXIT(lid, time, handle->metc, handle->metv,
                                            userdata);
                    break;
                }

            /* -- EPILOG Internal -- */

            case ELG_LOG_OFF:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }

                    elg_readcb_LOG_OFF(lid, time, handle->metc, handle->metv, userdata);
                    break;
                }

            case ELG_LOG_ON:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }

                    elg_readcb_LOG_ON(lid, time, handle->metc, handle->metv, userdata);
                    break;
                }

            case ELG_ENTER_TRACING:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }

                    elg_readcb_ENTER_TRACING(lid, time, handle->metc, handle->metv,
                                             userdata);
                    break;
                }

            case ELG_EXIT_TRACING:
                {
                    elg_ui4 lid;
                    elg_d8  time;
                    int     i;

                    if (handle->metc == 255)
                    {
                        handle->metc = (length - sizeof(time))
                                       / sizeof(elg_ui8);
                        if (handle->metc > 0)
                        {
                            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));
                            if (handle->metv == NULL)
                            {
                                UTILS_FATAL("Can't allocate metric data buffer");
                            }
                        }
                    }
                    if (length == (sizeof(lid)
                                   + sizeof(time)
                                   + (handle->metc * sizeof(elg_ui8))))
                    {
                        lid = ElgRec_read_ui4(rec);
                    }
                    else
                    {
                        lid = handle->single_lid;
                    }
                    time = ElgRec_read_d8(rec);
                    for (i = 0; i < handle->metc; i++)
                    {
                        handle->metv[i] = ElgRec_read_ui8(rec);
                    }

                    elg_readcb_EXIT_TRACING(lid, time, handle->metc, handle->metv,
                                            userdata);
                    break;
                }

            default:
                {
                    #ifdef ELG_SILENT
                        UTILS_DEBUG("unknown record type: %i", type);
                    #else
                        UTILS_WARNING("unknown record type: %i", type);
                    #endif

                    return 0;
                }
        }
    }

    return 1;
}


int
elg_read_next_def(ElgRCB* handle,
                  void*   userdata)
{
    int     res = 0;
    ElgRec* rec;

    while (1)
    {
        rec = ElgIn_read_record(handle->in);
        if (rec == NULL)
        {
            return 0;
        }

        if (!ElgRec_is_event(rec))
        {
            break;
        }
    }

    res = elg_read_dispatch(handle, rec, userdata);
    ElgRec_free(rec);

    return res;
}


int
elg_read_next_event(ElgRCB* handle,
                    void*   userdata)
{
    int     res = 0;
    ElgRec* rec;

    while (1)
    {
        rec = ElgIn_read_record(handle->in);
        if (rec == NULL)
        {
            return 0;
        }

        if (  ElgRec_is_event(rec)
           || ElgRec_is_attribute(rec))
        {
            break;
        }
    }

    res = elg_read_dispatch(handle, rec, userdata);
    ElgRec_free(rec);

    return res;
}


int
elg_read_seek(ElgRCB* handle,
              long    offset)
{
    return ElgIn_seek(handle->in, offset);
}
