/****************************************************************************
**  Copyright (c) 2013                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <mpi.h>
#include <gtest/gtest.h>

#include "ext-mpi-tap-printer.h"

using namespace testing;


int
main(int    argc,
     char** argv)
{
    // Initialization
    MPI_Init(&argc, &argv);
    InitGoogleTest(&argc, argv);

    // Install TAP printer (MPI version)
    TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new MpiTapPrinter);

    // Run all tests...
    const int result = RUN_ALL_TESTS();

    // Finalization
    MPI_Finalize();
    return result;
}
