/*
    pybind11/shared_ptr.h: Partial template specializations to shared_ptr cast
    Includes fix due to https://github.com/pybind/pybind11/issues/1546#issuecomment-526712704

    Copyright (c) 2020 Andrey Asadchev <asadchev@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include "pybind11.h" // gil
#include "cast.h"
#include <memory>

NAMESPACE_BEGIN(PYBIND11_NAMESPACE)
NAMESPACE_BEGIN(detail)

/// Shared object crash fix: https://github.com/pybind/pybind11/issues/1546#issuecomment-526712704

/// Specialize for the common std::shared_ptr, so users don't need to
template <typename T>
class type_caster<std::shared_ptr<T>> {

    PYBIND11_TYPE_CASTER (std::shared_ptr<T>, _(PYBIND11_STRING_NAME));

    using BaseCaster = copyable_holder_caster<T, std::shared_ptr<T>>;

    bool load (pybind11::handle src, bool b)
    {
        BaseCaster bc;
        bool success = bc.load (src, b);
        if (!success)
        {
            return false;
        }

	auto base_ptr = static_cast<std::shared_ptr<T>> (bc);
	auto h = BaseCaster::cast(base_ptr, return_value_policy(), handle());
	auto py_obj = reinterpret_borrow<object>(h);

        auto py_obj_ptr = std::shared_ptr<object>{
            new object{py_obj},
            [](auto py_object_ptr) {
                // It's possible that when the shared_ptr dies we won't have the
                // gil (if the last holder is in a non-Python thread), so we
                // make sure to acquire it in the deleter.
                gil_scoped_acquire gil;
                delete py_object_ptr;
           }
        };

        value = std::shared_ptr<T> (py_obj_ptr, base_ptr.get ());
        return true;
    }

    static handle cast (std::shared_ptr<T> sp,
                        return_value_policy rvp,
                        handle h)
    {
        return BaseCaster::cast (sp, rvp, h);
    }
};

// Specialization for shared_ptr:
template <typename base> struct is_holder_type<base, std::shared_ptr<base>> :
    std::true_type {};

NAMESPACE_END(detail)
NAMESPACE_END(PYBIND11_NAMESPACE)
