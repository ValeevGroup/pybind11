#pragma once

#include "../../stl.h"
#include <boost/container/small_vector.hpp>

NAMESPACE_BEGIN(PYBIND11_NAMESPACE)
NAMESPACE_BEGIN(detail)

  template <typename T, std::size_t N>
  struct type_caster< boost::container::small_vector<T,N> >
    : list_caster< boost::container::small_vector<T,N>, T > { };

NAMESPACE_END(detail)
NAMESPACE_END(PYBIND11_NAMESPACE)
