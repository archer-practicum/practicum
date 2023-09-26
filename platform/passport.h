#pragma once

#include "identity_document.h"
#include <iostream>
#include <string>
#include <ctime>

using namespace std::string_view_literals;

class Passport {
public:
    Passport()
        : expiration_date_(GetExpirationDate()) 
    {
        Passport::SetVTable(this);
        std::cout << "Passport::Ctor()"sv << std::endl;
    }

    Passport(const Passport& other)
        : m_identity_document(other.m_identity_document)
        , expiration_date_(other.expiration_date_) 
    {
        Passport::SetVTable(this);
        std::cout << "Passport::CCtor()"sv << std::endl;
    }

    ~Passport() {
        std::cout << "Passport::Dtor()"sv << std::endl;
        IdentityDocument::SetVTable((IdentityDocument*)this);
    }

    static void SetVTable(Passport* obj) {
        *(Passport::Vtable**)obj = &Passport::VTABLE;
    }
    
    using PrintIdFunction = void(*)(const Passport*);
    using PrintVisaFunction = void(*)(const Passport*, const std::string&);
    using DeleteFunction = void(*)(Passport*);    

    struct Vtable {
        DeleteFunction delete_this;
        const PrintIdFunction print_id_this;
        const PrintVisaFunction print_visa_this;
    };

    void Delete() {
        GetVtable()->delete_this(this);
    }

    void PrintID() const {
        GetVtable()->print_id_this(this);
    }

    void PrintVisa(const std::string& country) const {
        GetVtable()->print_visa_this(this, country);
    }

    operator IdentityDocument() {
        return {m_identity_document};
    }

    const Vtable* GetVtable() const {
        return (const Passport::Vtable*)m_identity_document.GetVtable();
    }

    Vtable* GetVtable() {
        return (Passport::Vtable*)m_identity_document.GetVtable();
    }

private:
    IdentityDocument m_identity_document;

    static Passport::Vtable VTABLE;

    static void Delete(Passport* obj) {
        delete obj;
    }

    static void PrintID(const Passport* passport) {
        std::cout << "Passport::PrintID() : "sv << passport->m_identity_document.GetID();
        std::cout << " expiration date : "sv << passport->expiration_date_.tm_mday << "/"sv << passport->expiration_date_.tm_mon << "/"sv
                  << passport->expiration_date_.tm_year + 1900 << std::endl;
    }

    static void PrintVisa(const Passport* obj, const std::string& country) {
        std::cout << "Passport::PrintVisa("sv << country << ") : "sv << obj->m_identity_document.GetID() << std::endl;
    }

    const struct tm expiration_date_;

    tm GetExpirationDate() {
	    time_t t = time(nullptr);
	    tm exp_date = *localtime(&t);
	    exp_date.tm_year += 10;
	    mktime(&exp_date);
	    return exp_date;
	}
};

Passport::Vtable Passport::VTABLE = { Passport::Delete,  Passport::PrintID, Passport::PrintVisa };