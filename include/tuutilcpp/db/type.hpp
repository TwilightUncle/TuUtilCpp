///----------------------------------
/// @file type.hpp
/// @brief DBフィールド用の型定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_TYPE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_TYPE_HPP

namespace tuutil::db
{
    template <std::size_t N> struct varchar : public std::string{};
    using smallint = std::int16_t;
    using integer = std::int32_t;
    using bigint = std::int64_t;

    // char(N)は組み込みそのままのため、ここでの定義は行わない

    /// varchar(max)の場合に使用
    /// (e.g. varchar<varchar_max>
    constexpr std::size_t varchar_max = std::string{}.max_size();

    /**
     * @fn
     * @brief 検査対象の型よりvarchar及び、char系配列のサイズを取得
     * @param T 検査対象の型
    */
    template <class T> struct get_maxlength_t;
    template <std::size_t N> struct get_maxlength_t<varchar<N>> { static constexpr auto size = N; };
    template <std::size_t N, typename T> struct get_maxlength_t<T[N]> { static constexpr auto size = N; };

    /**
     * @fn
     * @brief 検査対象の値よりvarchar及び、char計の配列のサイズを取得
     * @param v 検査対象の値
    */
    template <class T> constexpr std::size_t get_maxlength(const T& v) { return get_maxlength_t<T>::size; }

    /**
     * @fn
     * @brief SQL識別子として正しいフォーマットであるか検証。失敗時コンパイルエラー
     * @tparam Subject 検査対象
     * @details ひとまず、先頭が数値以外の半角英数字アンダーバー1 ~ 64文字を識別子として許可
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
     * @brief SQLエイリアス名として正しいフォーマットであるか検証。失敗時コンパイルエラー
     * @tparam Subject 検査対象
     * @details ひとまず、先頭が数値以外の半角英数字アンダーバー1 ~ 255文字を識別子として許可
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
