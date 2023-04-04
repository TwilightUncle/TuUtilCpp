///----------------------------------
/// @file mpl/reverse.hpp 
/// @brief �^�p�����[�^�p�b�N�̕��т𔽓]
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief std::conditional�̃��b�p�[�B����֐��ƌ����Ώۂ�n���A�x���]���ł���悤�ɂ�������
     * @tparam Pred ����֐�
     * @tparam Arg Pred�ɐH�킹�锻��Ώی^
     * @tparam T apply_v<Pred, Arg>���^�̏ꍇ�ԋp�����^
     * @tparam F apply_v<Pred, Arg>���U�̏ꍇ�ԋp�����^
    */
    template <MetaCallable Pred, class Arg, class T, class F>
    using mpl_if = std::conditional<apply_v<Pred, Arg>, T, F>;

    /**
     * @fn
     * @brief std::conditional�̃��b�p�[�B����֐��ƌ����Ώۂ�n���A�x���]���ł���悤�ɂ�������
     * @tparam Pred ����֐�
     * @tparam Arg Pred�ɐH�킹�锻��Ώی^
     * @tparam T apply_v<Pred, Arg>���^�̏ꍇ�ԋp�����^
     * @tparam F apply_v<Pred, Arg>���U�̏ꍇ�ԋp�����^
    */
    template <MetaCallable Pred, class Arg, class T, class F> using mpl_if_t = typename mpl_if<Pred, Arg, T, F>::type;

    /**
     * @fn
     * @brief �������w������ɍ��v����ꍇ�̂݁A���^�֐���K�p����
     * @tparam Pred ��������������֐�
     * @tparam F ���^�֐�
     * @tparam Arg ����
    */
    template <MetaCallable Pred, MetaCallable F, class Arg>
    using apply_if = mpl_if<Pred, Arg, apply_t<F, Arg>, Arg>;

    /**
     * @fn
     * @brief �������w������ɍ��v����ꍇ�̂݁A���^�֐���K�p����
     * @tparam Pred ��������������֐�
     * @tparam F ���^�֐�
     * @tparam Arg ����
    */
    template <MetaCallable Pred, MetaCallable F, class Arg> using apply_if_t = typename apply_if<Pred, F, Arg>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_IF_HPP
