#pragma once

#include "identity_document.h"
#include <iostream>
#include <string>

using namespace std::string_view_literals;

class DrivingLicence {

    friend class InternationalDrivingLicence;

public:
    DrivingLicence() {
        DrivingLicence::SetVTable(this);
        std::cout << "DrivingLicence::Ctor()"sv << std::endl;
    }

    DrivingLicence(const DrivingLicence& other) : m_identity_document(other.m_identity_document) {
        DrivingLicence::SetVTable(this);
        std::cout << "DrivingLicence::CCtor()"sv << std::endl;
    }

    ~DrivingLicence() {
        std::cout << "DrivingLicence::Dtor()"sv << std::endl;
        IdentityDocument::SetVTable((IdentityDocument*)this);
    }

    static void SetVTable(DrivingLicence* obj) {
        *(DrivingLicence::Vtable**)obj = &DrivingLicence::VTABLE;
    }

    using PrintIDFunction = void(*)(const DrivingLicence*);
    using DeleteFunction = void(*)(DrivingLicence*);

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

    operator IdentityDocument() {
        return {m_identity_document};
    }

    const Vtable* GetVtable() const {
        return (const DrivingLicence::Vtable*)m_identity_document.GetVtable();
    }

    Vtable* GetVtable() {
        return (DrivingLicence::Vtable*)m_identity_document.GetVtable();
    }

    // void PrintID() const {
    //     std::cout << "DrivingLicence::PrintID() : "sv << m_identity_document.GetID() << std::endl; 
    // }
private:
    IdentityDocument m_identity_document;

    static DrivingLicence::Vtable VTABLE;

    static void Delete(DrivingLicence* obj) {
        delete obj;
    }

    static void PrintID(const DrivingLicence* licence) {
        std::cout << "DrivingLicence::PrintID() : "sv << licence->m_identity_document.GetID() << std::endl;
    }
};

DrivingLicence::Vtable DrivingLicence::VTABLE = { DrivingLicence::Delete,  DrivingLicence::PrintID };