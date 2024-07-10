#pragma once
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <tuple>
#include <utility>

namespace CTC {

template <typename T, typename STYPE = ::std::size_t>
struct CTDArray {
private:
    T*    data_     = nullptr;
    STYPE size_     = 0;
    STYPE capacity_ = 0;

public:
    using value_type = T;
    using size_type  = STYPE;
    constexpr auto&       data() { return data_; }
    constexpr const auto& data() const { return data_; }
    constexpr auto        size() const { return size_; }
    constexpr CTDArray() {
        data_     = nullptr;
        size_     = 0;
        capacity_ = 0;
    }
    constexpr CTDArray(const CTDArray& o) {
        if (o.size_ == 0 || o.data_ == nullptr) return;
        delete_if_has_data();
        new_length(o.size_);
        for (size_type i = 0; i < o.size_; i++) data_[i] = o.data_[i];
    }
    constexpr CTDArray(CTDArray&& o) {
        size_ = o.size_;
        data_ = o.data_;
        o.delete_if_has_data();
    }
    template <typename... Ts>
    constexpr CTDArray(Ts... args) {
        new_length(sizeof...(args));
        setter<0>(data_, args...);
    }
    constexpr ~CTDArray() { delete_if_has_data(); }
    constexpr auto&       operator[](size_type i) { return &data_[i]; }
    constexpr const auto& operator[](size_type i) const { return &data_[i]; }
    constexpr auto        begin() { return &data_[0]; }
    constexpr const auto  begin() const { return &data_[0]; }
    constexpr auto        end() { return &data_[size_]; }
    constexpr const auto  end() const { return &data_[size_]; }
    constexpr auto&       push_back(const T& v) {
        new_length<true>(size_ + 1);
        data_[size_ - 1] = v;
        return data_[size_ - 1];
    }
    constexpr auto& push_back(T&& v) {
        new_length<true>(size_ + 1);
        data_[size_ - 1] = std::move(v);
        return data_[size_ - 1];
    }
    constexpr auto pop_back() {
        auto t = data_[size_ - 1];
        new_length(size_ - 1);
        return t;
    }
    constexpr void resize(size_type s) { new_length<true>(s); }
    constexpr auto max_size() { return std::numeric_limits<size_type>::max(); }
    constexpr auto capacity() { return capacity_; }

private:
    constexpr void delete_if_has_data() {
        if (data_ != nullptr && size_ != 0) delete[] data_;
        data_ = nullptr;
        size_ = 0;
    }
    template <bool CopyOldData = false>
    constexpr void new_length(size_type l) {
        if (capacity_ == 0) {
            data_     = new T[2];
            capacity_ = 2;
        }
        if (l < capacity_) {
            size_ = l;
            return;
        }
        auto temp_data = data_;
        data_          = nullptr;
        if (capacity_ + capacity_ / 2 > max_size()) {
            data_     = new T[max_size()];
            capacity_ = max_size();
        } else {
            data_     = new T[capacity_ + capacity_ / 2];
            capacity_ = capacity_ + capacity_ / 2;
        }
        if constexpr (CopyOldData) {
            if (temp_data != nullptr)
                for (size_type i = 0; i < std::min(size_, l); i++) {
                    data_[i] = std::move(temp_data[i]);
                }
        }
        delete[] temp_data;
        size_ = l;
    }
    template <auto I>
    constexpr auto setter(auto& data, const auto&... args) {
        data[I] = std::get<I>(std::forward_as_tuple(args...));
        if constexpr (I + 1 < sizeof...(args)) setter<I + 1>(data, args...);
    }
};
template <typename T, typename... Ts>
CTDArray(T first, Ts... rest) -> CTDArray<std::remove_cvref_t<T>>;
} // namespace CTC
