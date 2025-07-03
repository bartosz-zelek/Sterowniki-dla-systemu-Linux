/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_enable_shared_from_this.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This demonstrates a problem with cci::serialization and cci::enable_shared_from_this.
// (boost version 1.53)
// See boost TRAC ticket #9567
//
// Given the following class structure:
//    Base is a simple class
//    Derived inherits from Base
//    Derived also inherits from cci::enable_shared_from_this<Derived>
//    Base and Derived implement cci::serialization
//
// When deserializing an instance of Derived into a vector of cci::shared_ptr<Derived>:
//    Base and Derived members are reconstructed correctly.
//	  Derived::shared_from_this() works as expected.
//
// But when deserializing an instance of Derived into a vector of cci::shared_ptr<Base>:
//    Base and Derived members are still reconstructed correctly.
//	  Derived::shared_from_this() throws a bad_weak_ptr exception.
//    This is because enable_shared_from_this::weak_ptr is NOT reconstructed - It is zero.

#include <sstream>

#include <cci/archive/text_oarchive.hpp>
#include <cci/archive/text_iarchive.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <cci/serialization/shared_ptr.hpp>
#include <cci/serialization/export.hpp>
#include <cci/serialization/split_free.hpp>

#include "test_tools.hpp"

#include <set>

namespace cci {
namespace serialization {

struct enable_shared_from_this_helper {
    std::set<shared_ptr<void> > m_esfth;
    void record(cci::shared_ptr<void>  sp){
        m_esfth.insert(sp);
    }
};

template<class Archive, class T>
void serialize(
    Archive & ar,
    cci::enable_shared_from_this<T> & t,
    const unsigned int file_version
){
    enable_shared_from_this_helper & h =
        ar.template get_helper<
            enable_shared_from_this_helper
        >();

    shared_ptr<T> sp = t.shared_from_this();
    h.record(sp);
}

} // serialization
} // boost

class Base {
    friend class cci::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & CCI_SERIALIZATION_NVP(m_base);
	}
protected:
	Base() {}
	virtual ~Base() {}		// "virtual" forces RTTI, to enable serialization of Derived from Base pointer
public:
	int m_base;
};

class Derived : public Base, public cci::enable_shared_from_this<Derived> {
    friend class cci::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(Base);
		ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(
            cci::enable_shared_from_this<Derived>
        );
		ar & CCI_SERIALIZATION_NVP(m_derived);
	}
public:
	cci::shared_ptr<Derived> SharedPtr() { return shared_from_this(); }
	int m_derived;
	Derived() {}
	~Derived() {}
};
// The following is required to enable serialization from Base pointer
CCI_CLASS_EXPORT(Derived)

// This test passes
void test_passes(){
	std::stringstream ss;
	{
		cci::shared_ptr<Derived> d(new Derived());
		d->m_base = 1;
		d->m_derived = 2;

		// Get a raw pointer to Derived
		Derived* raw_d = d.get();

		// Verify base and derived members
		BOOST_CHECK(raw_d->m_base==1);
		BOOST_CHECK(raw_d->m_derived==2);

		// verify shared_from_this
		BOOST_CHECK(d == raw_d->SharedPtr());

		cci::archive::text_oarchive oa(ss);
		oa & CCI_SERIALIZATION_NVP(d);
	}
	{
		// Deserialize it back into a vector of shared_ptr<Derived>
		cci::shared_ptr<Derived> d;
		ss.seekg(0);
		cci::archive::text_iarchive ia(ss);
		ia & CCI_SERIALIZATION_NVP(d);

		// Get a raw pointer to Derived
		Derived* raw_d = d.get();

		// Verify base and derived members
		BOOST_CHECK(raw_d->m_base==1);
		BOOST_CHECK(raw_d->m_derived==2);

		// verify shared_from_this
		BOOST_CHECK(d == raw_d->SharedPtr());
	}	
}

// This test fails
void test_fails(){
	std::stringstream ss;
	{
		cci::shared_ptr<Base> b(new Derived());
		Derived* raw_d1 = static_cast<Derived*>(b.get());
		raw_d1->m_base = 1;
		raw_d1->m_derived = 2;

		// Get a raw pointer to Derived via shared_ptr<Base>
		Derived* raw_d = static_cast<Derived*>(b.get());

		// Verify base and derived members
		BOOST_CHECK(raw_d->m_base==1);
		BOOST_CHECK(raw_d->m_derived==2);

		// verify shared_from_this
		cci::shared_ptr<Derived> d = raw_d->SharedPtr();
		BOOST_CHECK(d == b);
		// Serialize the vector
		cci::archive::text_oarchive oa(ss);
		oa & CCI_SERIALIZATION_NVP(b);
	}
	{
		// Deserialize it back into a vector of shared_ptr<Base>
		ss.seekg(0);
		cci::archive::text_iarchive ia(ss);
		cci::shared_ptr<Base> b;
		ia & CCI_SERIALIZATION_NVP(b);

		// Get a raw pointer to Derived via shared_ptr<Base>
		Derived* raw_d = static_cast<Derived*>(b.get());
		// Verify base and derived members
		BOOST_CHECK(raw_d->m_base==1);
		BOOST_CHECK(raw_d->m_derived==2);

		// verify shared_from_this
		// FAIL: The following line throws bad_weak_ptr exception 
		cci::shared_ptr<Derived> d = raw_d->SharedPtr();
		BOOST_CHECK(d == b);
	}	
}

int test_main(int /*argc*/, char * /*argv */[]){
    test_fails();
    test_passes();
    return EXIT_SUCCESS;
}

// EOF
