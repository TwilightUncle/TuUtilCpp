///----------------------------------
/// @file mpl.hpp 
/// @brief オレオレメタプログラミング用ライブラリ
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP

#include <concepts>

namespace tuutil::mpl
{
    /**
     * @class
     * @brief 未発見だったり空だったりした場合に入る型
    */
    struct ignore_type {};
}

#include <tuutilcpp/mpl/meta_callback.hpp>
#include <tuutilcpp/mpl/map.hpp>
#include <tuutilcpp/mpl/fold.hpp>
#include <tuutilcpp/mpl/find_if.hpp>
#include <tuutilcpp/mpl/extract_if.hpp>

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP
