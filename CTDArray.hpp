#pragma once
#include <cstdlib>
#include <tuple>
#include <utility>
namespace CTC {

template <typename T, typename STYPE = ::std::size_t>
struct CTDArray {
private:
    T*    data_;
    STYPE size_;

public:
    using value_type = T;
    using size_type  = STYPE;
    constexpr auto&       data() { return data_; }
    constexpr const auto& data() const { return data_; }
    constexpr auto        size() const { return size_; }
    constexpr CTDArray() {
        data_ = nullptr;
        size_ = 0;
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
        auto new_data = new T[size_ + 1];
        for (size_type i = 0; i < size_; i++) new_data[i] = std::move(data_[i]);
        new_data[size_] = v;
        size_++;
        if (data_ != nullptr && size_ != 0) delete[] data_;
        data_ = new_data;
        return data_[size_ - 1];
    }
    constexpr auto& push_back(T&& v) {
        auto new_data = new T[size_ + 1];
        for (size_type i = 0; i < size_; i++) new_data[i] = std::move(data_[i]);
        new_data[size_] = std::move(v);
        size_++;
        if (data_ != nullptr && size_ != 0) delete[] data_;
        data_ = new_data;
        return data_[size_ - 1];
    }
    constexpr void resize(size_type s) {
        auto new_data = new T[s];
        for (size_type i = 0; i < std::min(s, size_); i++) new_data[i] = std::move(data_[i]);
        delete_if_has_data();
        data_ = new_data;
        size_ = s;
    }

private:
    constexpr void delete_if_has_data() {
        if (data_ != nullptr && size_ != 0) delete[] data_;
        data_ = nullptr;
        size_ = 0;
    }
    constexpr void new_length(size_type l) {
        data_ = new T[l];
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