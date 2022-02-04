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


/// @file    fixed_map.hh
/// @brief   Definition of fixed_priority_map
///
/// This file provides the definition of fixed_priority_map


#ifndef SCOUT_FIXED_MAP_HH
#define SCOUT_FIXED_MAP_HH


#include <algorithm>
#include <functional>
#include <utility>


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   fixed_priority_map
 *  @ingroup Utilities
 *  @brief   A fixed-size map.
 **/
/*-------------------------------------------------------------------------*/

template< class Key,
          class T,
          int maxsize,
          class KeyCmp = std::less< Key >,
          class TCmp   = std::less< T > >
class fixed_priority_map
{
    public:
        //
        // typedefs
        //

        typedef Key                 key_type;
        typedef T                   mapped_type;
        typedef std::pair< Key, T > value_type;
        typedef KeyCmp              key_compare;
        typedef TCmp                mapped_compare;
        typedef const value_type*   const_iterator;


        //
        // Constructors & destructor
        //

        fixed_priority_map()
            : need_purge(false),
              count(0)
        {
        }

        ~fixed_priority_map()
        {
        }

        //
        // public methods
        //

        int
        size() const
        {
            return count;
        }

        bool
        is_empty() const
        {
            return (count == 0);
        }

        const_iterator
        begin() const
        {
            purge();

            return array;
        }

        const_iterator
        end() const
        {
            purge();

            return (array + count);
        }

        const_iterator
        find(const key_type& key) const
        {
            purge();

            return (array + find_index(key));
        }

        void
        clear()
        {
            count = 0;
        }

        mapped_type&
        operator[](const key_type& key)
        {
            purge();

            int p = lower_bound_index(key);

            if (  (p < count)
               && (key == array[p].first))
            {
                return array[p].second;
            }

            // Not found -> create new element.
            // May be also a temporary element at position array[maxsize].

            if (count == maxsize)
            {
                p          = maxsize;
                need_purge = true;
            }
            else
            {
                ++count;
            }

            for (int i = count - 1; i > p; --i)
            {
                array[i] = array[i - 1];
            }

            array[p] = std::make_pair(key, T());

            return array[p].second;
        }


    private:
        //
        // member variables
        //

        mutable value_type array[maxsize + 1];    // the map
        mutable bool       need_purge;
        int                count;    // current number of elements


        //
        // private methods
        //

        void
        purge() const
        {
            if (need_purge)
            {
                // find smallest existing element
                int min = minimum_index();

                // replace minimum with new object
                if (TCmp()(array[min].second, array[maxsize].second))
                {
                    int lb = lower_bound_index(array[maxsize].first);

                    if (lb < min)
                    {
                        for (int i = min; i > lb; --i)
                        {
                            array[i] = array[i - 1];
                        }

                        array[lb] = array[maxsize];
                    }
                    else if (lb > min)
                    {
                        for (int i = min; i < (lb - 1); ++i)
                        {
                            array[i] = array[i + 1];
                        }

                        array[lb - 1] = array[maxsize];
                    }
                    else
                    {
                        array[lb] = array[maxsize];
                    }
                }

                need_purge = false;
            }
        }

        int
        minimum_index() const
        {
            if (count == 0)
            {
                return 0;
            }

            int min = 0;

            for (int i = 1; i < count; ++i)
            {
                if (TCmp()(array[i].second, array[min].second))
                {
                    min = i;
                }
            }

            return min;
        }

        int
        lower_bound_index(const key_type& key) const
        {
            // binary search

            int l = 0, r = count;

            while (l < r)
            {
                int p = l + (r - l) / 2;

                if (KeyCmp()(array[p].first, key))
                {
                    l = p + 1;
                }
                else
                {
                    r = p;
                }
            }

            return l;
        }

        int
        find_index(const key_type& key) const
        {
            int i = lower_bound_index(key);

            return ((  (i < count)
                    && (array[i].first == key)) ? i : count);
        }
};
}    // namespace scout


#endif    // !SCOUT_FIXED_MAP_HH
