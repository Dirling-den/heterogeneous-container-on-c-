#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include <stdexcept>

class TypeBox {
private:
    struct BaseStorage {
        virtual ~BaseStorage() = default;
    };

    template <typename T>
    struct Storage : BaseStorage {
        std::vector<T> data;
    };

    std::unordered_map<std::type_index, std::unique_ptr<BaseStorage>> storages;

    template <typename T>
    Storage<T>& ensure() {
        auto key = std::type_index(typeid(T));
        auto it = storages.find(key);
        if (it == storages.end()) {
            auto ptr = std::make_unique<Storage<T>>();
            auto* raw = ptr.get();
            storages.emplace(key, std::move(ptr));
            return *raw;
        }
        return *static_cast<Storage<T>*>(it->second.get());
    }

    template <typename T>
    const Storage<T>& getStorage() const {
        auto key = std::type_index(typeid(T));
        auto it = storages.find(key);
        if (it == storages.end()) {
            throw std::runtime_error("Type not found");
        }
        return *static_cast<const Storage<T>*>(it->second.get());
    }

public:
    template <typename T>
    void add(const T& value) {
        ensure<T>().data.push_back(value);
    }

    template <typename T>
    std::vector<T>& get() {
        return ensure<T>().data;
    }

    template <typename T>
    const std::vector<T>& get() const {
        return getStorage<T>().data;
    }

    template <typename T>
    size_t size() const {
        return getStorage<T>().data.size();
    }

    template <typename T>
    void clear() {
        ensure<T>().data.clear();
    }

    template <typename T>
    T& at(size_t index) {
        auto& vec = ensure<T>().data;
        if (index >= vec.size()) {
            throw std::out_of_range("Index out of range");
        }
        return vec[index];
    }

    template <typename T>
    const T& at(size_t index) const {
        auto& vec = getStorage<T>().data;
        if (index >= vec.size()) {
            throw std::out_of_range("Index out of range");
        }
        return vec[index];
    }
};