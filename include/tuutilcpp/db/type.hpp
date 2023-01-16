///----------------------------------
/// @file type.hpp
/// @brief DB�t�B�[���h�p�̌^��`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_TYPE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_TYPE_HPP

namespace tuutil::db
{
    /**
     * @class
     * @brief sql�̉ϒ�������^
    */
    template <std::size_t N>
    requires (N < 256)
    struct varchar : public std::string {};

    template <class T> struct is_varchar : public std::false_type {};
    template <std::size_t N> struct is_varchar<varchar<N>> : public std::true_type {};
    template <class T> constexpr auto is_varchar_v = is_varchar<T>::value;

    /**
     * @class
     * @brief sql�̌Œ蒷������^
    */
    template <std::size_t N>
    requires (N < 256)
    struct character : public str::cstr<N + 1> {};

    template <class T> struct is_character : public std::false_type {};
    template <std::size_t N> struct is_character<character<N>> : public std::true_type {};
    template <class T> constexpr auto is_character_v = is_character<T>::value;

    /**
     * @fn
     * @brief sql������^�̃T�C�Y���擾����
    */
    template <class T> struct get_sql_string_size;
    template <template <std::size_t> class Str, std::size_t N>
    struct get_sql_string_size<Str<N>> : public std::integral_constant<std::size_t, N> {};
    template <class T> constexpr auto get_sql_string_size_v = get_sql_string_size<T>::value;

    // �����^�̃G�C���A�X
    using bit = bool;
    using tinyint = std::int8_t;
    using smallint = std::int16_t;
    using integer = std::int32_t;
    using bigint = std::int64_t;
    using unsigned_tinyint = std::uint8_t;
    using unsigned_smallint = std::uint16_t;
    using unsigned_integer = std::uint32_t;
    using unsigned_bigint = std::uint64_t;

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
