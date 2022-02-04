/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include <pearl/Paradigm.h>

#include <gtest/gtest.h>

using namespace testing;
using namespace pearl;


// --- Paradigm tests -------------------------------------------------------

TEST(ParadigmT,
     testToString)
{
    // Ensure that we test all known paradigms; when a new paradigm is
    // added, adjust the second parameter accordingly
    ASSERT_EQ(Paradigm::NUMBER_OF_PARADIGMS - 1, Paradigm::NONE);

    // Use ASSERT_EQ here to stop if we've missed a comma in the string
    // representation list
    ASSERT_EQ("unknown", toString(Paradigm::UNKNOWN));
    ASSERT_EQ("user", toString(Paradigm::USER));
    ASSERT_EQ("compiler", toString(Paradigm::COMPILER));
    ASSERT_EQ("openmp", toString(Paradigm::OPENMP));
    ASSERT_EQ("mpi", toString(Paradigm::MPI));
    ASSERT_EQ("cuda", toString(Paradigm::CUDA));
    ASSERT_EQ("measurement", toString(Paradigm::MEASUREMENT_SYSTEM));
    ASSERT_EQ("pthread", toString(Paradigm::PTHREAD));
    ASSERT_EQ("hmpp", toString(Paradigm::HMPP));
    ASSERT_EQ("ompss", toString(Paradigm::OMPSS));
    ASSERT_EQ("hardware", toString(Paradigm::HARDWARE));
    ASSERT_EQ("gaspi", toString(Paradigm::GASPI));
    ASSERT_EQ("upc", toString(Paradigm::UPC));
    ASSERT_EQ("shmem", toString(Paradigm::SHMEM));
    ASSERT_EQ("winthread", toString(Paradigm::WINTHREAD));
    ASSERT_EQ("qtthread", toString(Paradigm::QTTHREAD));
    ASSERT_EQ("acethread", toString(Paradigm::ACETHREAD));
    ASSERT_EQ("tbbthread", toString(Paradigm::TBBTHREAD));
    ASSERT_EQ("openacc", toString(Paradigm::OPENACC));
    ASSERT_EQ("opencl", toString(Paradigm::OPENCL));
    ASSERT_EQ("mtapi", toString(Paradigm::MTAPI));
    ASSERT_EQ("sampling", toString(Paradigm::SAMPLING));
    ASSERT_EQ("none", toString(Paradigm::NONE));
}
