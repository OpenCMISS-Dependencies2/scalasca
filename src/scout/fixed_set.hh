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
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/// @file    fixed_set.hh
/// @brief   Definition of fixed_set
///
/// This file provides the definition of fixed_set


#ifndef SCOUT_FIXED_SET_HH
#define SCOUT_FIXED_SET_HH


#include <algorithm>
#include <cstring>


namespace scout
{
// forward declarations

template< class Key >
class fixed_set;

template< class Key >
bool
operator>=(const fixed_set< Key >& lhs,
           const fixed_set< Key >& rhs);

template< class Key >
fixed_set< Key >
operator-(const fixed_set< Key >& lhs,
          const fixed_set< Key >& rhs);


template< class Key >
class fixed_set
{
    public:
        // --- constructors & destructor -----------------------------------------

        fixed_set()
            : m_min(0),
              m_max(0),
              m_rep(one)
        {
            m_data.entry = m_max + 1;
        }

        fixed_set(const Key& min,
                  const Key& max)
            : m_min(min),
              m_max(max),
              m_rep(one)
        {
            m_data.entry = m_max + 1;
        }

        fixed_set(const fixed_set< Key >& s)
            : m_min(0),
              m_max(0),
              m_rep(one)
        {
            m_data.entry = m_max + 1;
            copy(s);
        }

        ~fixed_set()
        {
            clear();
        }

        // --- iterators ---------------------------------------------------------

        class iterator
        {
            Key                     m_pos;
            const fixed_set< Key >* m_set;


            public:
                iterator()
                    : m_pos(0),
                      m_set(0)
                {
                }

                iterator(Key                     pos,
                         const fixed_set< Key >* set)
                    : m_pos(pos),
                      m_set(set)
                {
                }

                // --- iterating

                iterator
                operator++()
                {
                    ++m_pos;

                    switch (m_set->m_rep)
                    {
                        case one:
                            if (m_pos != m_set->m_data.entry)
                            {
                                m_pos = m_set->m_max + 1;
                            }
                            break;

                        case one_inverse:
                            if (  (m_pos == m_set->m_data.entry)
                               && (m_pos <= m_set->m_max))
                            {
                                ++m_pos;
                            }
                            break;

                        case full:
                            {
                                bitvector bv(m_set);

                                while (  (m_pos <= m_set->m_max)
                                      && !bv.contains(m_pos))
                                {
                                    ++m_pos;
                                }
                            }
                            break;
                    }

                    return *this;
                }

                iterator
                operator++(int)
                {
                    return ++iterator(*this);
                }

                // --- dereference

                Key
                operator*() const
                {
                    if (m_set->contains(m_pos))
                    {
                        // bad call -> crash!
                        return m_pos;
                    }

                    return m_set->end().m_pos;
                }

                // --- comparison

                bool
                operator==(const iterator& i) const
                {
                    // invalid iterators are equal
                    if (  (m_pos > m_set->m_max)
                       && (i.m_pos > m_set->m_max))
                    {
                        return true;
                    }

                    return (m_pos == i.m_pos);
                }

                bool
                operator!=(const iterator& i) const
                {
                    // invalid iterators are equal
                    if (  (m_pos > m_set->m_max)
                       && (i.m_pos > m_set->m_max))
                    {
                        return false;
                    }

                    return (m_pos != i.m_pos);
                }

                friend class fixed_set< Key >;
        };


        iterator
        begin() const
        {
            iterator it(m_min, this);

            switch (m_rep)
            {
                case one:
                    it.m_pos = m_data.entry;
                    break;

                case one_inverse:
                    if (m_data.entry == m_min)
                    {
                        ++it.m_pos;
                    }
                    break;

                case full:
                    it.m_pos = bitvector(this).first_set();
            }

            return it;
        }

        iterator
        end() const
        {
            return iterator(m_max + 1, this);
        }

        // --- operators ---------------------------------------------------------

        fixed_set< Key >&
        operator=(const fixed_set< Key >& s)
        {
            copy(s);

            return *this;
        }

        fixed_set< Key >&
        operator-=(const fixed_set< Key >& s)
        {
            if (empty())
            {
                return *this;
            }

            switch (m_rep)
            {
                case one:
                    if (s.contains(m_data.entry))
                    {
                        m_data.entry = m_max + 1;
                    }
                    break;

                default:
                    switch (s.m_rep)
                    {
                        case one:
                            if (s.m_data.entry <= s.m_max)
                            {
                                erase(s.m_data.entry);
                            }
                            break;

                        default:
                            for (iterator it = s.begin(); it != s.end(); ++it)
                            {
                                erase(*it);
                            }
                    }
            }

            return *this;
        }

        // --- public interface --------------------------------------------------

        void
        fill()
        {
            clear();

            m_rep = one_inverse;
        }

        void
        insert(const Key& elem)
        {
            if (  (elem < m_min)
               || (elem > m_max))
            {
                return;
            }

            switch (m_rep)
            {
                case one:
                    if (m_data.entry > m_max)
                    {
                        m_data.entry = elem;
                    }
                    else if (m_data.entry != elem)
                    {
                        Key tmp = m_data.entry;

                        m_data.bitvec = 0;

                        bitvector bv(this);

                        bv.init();
                        bv.insert(tmp);
                        bv.insert(elem);

                        m_rep = full;
                    }
                    break;

                case one_inverse:
                    if (m_data.entry == elem)
                    {
                        m_data.entry = m_max + 1;
                    }
                    break;

                case full:
                    bitvector(this).insert(elem);
                    break;
            }
        }

        void
        erase(const Key& elem)
        {
            if (  (elem < m_min)
               || (elem > m_max))
            {
                return;
            }

            switch (m_rep)
            {
                case one:
                    if (m_data.entry == elem)
                    {
                        m_data.entry = m_max + 1;
                    }

                    break;

                case one_inverse:
                    if (m_data.entry > m_max)
                    {
                        m_data.entry = elem;
                    }
                    else if (m_data.entry != elem)
                    {
                        Key tmp = m_data.entry;

                        m_data.bitvec = 0;

                        bitvector bv(this);

                        bv.init();
                        bv.fill();
                        bv.erase(tmp);
                        bv.erase(elem);

                        m_rep = full;
                    }
                    break;

                case full:
                    {
                        bitvector bv(this);

                        bv.erase(elem);

                        if (bv.empty())
                        {
                            bv.detach();

                            m_data.entry = m_max + 1;
                            m_rep        = one;
                        }
                    }
                    break;
            }
        }

        void
        clear()
        {
            switch (m_rep)
            {
                case full:
                    bitvector(this).detach();
                    break;

                default:
                    break;
            }

            m_rep        = one;
            m_data.entry = m_max + 1;
        }

        void
        toggle(const Key& elem)
        {
            if (contains(elem))
            {
                erase(elem);
            }
            else
            {
                insert(elem);
            }
        }

        bool
        empty() const
        {
            if (m_max < m_min)
            {
                return true;
            }

            switch (m_rep)
            {
                case one:
                    return (m_data.entry > m_max);

                case one_inverse:
                    return (  (m_min == m_max)
                           && (m_data.entry == m_min));

                case full:
                    return bitvector(this).empty();

                default:
                    break;
            }

            return false;
        }

        Key
        range() const
        {
            return (m_max - m_min);
        }

        bool
        contains(const Key& elem) const
        {
            if (  (elem < m_min)
               || (elem > m_max))
            {
                return false;
            }

            switch (m_rep)
            {
                case one:
                    return (m_data.entry == elem);

                case one_inverse:
                    return (m_data.entry != elem);

                case full:
                    return bitvector(this).contains(elem);
            }

            return false;
        }

        fixed_set< Key >
        intersection_with(const fixed_set< Key >& fset)
        {
            fixed_set< Key > nset(std::max(m_min, fset.m_min), std::min(m_max, fset.m_max));

            const fixed_set< Key >* a = this;
            const fixed_set< Key >* b = &fset;

            if (a->m_rep > b->m_rep)
            {
                std::swap(a, b);
            }

            switch (a->m_rep)
            {
                case one:
                    if (b->contains(a->m_data.entry))
                    {
                        nset.insert(a->m_data.entry);
                    }

                    return nset;

                case one_inverse:
                    if (b->m_rep == one_inverse)
                    {
                        nset.fill();

                        if (a->m_data.entry <= a->m_max)
                        {
                            nset.erase(a->m_data.entry);
                        }
                        if (b->m_data.entry <= b->m_max)
                        {
                            nset.erase(b->m_data.entry);
                        }

                        return nset;
                    }

                default:
                    for (Key k = nset.m_min; k <= nset.m_max; ++k)
                    {
                        if (a->contains(k) && b->contains(k))
                        {
                            nset.insert(k);
                        }
                    }
            }

            return nset;
        }


    private:
        // --- private member functions -----------------------------------------

        void
        copy(const fixed_set< Key >& s)
        {
            if (this == &s)
            {
                return;
            }

            clear();

            m_min = s.m_min;
            m_max = s.m_max;
            m_rep = s.m_rep;

            switch (m_rep)
            {
                case one:
                case one_inverse:
                    m_data.entry = s.m_data.entry;
                    break;

                case full:
                    m_data.bitvec = 0;

                    bitvector(this).copy(s);
                    break;
            }
        }

        //
        // --- data types -------------------------------------------------------
        //

        // --- a ref-counted copy-on-write bitvector implementation

        class bitvector
        {
            // --- private data

            // we access the parent's data to be able to use a union

            fixed_set< Key >*       p;
            const fixed_set< Key >* c_p;

            // --- private functions

            void
            deep_copy()
            {
                unsigned nbytes = (p->m_max - p->m_min) / 8 + 1;
                rep_t*   tmp_p  = new rep_t;

                tmp_p->rcount = 1;
                tmp_p->data   = new unsigned char[nbytes];

                memcpy(tmp_p->data, p->m_data.bitvec->data, nbytes);

                detach();
                p->m_data.bitvec = tmp_p;
            }


            public:
                // --- data structures

                struct rep_t
                {
                    unsigned char* data;
                    unsigned       rcount;
                };


                // --- constructor

                bitvector(fixed_set< Key >* parent)
                    : p(parent),
                      c_p(parent)
                {
                }

                bitvector(const fixed_set< Key >* parent)
                    : p(0),
                      c_p(parent)
                {
                }

                // no automatic destruction: explicitely call detach()

                bitvector&
                copy(const fixed_set< Key >& s)
                {
                    if (p->m_data.bitvec == s.m_data.bitvec)
                    {
                        return *this;
                    }

                    detach();

                    p->m_data.bitvec = s.m_data.bitvec;

                    if (p->m_data.bitvec)
                    {
                        ++(p->m_data.bitvec->rcount);
                    }

                    return *this;
                }

                void
                init()
                {
                    detach();

                    p->m_data.bitvec = new rep_t;

                    unsigned nbytes = (p->m_max - p->m_min) / 8 + 1;

                    p->m_data.bitvec->rcount = 1;
                    p->m_data.bitvec->data   = new unsigned char[nbytes];

                    memset(p->m_data.bitvec->data, 0, nbytes);
                }

                void
                detach()
                {
                    if (  p->m_data.bitvec
                       && (--(p->m_data.bitvec->rcount) == 0))
                    {
                        delete[] p->m_data.bitvec->data;
                        delete p->m_data.bitvec;
                    }

                    p->m_data.bitvec = 0;
                }

                void
                insert(const Key& elem)
                {
                    deep_copy();

                    unsigned pos = (elem - p->m_min);

                    p->m_data.bitvec->data[pos / 8] |= ((unsigned char)1 << (pos % 8));
                }

                void
                erase(const Key& elem)
                {
                    if (p->m_data.bitvec->rcount > 1)
                    {
                        deep_copy();
                    }

                    unsigned      pos  = (elem - p->m_min);
                    unsigned char byte = (unsigned char)0xFF ^ ((unsigned char)1 << (pos % 8));

                    p->m_data.bitvec->data[pos / 8] &= byte;
                }

                void
                fill()
                {
                    unsigned nbytes = (p->m_max - p->m_min) / 8 + 1;

                    memset(p->m_data.bitvec->data, 0xFF, nbytes - 1);

                    unsigned rem = (p->m_max - p->m_min) % 8;

                    p->m_data.bitvec->data[nbytes - 1] = 1;

                    while (rem--)
                    {
                        p->m_data.bitvec->data[nbytes - 1] <<= 1;
                        p->m_data.bitvec->data[nbytes - 1]  |= (unsigned char)1;
                    }
                }

                bool
                contains(const Key& elem) const
                {
                    unsigned pos = (elem - c_p->m_min);

                    return (c_p->m_data.bitvec->data[pos / 8] & ((unsigned char)1 << (pos % 8)));
                }

                bool
                empty() const
                {
                    unsigned nbytes = (c_p->m_max - c_p->m_min) / 8 + 1;

                    for (unsigned n = 0; n < nbytes; ++n)
                    {
                        if (c_p->m_data.bitvec->data[n])
                        {
                            return false;
                        }
                    }

                    return true;
                }

                Key
                first_set() const
                {
                    unsigned nbytes = (c_p->m_max - c_p->m_min) / 8 + 1;
                    unsigned byte;

                    for (byte = 0; (byte < nbytes) && !c_p->m_data.bitvec->data[byte]; ++byte)
                    {
                    }

                    if (byte < nbytes)
                    {
                        unsigned char last = c_p->m_data.bitvec->data[byte];
                        unsigned      bit;

                        for (bit = 0; (bit < 8) && !(last & ((unsigned char)1 << bit)); ++bit)
                        {
                        }

                        if (bit < 8)
                        {
                            return (c_p->m_min + (byte * 8) + bit);
                        }
                    }

                    return (c_p->m_max + 1);
                }
        };


        //
        // --- data members -----------------------------------------------------
        //

        Key m_min;
        Key m_max;


        // --- set representations ----------------------------------------------

        enum
        {
            one,
            one_inverse,
            full
        }   m_rep;


        union
        {
            Key                        entry;  // max. one element representation
            typename bitvector::rep_t* bitvec; // full representation
        }   m_data;


        // --- friends -----------------------------------------------------------

        friend bool
        operator>=< Key >(const fixed_set< Key >& a,
                          const fixed_set< Key >& b);
};


template< class Key >
bool
operator>=(const fixed_set< Key >& a,
           const fixed_set< Key >& b)
{
    if (b.empty())
    {
        return true;
    }
    if (a.empty())
    {
        return false;
    }

    if (  (a.m_min > b.m_max)
       || (a.m_max < b.m_min))
    {
        return false;
    }

    switch (a.m_rep)
    {
        case fixed_set< Key >::one:
            switch (b.m_rep)
            {
                case fixed_set< Key >::one:
                    return (a.m_data.entry == b.m_data.entry);

                case fixed_set< Key >::one_inverse:
                    return (  (b.m_min == b.m_max)
                           && (b.m_data.entry != a.m_data.entry));

                case fixed_set< Key >::full:
                    for (typename fixed_set< Key >::iterator it = b.begin();
                         it != b.end();
                         ++it)
                    {
                        if (*it != a.m_data.entry)
                        {
                            return false;
                        }
                    }

                    return true;
            }

        case fixed_set< Key >::one_inverse:
            if (a.m_data.entry > a.m_max)
            {
                // set a is full

                switch (b.m_rep)
                {
                    case fixed_set< Key >::one:
                        return (  (b.m_data.entry >= a.m_min)
                               && (b.m_data.entry <= a.m_max));

                    case fixed_set< Key >::one_inverse:
                        if (  (b.m_max > a.m_max)
                           || (b.m_min < a.m_min))
                        {
                            return false;
                        }

                        break;

                    case fixed_set< Key >::full:
                        {
                            typename fixed_set< Key >::bitvector bv(&b);

                            for (Key k = b.m_min; k < a.m_min; ++k)
                            {
                                if (bv.contains(k))
                                {
                                    return false;
                                }
                            }
                            for (Key k = a.m_max + 1; k <= b.m_max; ++k)
                            {
                                if (bv.contains(k))
                                {
                                    return false;
                                }
                            }
                        }
                }

                return true;
            }
            else
            {
                // set a misses one element

                switch (b.m_rep)
                {
                    case fixed_set< Key >::one:
                        return (  (b.m_data.entry >= a.m_min)
                               && (b.m_data.entry <= a.m_max)
                               && (a.m_data.entry != b.m_data.entry));

                    case fixed_set< Key >::one_inverse:
                        if (  (b.m_max > a.m_max)
                           || (b.m_min < a.m_min))
                        {
                            return false;
                        }
                        if (  (b.m_data.entry <= b.m_max)
                           && (b.m_data.entry != a.m_data.entry))
                        {
                            return false;
                        }
                        break;

                    case fixed_set< Key >::full:
                        {
                            typename fixed_set< Key >::bitvector bv(&b);

                            for (Key k = b.m_min; k < a.m_min; ++k)
                            {
                                if (bv.contains(k))
                                {
                                    return false;
                                }
                            }
                            for (Key k = a.m_max + 1; k <= b.m_max; ++k)
                            {
                                if (bv.contains(k))
                                {
                                    return false;
                                }
                            }

                            if (bv.contains(a.m_data.entry))
                            {
                                return false;
                            }
                        }
                        break;
                }

                return true;
            }

        case fixed_set< Key >::full:
            {
                typename fixed_set< Key >::bitvector bv(&a);

                switch (b.m_rep)
                {
                    case fixed_set< Key >::one:
                        return bv.contains(b.m_data.entry);

                    default:
                        for (typename fixed_set< Key >::iterator it = b.begin();
                             it != b.end();
                             ++it)
                        {
                            if (!bv.contains(*it))
                            {
                                return false;
                            }
                        }

                        return true;
                }
            }
    }

    return false;
}


template< class Key >
fixed_set< Key >
operator-(const fixed_set< Key >& a,
          const fixed_set< Key >& b)
{
    fixed_set< Key > nset(a);

    return (nset -= b);
}
}    // namespace scout


#endif    // !SCOUT_FIXED_SET_HH
