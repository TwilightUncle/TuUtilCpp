///----------------------------------
/// @file mpl.hpp 
/// @brief �I���I�����^�v���O���~���O�p���C�u����
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP

#include <concepts>

namespace tuutil::mpl
{
    /**
     * @class
     * @brief ��������������󂾂����肵���ꍇ�ɓ���^
    */
    struct ignore_type {};

    /**
     * @class
     * @brief �Ƃ肠�����p�����[�^�p�b�N��ێ��������Ƃ��ɗ��p
    */
    template <class... Types> struct type_list {};

    /**
     * @class
     * @brief �Ƃ肠�����A�l�̃p�����[�^�p�b�N��ێ��������Ƃ��ɗ��p
    */
    template <auto... Values> struct value_list {};
}

#include <tuutilcpp/mpl/concepts.hpp>
#include <tuutilcpp/mpl/value_constant.hpp>
#include <tuutilcpp/mpl/meta_callback.hpp>
#include <tuutilcpp/mpl/get_element.hpp>
#include <tuutilcpp/mpl/mpl_if.hpp>
#include <tuutilcpp/mpl/map.hpp>
#include <tuutilcpp/mpl/fold.hpp>
#include <tuutilcpp/mpl/push.hpp>
#include <tuutilcpp/mpl/parameter_pack.hpp>
#include <tuutilcpp/mpl/argment.hpp>
#include <tuutilcpp/mpl/find_if.hpp>
#include <tuutilcpp/mpl/extract_if.hpp>
#include <tuutilcpp/mpl/unique.hpp>
#include <tuutilcpp/mpl/count.hpp>

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP
