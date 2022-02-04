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


#ifndef SILAS_MODEL_H
#define SILAS_MODEL_H


#include <map>
#include <set>
#include <string>

#include <pearl/CallbackManager.h>


namespace silas
{
/**
 * Base class for simulation models. Simulation models define how
 * the event stream is modified by registering its callbacks to the
 * simulation infrastructure.
 */
class Model
{
    public:
        /**
         * Runtime configuration structure
         */
        struct Config
        {
            bool        forward;
            const char* description;
            int         mincbevent;  // minimal event id to execute callbacks for
        };


        /**
         * Destructor.
         */
        virtual
        ~Model();

        /**
         * Return the model name as a std::string to be used in
         * log functions etc.
         */
        virtual const std::string
        get_name() const = 0;

        /**
         * Trigger callback registration for a specific run
         */
        virtual void
        register_callbacks(const int               run,
                           pearl::CallbackManager* cbmanager) = 0;

        /**
         * Retrieve configuration of simulation runs
         */
        virtual std::vector< Config >
        get_run_configuration() const = 0;

        /// @name Model Configuration
        /// @{

        /**
         * Set a configuration flag
         */
        virtual void
        set_flag(const std::string& flag);

        /**
         * Unset a configuration flag
         */
        virtual void
        unset_flag(const std::string& flag);

        /**
         * Test a configuration flag
         */
        virtual bool
        is_set(const std::string& flag) const;

        /**
         * Set a configuration option
         */
        virtual void
        set_option(const std::string& option,
                   const std::string& value);

        /**
         * Unset a configuration option
         */
        virtual void
        unset_option(const std::string& option);

        /**
         * Get a configuration option
         */
        virtual const std::string
        get_option(const std::string& option,
                   const std::string& defval = std::string("")) const;

        /// @}


    protected:
        /// @name Flags and Options
        /// @{

        std::set< std::string >              m_flags;
        std::map< std::string, std::string > m_options;

        /// @}
};
}    // namespace silas


#endif    // !SILAS_MODEL_H
