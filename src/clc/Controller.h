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


#ifndef CLC_CONTROLLER_H
#define CLC_CONTROLLER_H


class Controller
{
    public:
        // Constructors & destructor
        Controller()
            : gamma(0.99999)
        {
        }

        ~Controller()
        {
        }

        double
        get_gamma()
        {
            return gamma;
        }

        void
        set_gamma(int pass);


    private:
        // Control variable.
        double gamma;
};


#endif    // !CLC_CONTROLLER_H
