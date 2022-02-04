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
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SILAS_MODELFACTORY_H
#define SILAS_MODELFACTORY_H


#include <string>


namespace silas
{
class Model;


class ModelFactory
{
    public:
        static silas::Model*
        create_model(const std::string& model);
};
}    // namespace silas


#endif    // !SILAS_MODELFACTORY_H
