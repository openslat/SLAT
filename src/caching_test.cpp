/**
 * @file   caching_test.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Fri Jan 22 14:57:54 NZDT 2016
 * 
 * @brief Unit tests for the caching mechanism.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2016 Canterbury University
 */
#include <list>
#include "caching.h"
#include <iostream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace SLAT;

/*
 * Check that function values are actually cached.
 */
BOOST_AUTO_TEST_CASE(Cached_Function_Caching)
{
    int counter=0;
    Caching::CachedFunction<int, double> f([&counter] (double) { return ++counter; }, "Test Function");
    BOOST_CHECK_EQUAL(f(2.0), 1);
    BOOST_CHECK_EQUAL(f(1.0), 2);
    BOOST_CHECK_EQUAL(f(1.0), 2);
    BOOST_CHECK_EQUAL(f(2.0), 1);
}

/*
 * Clearing a cache will cause values to be re-calculated when requested.
 */
BOOST_AUTO_TEST_CASE(Cached_Function_ClearCache)
{
    int counter=0;
    Caching::CachedFunction<int, double> f([&counter] (double) { return ++counter; }, "Test Function");
    BOOST_CHECK_EQUAL(f(3.0), 1);
    BOOST_CHECK_EQUAL(f(2.0), 2);
    BOOST_CHECK_EQUAL(f(1.0), 3);
    BOOST_CHECK_EQUAL(f(3.0), 1);
    BOOST_CHECK_EQUAL(f(2.0), 2);
    BOOST_CHECK_EQUAL(f(1.0), 3);
    f.ClearCache();
    BOOST_CHECK_EQUAL(f(1.0), 4);
    BOOST_CHECK_EQUAL(f(2.0), 5);
    BOOST_CHECK_EQUAL(f(3.0), 6);
}

/*
 * Make sure two caches don't interfere with each other:
 */
BOOST_AUTO_TEST_CASE(Cached_Function_Two_Functions)
{
    int counter1=0, counter2=0;
    Caching::CachedFunction<int, double> f1([&counter1] (double) { return ++counter1; }, "Test Function #1");
    Caching::CachedFunction<int, double> f2([&counter2] (double) { return ++counter2; }, "Test Function #2");
    BOOST_CHECK_EQUAL(f1(3.0), 1);
    BOOST_CHECK_EQUAL(f1(2.0), 2);
    BOOST_CHECK_EQUAL(f1(1.0), 3);

    BOOST_CHECK_EQUAL(f2(3.0), 1);
    BOOST_CHECK_EQUAL(f2(2.0), 2);
    BOOST_CHECK_EQUAL(f2(1.0), 3);
    
    BOOST_CHECK_EQUAL(f1(3.0), 1);
    BOOST_CHECK_EQUAL(f1(2.0), 2);
    BOOST_CHECK_EQUAL(f1(1.0), 3);

    BOOST_CHECK_EQUAL(f2(3.0), 1);
    BOOST_CHECK_EQUAL(f2(2.0), 2);
    BOOST_CHECK_EQUAL(f2(1.0), 3);

    f1.ClearCache();
    
    BOOST_CHECK_EQUAL(f2(3.0), 1);
    BOOST_CHECK_EQUAL(f2(2.0), 2);
    BOOST_CHECK_EQUAL(f2(1.0), 3);
    
    BOOST_CHECK_EQUAL(f1(1.0), 4);
    BOOST_CHECK_EQUAL(f1(2.0), 5);
    BOOST_CHECK_EQUAL(f1(3.0), 6);

    f2.ClearCache();
    
    BOOST_CHECK_EQUAL(f2(3.0), 4);
    BOOST_CHECK_EQUAL(f2(2.0), 5);
    BOOST_CHECK_EQUAL(f2(1.0), 6);
    
    BOOST_CHECK_EQUAL(f1(1.0), 4);
    BOOST_CHECK_EQUAL(f1(2.0), 5);
    BOOST_CHECK_EQUAL(f1(3.0), 6);

    Caching::Clear_Caches();
    
    BOOST_CHECK_EQUAL(f2(3.0), 7);
    BOOST_CHECK_EQUAL(f2(2.0), 8);
    BOOST_CHECK_EQUAL(f2(1.0), 9);
    
    BOOST_CHECK_EQUAL(f1(1.0), 7);
    BOOST_CHECK_EQUAL(f1(2.0), 8);
    BOOST_CHECK_EQUAL(f1(3.0), 9);
}

/*
 * Make sure that when one cache is destroyed, others are not affected and Caching::Clear_Caches() still works.
 */
BOOST_AUTO_TEST_CASE(Cached_Function_Delete_Function)
{
    {
        int counter1=0;
        Caching::CachedFunction<int, double> f1([&counter1] (double) { return ++counter1; }, "Test Function #1");

        {
            int counter2=0;
            Caching::CachedFunction<int, double> f2([&counter2] (double) { return ++counter2; }, "Test Function #2");

            BOOST_CHECK_EQUAL(f1(3.0), 1);
            BOOST_CHECK_EQUAL(f1(2.0), 2);
            BOOST_CHECK_EQUAL(f1(1.0), 3);

            BOOST_CHECK_EQUAL(f2(3.0), 1);
            BOOST_CHECK_EQUAL(f2(2.0), 2);
            BOOST_CHECK_EQUAL(f2(1.0), 3);
        }
        Caching::Clear_Caches();
    
        BOOST_CHECK_EQUAL(f1(3.0), 4);
        BOOST_CHECK_EQUAL(f1(2.0), 5);
        BOOST_CHECK_EQUAL(f1(1.0), 6);
    }
}


/*
 * Check that values are actually cached (computed only once).
 */
BOOST_AUTO_TEST_CASE(Cached_Value_Caching)
{
    int counter=0;
    Caching::CachedValue<int> v([&counter] () { return ++counter; }, "Test Function");
    BOOST_CHECK_EQUAL(v(), 1);
    BOOST_CHECK_EQUAL(v(), 1);
    BOOST_CHECK_EQUAL(v(), 1);
    BOOST_CHECK_EQUAL(v(), 1);
}

/*
 * Clearing a cache will cause the value to be re-calculated when requested.
 */
BOOST_AUTO_TEST_CASE(Cached_Value_ClearCache)
{
    int counter=0;
    Caching::CachedValue<int> v([&counter] () { return ++counter; }, "Test Function");
    BOOST_CHECK_EQUAL(v(), 1);
    BOOST_CHECK_EQUAL(v(), 1);
    BOOST_CHECK_EQUAL(v(), 1);
    v.ClearCache();
    BOOST_CHECK_EQUAL(v(), 2);
    BOOST_CHECK_EQUAL(v(), 2);
}

/*
 * Make sure two caches don't interfere with each other:
 */
BOOST_AUTO_TEST_CASE(Cached_Value_Two_Values)
{
    int counter1=0, counter2=0;
    Caching::CachedValue<int> v1([&counter1] () { return ++counter1; }, "Test Function #1");
    Caching::CachedValue<int> v2([&counter2] () { return ++counter2; }, "Test Function #2");
    BOOST_CHECK_EQUAL(v1(), 1);
    BOOST_CHECK_EQUAL(v1(), 1);

    BOOST_CHECK_EQUAL(v2(), 1);
    BOOST_CHECK_EQUAL(v2(), 1);

    v1.ClearCache();
    
    BOOST_CHECK_EQUAL(v1(), 2);
    BOOST_CHECK_EQUAL(v1(), 2);

    BOOST_CHECK_EQUAL(v2(), 1);
    BOOST_CHECK_EQUAL(v2(), 1);

    v2.ClearCache();
    
    BOOST_CHECK_EQUAL(v1(), 2);
    BOOST_CHECK_EQUAL(v1(), 2);

    BOOST_CHECK_EQUAL(v2(), 2);
    BOOST_CHECK_EQUAL(v2(), 2);

    Caching::Clear_Caches();
    
    BOOST_CHECK_EQUAL(v1(), 3);
    BOOST_CHECK_EQUAL(v1(), 3);

    BOOST_CHECK_EQUAL(v2(), 3);
    BOOST_CHECK_EQUAL(v2(), 3);
}

/*
 * Make sure that when one cache is destroyed, others are not affected and Caching::Clear_Caches() still works.
 */
BOOST_AUTO_TEST_CASE(Cached_Value_Delete_Value)
{
    {
        int counter1=0;
        Caching::CachedValue<int> v1([&counter1] () { return ++counter1; }, "Test Function #1");

        {
            int counter2=0;
            Caching::CachedValue<int> v2([&counter2] () { return ++counter2; }, "Test Function #2");

            BOOST_CHECK_EQUAL(v1(), 1);
            BOOST_CHECK_EQUAL(v1(), 1);

            BOOST_CHECK_EQUAL(v2(), 1);
            BOOST_CHECK_EQUAL(v2(), 1);
        }
        Caching::Clear_Caches();
    
        BOOST_CHECK_EQUAL(v1(), 2);
        BOOST_CHECK_EQUAL(v1(), 2);
    }
}

/*
 * CachedFuntion and CachedValue don't interfere with each other.
 */
BOOST_AUTO_TEST_CASE(Cached_Function_and_Value)
{
    int counter1=0;
    Caching::CachedValue<int> v([&counter1] () { return ++counter1; }, "Test Function #1");

    int counter2=0;
    Caching::CachedFunction<int, double> f([&counter2] (double) { return ++counter2; }, "Test Function #2");

    BOOST_CHECK_EQUAL(v(), 1);
    BOOST_CHECK_EQUAL(v(), 1);

    BOOST_CHECK_EQUAL(f(0.0), 1);
    BOOST_CHECK_EQUAL(f(1.0), 2);

    BOOST_CHECK_EQUAL(f(0.0), 1);
    BOOST_CHECK_EQUAL(f(1.0), 2);

    v.ClearCache();
    BOOST_CHECK_EQUAL(v(), 2);
    BOOST_CHECK_EQUAL(f(0.0), 1);
    BOOST_CHECK_EQUAL(f(1.0), 2);

    f.ClearCache();
    BOOST_CHECK_EQUAL(v(), 2);
    BOOST_CHECK_EQUAL(f(0.0), 3);
    BOOST_CHECK_EQUAL(f(1.0), 4);

    Caching::Clear_Caches();
    BOOST_CHECK_EQUAL(v(), 3);
    BOOST_CHECK_EQUAL(f(0.0), 5);
    BOOST_CHECK_EQUAL(f(1.0), 6);
}

