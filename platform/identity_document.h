#pragma once

#include <iostream>
#include <string>

using namespace std::string_view_literals;

class IdentityDocument {

    friend class DrivingLicence;    
    friend class InternationalDrivingLicence;    
    friend class Passport;  

public:
    IdentityDocument()
        : unique_id_(++unique_id_count_) 
    {
        SetVTable(this);
        std::cout << "IdentityDocument::Ctor() : "sv << unique_id_ << std::endl;
    }

    ~IdentityDocument() {
        --unique_id_count_;
        std::cout << "IdentityDocument::Dtor() : "sv << unique_id_ << std::endl;
    }

    IdentityDocument(const IdentityDocument& other)
        : unique_id_(++unique_id_count_) 
    {
        SetVTable(this);
        std::cout << "IdentityDocument::CCtor() : "sv << unique_id_ << std::endl;
    }

    IdentityDocument& operator=(const IdentityDocument&) = delete;

    static void PrintUniqueIDCount() {
        std::cout << "unique_id_count_ : "sv << unique_id_count_ << std::endl;
    }

    static void SetVTable(IdentityDocument* obj) {
        //(new-type) expression
        *(IdentityDocument::Vtable**)obj = &IdentityDocument::VTABLE;
    }

    using DeleteFunction = void(*)(IdentityDocument*);
    using PrintIDFunction = void(*)(const IdentityDocument*);

    struct Vtable {
        DeleteFunction delete_this;
        const PrintIDFunction print_id_this;
    };

    void PrintID() const {
        vptr_->print_id_this(this);
    }

    void Delete() {
        GetVtable()->delete_this(this);
    }

    const IdentityDocument::Vtable* GetVtable() const {
        return vptr_;
    }

    IdentityDocument::Vtable* GetVtable() {
        return vptr_;
    }

protected:
    int GetID() const {
        return unique_id_;
    }

private:
    static int unique_id_count_;
    int unique_id_;

    IdentityDocument::Vtable* vptr_ = nullptr;
    static IdentityDocument::Vtable VTABLE;

    static void Delete(IdentityDocument* obj) {
        //В этот момент тип объекта известен. Просто удаляем указатель.
        //Вызов delete запустит процесс вызовов деструкторов
        delete obj;
    }

    static void PrintID(const IdentityDocument* doc) {
        std::cout << "IdentityDocument::PrintID() : "sv << doc->unique_id_ << std::endl;
    }
};

int IdentityDocument::unique_id_count_ = 0;

IdentityDocument::Vtable IdentityDocument::VTABLE = { IdentityDocument::Delete, IdentityDocument::PrintID };