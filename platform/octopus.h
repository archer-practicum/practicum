#pragma once

#include "ptrvector.h"
#include "scopedptr.h"

#include <new> // Для исключения bad_alloc
#include <vector>

// Щупальце
class Tentacle {
public:
    explicit Tentacle(int id) noexcept
        : id_(id) {
    }

    int GetId() const noexcept {
        return id_;
    }

    Tentacle* GetLinkedTentacle() const noexcept {
        return linked_tentacle_;
    }
    void LinkTo(Tentacle& tentacle) noexcept {
        linked_tentacle_ = &tentacle;
    }
    void Unlink() noexcept {
        linked_tentacle_ = nullptr;
    }

private:
    int id_ = 0;
    Tentacle* linked_tentacle_ = nullptr;
};

// Осьминог
class Octopus {
public:
    Octopus()
        : Octopus(8) {
    }

    explicit Octopus(int num_tentacles) {
        tentacles_.reserve(static_cast<std::size_t>(num_tentacles));
        Tentacle* t = nullptr;
        ScopedPtr<Tentacle>* scoped_ptr = nullptr;
        try {
            for (int i = 0; i < num_tentacles; ++i) {
                t = new Tentacle(i + 1);
                scoped_ptr = new ScopedPtr<Tentacle>(t);
                tentacles_.emplace_back(scoped_ptr);
            }
        } catch (...) {
            delete scoped_ptr;
            delete t;
            Cleanup();
            throw;
        }                
    }

    Octopus(const Octopus& other) {
        tentacles_.reserve(other.GetTentacleCount());
        Tentacle* t = nullptr;
        ScopedPtr<Tentacle>* scoped_ptr = nullptr;
        try {
            for (const ScopedPtr<Tentacle>* el : other.tentacles_) {
                el->GetRawPtr()->GetId();
                t = new Tentacle(el->GetRawPtr()->GetId());
                scoped_ptr = new ScopedPtr<Tentacle>(t);
                t->LinkTo(*el->GetRawPtr()->GetLinkedTentacle());
                tentacles_.emplace_back(scoped_ptr);
            }
        } catch (...) {
            delete scoped_ptr;
            delete t;            
            Cleanup();
            throw;
        }        
    }

    ~Octopus() {
        // Осьминог владеет объектами в динамической памяти (щупальца),
        // которые должны быть удалены при его разрушении.
        // Деструктор - лучшее место, чтобы прибраться за собой.
        Cleanup();
    }

    Octopus& operator=(const Octopus& rht) {
        if (this == &rht) return *this;

        auto copy_rht(rht);

        swap(copy_rht);

        return *this;
    }

    void swap(Octopus& rht) noexcept {
        tentacles_.swap(rht.tentacles_);
    }

    Tentacle& AddTentacle() {
        Tentacle* t = nullptr;
        ScopedPtr<Tentacle>* scoped_ptr = nullptr;
        try {
            t = new Tentacle(GetTentacleCount() + 1);
            scoped_ptr = new ScopedPtr<Tentacle>(t);
            tentacles_.emplace_back(scoped_ptr);
        } catch (...) {
            delete scoped_ptr;
            delete t;            
            Cleanup();            
            throw;
        }
        return *tentacles_.back()->GetRawPtr();
    }

    int GetTentacleCount() const noexcept {
        return static_cast<int>(tentacles_.size());
    }

    const Tentacle& GetTentacle(size_t index) const {
        return *tentacles_.at(index)->GetRawPtr();
    }
    Tentacle& GetTentacle(size_t index) {
        return *tentacles_.at(index)->GetRawPtr();
    }

private:
    void Cleanup() {        
        // Очищаем массив указателей на щупальца
        for (const auto& el : tentacles_) {
            delete el;
        }
        tentacles_.clear();
    }

    // Вектор хранит указатели на щупальца. Сами объекты щупалец находятся в куче
    std::vector<ScopedPtr<Tentacle>*> tentacles_;
};