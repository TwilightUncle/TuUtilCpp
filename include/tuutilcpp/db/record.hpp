///----------------------------------
/// @file record.hpp
/// @brief DBデータの1行あたりの格納用オブジェクト
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_RECORD_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_RECORD_HPP

namespace tuutil::db
{
    /**
     * @class
     * @brief 結果を格納したり、insert, updateで渡したりする
     * @tparam ColumnDefinitions 該当のカラム定義
    */
    template <ColumnDefinable... ColumnDefinitions>
    struct record
        : public std::tuple<typename ColumnDefinitions::field_type...>
    {
    private:
        using parent = std::tuple<typename ColumnDefinitions::field_type...>;
    
    public:
        using parent::parent;
    };

    /**
     * @fn
     * @brief recordのColIDに該当する要素を取得する
     * @tparam ColID カラム識別用の列挙型の値。これを参照して値を代入する要素を決定する
     * @param re 対象レコード
    */
    template <mpl::Enumeration auto ColID, ColumnDefinable... ColumnDefinitions>
    requires (!std::same_as<get_column_def_t<ColID, mpl::type_list<ColumnDefinitions...>>, mpl::ignore_type>)
    constexpr auto& get(record<ColumnDefinitions...>& re)
    {
        using column_def_type = get_column_def<ColID, mpl::type_list<ColumnDefinitions...>>;
        return std::get<column_def_type::value>(re);
    }

    /**
     * @fn
     * @brief recordのインスタンスに値をセットする
     * @tparam ColID カラム識別用の列挙型の値。これを参照して値を代入する要素を決定する
     * @param re 値をセットする対象
     * @param value セットする値
    */
    template <mpl::Enumeration auto ColID, ColumnDefinable... ColumnDefinitions>
    constexpr void set(
        record<ColumnDefinitions...>& re,
        const typename get_column_def_t<ColID, mpl::type_list<ColumnDefinitions...>>::field_type& value
    ) {
        get<ColID>(re) = value;
    }
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_RECORD_HPP
