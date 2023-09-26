#pragma once

#include "driving_licence.h"
#include <iostream>
#include <string>

using namespace std::string_view_literals;

class InternationalDrivingLicence {
public:
    InternationalDrivingLicence() {
        InternationalDrivingLicence::SetVTable(this);
        std::cout << "InternationalDrivingLicence::Ctor()"sv << std::endl;
    }

    InternationalDrivingLicence(const InternationalDrivingLicence& other)
        : m_driving_licence(other.m_driving_licence) 
    {
        InternationalDrivingLicence::SetVTable(this);
        std::cout << "InternationalDrivingLicence::CCtor()"sv << std::endl;
    }

    ~InternationalDrivingLicence() {
        std::cout << "InternationalDrivingLicence::Dtor()"sv << std::endl;
        DrivingLicence::SetVTable((DrivingLicence*)this);
    }

    static void SetVTable(InternationalDrivingLicence* obj) {
        *(InternationalDrivingLicence::Vtable**)obj = &InternationalDrivingLicence::VTABLE;
    }
    
    using PrintIDFunction = void(*)(const InternationalDrivingLicence*);
    using DeleteFunction = void(*)(InternationalDrivingLicence*);

    struct Vtable {
        DeleteFunction delete_this;
        const PrintIDFunction print_id_this;
    };

    void Delete() {
        GetVtable()->delete_this(this);
    }

    void PrintID() const {
        GetVtable()->print_id_this(this);
    }

    operator DrivingLicence() {
        return {m_driving_licence};
    }

    const Vtable* GetVtable() const {
        return (const InternationalDrivingLicence::Vtable*)m_driving_licence.GetVtable();
    }

    Vtable* GetVtable() {
        return (InternationalDrivingLicence::Vtable*)m_driving_licence.GetVtable();
    }


private:
    DrivingLicence m_driving_licence;

    static InternationalDrivingLicence::Vtable VTABLE;

    static void Delete(InternationalDrivingLicence* obj) {
        delete obj;
    }

    static void PrintID(const InternationalDrivingLicence* licence) {
        std::cout << "InternationalDrivingLicence::PrintID() : "sv << licence->m_driving_licence.m_identity_document.GetID() << std::endl;
    }
};

InternationalDrivingLicence::Vtable InternationalDrivingLicence::VTABLE = { InternationalDrivingLicence::Delete,  InternationalDrivingLicence::PrintID };