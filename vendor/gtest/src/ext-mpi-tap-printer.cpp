/****************************************************************************
**  Copyright (c) 2013                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "ext-mpi-tap-printer.h"
#include "ext-tap-helper.h"

#include <cstring>

using namespace std;
using namespace testing;
using namespace testing::internal;


//--- Event listener callbacks ----------------------------------------------

void
MpiTapPrinter::OnTestProgramStart(const UnitTest& unit_test)
{
    // MPI-specific setup
    MPI_Comm_dup(MPI_COMM_WORLD, &mCommunicator);
    MPI_Comm_size(mCommunicator, &mWorldSize);
    MPI_Comm_rank(mCommunicator, &mWorldRank);

    // SANITY CHECK: Shuffling of tests not supported for MPI!
    const bool shuffled = GTEST_FLAG(shuffle);
    if (shuffled) {
        if (mWorldRank == 0) {
            printf("Shuffling not supported for MPI tests!\n\n");
        }
        MPI_Barrier(mCommunicator);
        MPI_Abort(mCommunicator, 1);
    }

    // Print test plan
    if (mWorldRank == 0) {
        PrintTapTestPlan(unit_test);
    }
}


void
MpiTapPrinter::OnTestEnd(const TestInfo& test_info)
{
    // Determine overall result
    int local  = test_info.result()->Failed() ? 1 : 0;
    int global = 0;
    MPI_Allreduce(&local, &global, 1, MPI_INT, MPI_SUM, mCommunicator);

    // Print overall result
    const bool failed = (global > 0);
    if (mWorldRank == 0) {
        PrintTapTestLine(test_info, failed);
    }

    // If test failed, print details by rank
    if (failed) {
        if (mWorldRank == 0) {
            // Print own diagnostics
            PrintTapSeparator("MPI rank 0");
            PrintTapDiagnostics(GetTestDetails(test_info));

            // Print diagnostics from other ranks
            for (int rank = 1; rank < mWorldSize; ++rank) {
                PrintTapSeparator("MPI rank " + IntToString(rank));

                // Receive length of diagnostics (including '\0' char)
                MPI_Status status;
                int        length;
                MPI_Recv(&length, 1, MPI_INT, rank, 42, mCommunicator, &status);

                // Receive & print diagnostics
                char* diags = new char[length];
                MPI_Recv(diags, length, MPI_CHAR, rank, 42, mCommunicator,
                         &status);
                PrintTapDiagnostics(diags);
                delete[] diags;
            }
            PrintTapSeparator();
        } else {
            std::string diags = GetTestDetails(test_info);

            // Send length of diagnostics
            int length = diags.length() + 1;
            MPI_Ssend(&length, 1, MPI_INT, 0, 42, mCommunicator);

            // Send diagnostics
            char* buffer = new char[length];
            strcpy(buffer, diags.c_str());
            MPI_Ssend(buffer, length, MPI_CHAR, 0, 42, mCommunicator);
            delete[] buffer;
        }
    }
}


void
MpiTapPrinter::OnTestIterationEnd(const UnitTest& unit_test,
                                  int             iteration)
{
    if (mWorldRank == 0) {
        PrintTapDisabledTests(unit_test);
    }
}


void
MpiTapPrinter::OnTestProgramEnd(const UnitTest& unit_test)
{
    // Release MPI resources
    MPI_Comm_free(&mCommunicator);
}
