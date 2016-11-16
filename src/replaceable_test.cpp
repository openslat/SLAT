/**
 * @file   replaceable_test.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Fri Jan 22 14:57:54 NZDT 2016
 * 
 * @brief Unit tests for the 'Replaceable' template class.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2016 Canterbury University
 */
#include <list>
#include "replaceable.h"
#include <iostream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace SLAT;

/*
 * Define a class for testing. The constructor accepts an integer;
 * GetSecretValue() returns it.
 */
class TestClass : public Replaceable<TestClass> {
public:
    TestClass(int i) { secret_value = i; };
    int GetSecretValue(void) { return secret_value; };
private:
    int secret_value;
};

/*
 * Define a test class using TestClass. It includes a shared pointer to a
 * TestClass object; its Value() method returns the "secret value" of the
 * TestClass. Replacing the TestClass changes the result of the Value() method.
 */
class TestClassClient {
protected:
    std::shared_ptr<TestClass> service;
    int callback_id;
public:
    TestClassClient(std::shared_ptr<TestClass> s) {
        service = s;

        /*
         * Install callbacks to service. Empty 'change' callback; 'notify'
         * callback replaces service.
         */
        callback_id = service->add_callbacks(
            [this] (void) { },
            [this] (std::shared_ptr<TestClass> new_service) {
                this->service = new_service;
            });
    };
    
    int Value(void) { return service->GetSecretValue(); };
};

/*
 * Create a TestClassClient, and make sure the replace() method works.
 */
BOOST_AUTO_TEST_CASE(Replaceable_Test_1)
{
    std::shared_ptr<TestClass> tc1 = std::make_shared<TestClass>(1);
    std::shared_ptr<TestClass> tc2 = std::make_shared<TestClass>(2);
    TestClassClient client(tc1);
    BOOST_CHECK_EQUAL(tc1->GetSecretValue(), 1);
    BOOST_CHECK_EQUAL(tc2->GetSecretValue(), 2);
    BOOST_CHECK_EQUAL(client.Value(), 1);
    tc1->replace(tc2);
    BOOST_CHECK_EQUAL(tc1->GetSecretValue(), 1);
    BOOST_CHECK_EQUAL(tc2->GetSecretValue(), 2);
    BOOST_CHECK_EQUAL(client.Value(), 2);
}

/*
 * Test the remove_callbacks() method:
 */
BOOST_AUTO_TEST_CASE(Replaceable_Test_2)
{
/*
 * Define a test class using TestClass. It includes a shared pointer to a
 * TestClass object; its Value() method returns the "secret value" of the
 * TestClass. Replacing the TestClass changes the result of the Value() method.
 */
    class TestClassClient2 : public TestClassClient {
    private: 
        int change_counter;
    public:
        TestClassClient2(std::shared_ptr<TestClass> s) : TestClassClient(s) {};
        void RemoveCallbacks() {
            service->remove_callbacks(callback_id);
        };
    };

    std::shared_ptr<TestClass> tc1 = std::make_shared<TestClass>(1);
    std::shared_ptr<TestClass> tc2 = std::make_shared<TestClass>(2);
    TestClassClient2 client(tc1);
    BOOST_CHECK_EQUAL(tc1->GetSecretValue(), 1);
    BOOST_CHECK_EQUAL(tc2->GetSecretValue(), 2);
    BOOST_CHECK_EQUAL(client.Value(), 1);

    // Remove callbacks before replacing TestClass. client.Value() should not be affected.
    client.RemoveCallbacks();
    tc1->replace(tc2);
    BOOST_CHECK_EQUAL(tc1->GetSecretValue(), 1);
    BOOST_CHECK_EQUAL(tc2->GetSecretValue(), 2);
    BOOST_CHECK_EQUAL(client.Value(), 1);

    // Remove callbacks again to make sure that doesn't cause any problems:
    client.RemoveCallbacks();
    tc1->replace(tc2);
    BOOST_CHECK_EQUAL(tc1->GetSecretValue(), 1);
    BOOST_CHECK_EQUAL(tc2->GetSecretValue(), 2);
    BOOST_CHECK_EQUAL(client.Value(), 1);
}

/*
 * Test the change notification mechanism:
 */
BOOST_AUTO_TEST_CASE(Replaceable_Test_3)
{
    class TestClassClient2 {
    protected:
        std::shared_ptr<TestClass> service;
        int callback_id;
        int changes;
    public:
        TestClassClient2(std::shared_ptr<TestClass> s) {
            service = s;
            changes = 0;

            /*
             * Install callbacks to service. Empty 'change' callback; 'notify'
             * callback replaces service.
             */
            callback_id = service->add_callbacks(
                [this] (void) { this->changes++; },
                [this] (std::shared_ptr<TestClass> new_service) {
                    this->service = new_service;
                });
        };
        void RemoveCallbacks() {
            service->remove_callbacks(callback_id);
        };
    
        int Value(void) { return service->GetSecretValue(); };
        int Changes(void) { return changes; };
    };
    std::shared_ptr<TestClass> tc1 = std::make_shared<TestClass>(1);
    TestClassClient2 client(tc1);

    BOOST_CHECK_EQUAL(client.Changes(), 0);
    BOOST_CHECK_EQUAL(client.Changes(), 0);

    tc1->notify_change();
    BOOST_CHECK_EQUAL(client.Changes(), 1);

    // Remove callbacks again to make sure that doesn't cause any problems:
    client.RemoveCallbacks();
    tc1->notify_change();
    BOOST_CHECK_EQUAL(client.Changes(), 1);
    BOOST_CHECK_EQUAL(client.Changes(), 1);
}


