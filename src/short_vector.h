//
// Created by siy on 4/2/18.
//

#include <cstddef>
#include <utility>
#include <cassert>
#include <algorithm>
#include <iostream>
#include "bitmixer.h"

#ifndef VCNC_VEC_H
#define VCNC_VEC_H

template <typename ElementType, size_t Size>
class short_vector {
        static_assert(Size < std::numeric_limits<size_t>::digits, "Vector size cannot exceed size of size_t in bits");
    public:
        ElementType data[Size];

        using SelfType = short_vector<ElementType, Size>;

        short_vector():data{} {
        }

//        short_vector(ElementType initialValue) {
//            std::fill_n(std::begin(data), Size, initialValue);
//        }

//        void fill(ElementType initialValue) {
//            std::fill_n(std::begin(data), Size, initialValue);
//        }

        template <typename ... Args>
        explicit short_vector(Args ... args): data{ElementType(args) ...} {
        }

        short_vector(const SelfType& other) {
            std::copy(std::begin(other.data), std::end(other.data), std::begin(data));
        }

        short_vector(const SelfType&& other) noexcept {
            std::copy(std::begin(other.data), std::end(other.data), std::begin(data));
        }

        short_vector& operator= (const SelfType& other) {
            std::copy(std::begin(other.data), std::end(other.data), std::begin(data));
            return *this;
        }

        template <typename OtherElementType>
        short_vector(short_vector<OtherElementType, Size>& other, ElementType scaleFactor) {
            std::transform(std::begin(other.data),
                           std::end(other.data),
                           std::begin(data),
                           [=] (OtherElementType value) { return ElementType(value * scaleFactor);});
        }

        template <typename ScalerType>
        short_vector& operator*= (ScalerType scaleFactor) {
            std::transform(std::begin(data),
                           std::end(data),
                           std::begin(data),
                           [=] (ElementType value) { return ElementType(value * scaleFactor);});
        }

        template <typename ScalerType>
        short_vector& operator/= (const ScalerType scaleFactor) {
            std::transform(std::begin(data),
                           std::end(data),
                           std::begin(data),
                           [=] (ElementType value) { return ElementType(value / scaleFactor);});
        }

//        short_vector& operator/= (const SelfType& other) {
//            for (int i = 0; i < Size; ++i) {
//                (other.data == 0) ? data[i] = 0 : data[i] / other.data[i];
//            }
//            return *this;
//        }

        short_vector& operator+= (const SelfType other) {
            for (int i = 0; i < Size; ++i) {
                data[i] += other.data[i];
            }
            return *this;
        }

        short_vector& operator-= (const SelfType other) {
            for (int i = 0; i < Size; ++i) {
                data[i] -= other.data[i];
            }
            return *this;
        }

        template <typename T>
        short_vector& operator>>= (const T count) {
            for (int i = 0; i < Size; ++i) {
                data[i] >>= count;
            }
            return *this;
        }

        template <typename T>
        short_vector& operator&= (const T mask) {
            for (int i = 0; i < Size; ++i) {
                data[i] &= mask;
            }
            return *this;
        }

        SelfType operator/ (const SelfType other) {
            SelfType result(*this);
            for (int i = 0; i < Size; ++i) {
                result.data[i] /= other.data[i];
            }
            return result;
        }

        template <typename ScalerType>
        SelfType operator/ (const ScalerType other) {
            SelfType result(*this);
            for (int i = 0; i < Size; ++i) {
                result.data[i] /= other;
            }
            return result;
        }

        SelfType operator* (const SelfType other) {
            SelfType result(*this);
            for (int i = 0; i < Size; ++i) {
                result.data[i] *= other.data[i];
            }
            return result;
        }

        template <typename ScalerType>
        SelfType operator* (const ScalerType other) {
            SelfType result(*this);
            for (int i = 0; i < Size; ++i) {
                result.data[i] *= other;
            }
            return result;
        }

        const ElementType& operator[](int idx) {
            assert(idx < Size);
            return data[idx];
        }

        template <typename T, size_t S, bit_name ... Bits>
        bitmixer::bit_collector<T, S, Bits...>& step_and_reset(size_t mask, bitmixer::bit_collector<T, S, Bits...>& collector) {
            for (int i = 0; i < Size; ++i) {
                collector.add(data[i] & ~mask);
                data[i] &= mask;
            }
            return collector;
        }

        template <typename T, size_t S, bit_name ... Bits>
        void abs(bitmixer::bit_collector<T, S, Bits...>& collector) {
            for (int i = 0; i < Size; ++i) {
                collector.add(data[i] < 0);
                data[i] = std::abs(data[i]);
            }
        }

        template <typename T, size_t S>
        friend std::ostream& operator<< (std::ostream& os, const short_vector<T, S>& vec);
};

template <typename T, size_t Size>
std::ostream& operator<< (std::ostream& os, const short_vector<T, Size>& vec) {
    os << "<";
    for(T value: vec.data) {
        os << value << ", ";
    }
    os << ">";
    return os;
};

#endif //VCNC_VEC_H
