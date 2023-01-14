///----------------------------------
/// @file type.hpp
/// @brief DB�t�B�[���h�p�̌^��`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_TYPE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_TYPE_HPP

namespace tuutil::db
{
    template <std::size_t N> struct varchar : public std::string{};
    using smallint = std::int16_t;
    using integer = std::int32_t;
    using bigint = std::int64_t;

    // char(N)�͑g�ݍ��݂��̂܂܂̂��߁A�����ł̒�`�͍s��Ȃ�

    /// varchar(max)�̏ꍇ�Ɏg�p
    /// (e.g. varchar<varchar_max>
    constexpr std::size_t varchar_max = std::string{}.max_size();

    /**
     * @fn
     * @brief �����Ώۂ̌^���varchar�y�сAchar�n�z��̃T�C�Y���擾
     * @param T �����Ώۂ̌^
    */
    template <class T> struct get_maxlength_t;
    template <std::size_t N> struct get_maxlength_t<varchar<N>> { static constexpr auto size = N; };
    template <std::size_t N, typename T> struct get_maxlength_t<T[N]> { static constexpr auto size = N; };

    /**
     * @fn
     * @brief �����Ώۂ̒l���varchar�y�сAchar�v�̔z��̃T�C�Y���擾
     * @param v �����Ώۂ̒l
    */
    template <class T> constexpr std::size_t get_maxlength(const T& v) { return get_maxlength_t<T>::size; }

    /**
     * @fn
     * @brief SQL���ʎq�Ƃ��Đ������t�H�[�}�b�g�ł��邩���؁B���s���R���p�C���G���[
     * @tparam Subject �����Ώ�
     * @details �ЂƂ܂��A�擪�����l�ȊO�̔��p�p�����A���_�[�o�[1 ~ 64���������ʎq�Ƃ��ċ���
    */
    template <str::cstr Subject>
    consteval bool validate_sql_identity()
    {
        static_assert(
            str::regex<R"(^(?!\d)\w{1,64}$)">::match(Subject),
            "Invalid SQL identity."
        );
        return true;
    }

    /**
     * @fn
     * @brief SQL�G�C���A�X���Ƃ��Đ������t�H�[�}�b�g�ł��邩���؁B���s���R���p�C���G���[
     * @tparam Subject �����Ώ�
     * @details �ЂƂ܂��A�擪�����l�ȊO�̔��p�p�����A���_�[�o�[1 ~ 255���������ʎq�Ƃ��ċ���
    */
    template <str::cstr Subject>
    consteval bool validate_sql_alias()
    {
        static_assert(
            str::regex<R"(^(?!\d)\w{1,255}$)">::match(Subject),
            "Invalid SQL alias."
        );
        return true;
    }
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_TYPE_HPP
