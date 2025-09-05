/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// A.ipp    simple class test

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#if BOOST_WORKAROUND(BOOST_DINKUMWARE_STDLIB, == 1)
#include <cci/archive/dinkumware.hpp>
#endif

#include <cci/serialization/nvp.hpp>
#include "A.hpp"

template<class Archive>
void A::serialize(
    Archive &ar,
    const unsigned int /* file_version */
){
    ar & CCI_SERIALIZATION_NVP(b);
    #ifndef BOOST_NO_INT64_T
    ar & CCI_SERIALIZATION_NVP(f);
    ar & CCI_SERIALIZATION_NVP(g);
    #endif
    #if BOOST_WORKAROUND(__BORLANDC__,  <= 0x551 )
        int i;
        if(BOOST_DEDUCED_TYPENAME Archive::is_saving::value){
            i = l;
            ar & CCI_SERIALIZATION_NVP(i);
        }
        else{
            ar & CCI_SERIALIZATION_NVP(i);
            l = i;
        }
    #else
        ar & CCI_SERIALIZATION_NVP(l);
    #endif
    ar & CCI_SERIALIZATION_NVP(m);
    ar & CCI_SERIALIZATION_NVP(n);
    ar & CCI_SERIALIZATION_NVP(o);
    ar & CCI_SERIALIZATION_NVP(p);
    ar & CCI_SERIALIZATION_NVP(q);
    #ifndef BOOST_NO_CWCHAR
    ar & CCI_SERIALIZATION_NVP(r);
    #endif
    ar & CCI_SERIALIZATION_NVP(c);
    ar & CCI_SERIALIZATION_NVP(s);
    ar & CCI_SERIALIZATION_NVP(t);
    ar & CCI_SERIALIZATION_NVP(u);
    ar & CCI_SERIALIZATION_NVP(v);
    ar & CCI_SERIALIZATION_NVP(w);
    ar & CCI_SERIALIZATION_NVP(x);
    ar & CCI_SERIALIZATION_NVP(y);
    #ifndef BOOST_NO_STD_WSTRING
    ar & CCI_SERIALIZATION_NVP(z);
    #endif
}
