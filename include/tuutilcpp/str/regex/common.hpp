///----------------------------------
/// @file regex/common.hpp 
/// @brief regex.hppの関連
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_COMMON_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_COMMON_HPP

namespace tuutil::str
{
    struct regex_char_attribute
    {
        enum {
            QUANTIFIER      = 0x0001u,
            ANCHOR          = 0x0002u,
            CLASS           = 0x0004u,
            CAPTURE         = 0x0008u,
            OR_MATCH        = 0x0010u,
            CHARSET         = 0x0020u,
            CHARSET_INNER   = 0x0040u,
            REFERENCE       = 0x0080u,
            DENY            = 0x0100u,
            BK              = 0x0200u,
            OTHERS          = 0x0400u,
        };

        // 数量子の起点
        static constexpr auto quantifier_chars      = cstr{"*+?{"};
        // アンカーの起点
        static constexpr auto anchor_chars          = cstr{"^$("};
        static constexpr auto bk_anchor_chars       = cstr{"bB"};
        // 文字クラス
        static constexpr auto class_chars           = cstr{"."};
        static constexpr auto bk_class_chars        = cstr{"dDwWsStrnvf0"};
        // (キャプチャ/非キャプチャ)グループ
        static constexpr auto capture_chars         = cstr{"("};
        // or
        static constexpr auto or_match_chars        = cstr{"|"};
        // 文字集合
        static constexpr auto char_set              = cstr{"["};
        static constexpr auto char_set_inner        = cstr{"^-"};
        static constexpr auto bk_char_set_inner     = cstr{"b"};
        // キャプチャグループ参照
        static constexpr auto bk_reference_chars    = cstr{"123456789k"};
        // 単体で出てきてはいけないもの
        static constexpr auto deny_chars            = cstr{"})]"};
        // 上記全てを含む
        static constexpr auto special_chars = quantifier_chars
            + anchor_chars
            + bk_anchor_chars
            + class_chars
            + bk_class_chars
            + capture_chars
            + or_match_chars
            + char_set
            + char_set_inner
            + bk_char_set_inner
            + bk_reference_chars
            + deny_chars;

        // 属性リスト
        static constexpr auto attributes = []() {
            std::uint32_t attrs[(std::numeric_limits<char>::max)() + 1] = {};
            for (const auto c : quantifier_chars.view())    attrs[c] |= QUANTIFIER;
            for (const auto c : anchor_chars.view())        attrs[c] |= ANCHOR;
            for (const auto c : bk_anchor_chars.view())     attrs[c] |= ANCHOR          | BK;
            for (const auto c : class_chars.view())         attrs[c] |= CLASS;
            for (const auto c : bk_class_chars.view())      attrs[c] |= CLASS           | BK;
            for (const auto c : capture_chars.view())       attrs[c] |= CAPTURE;
            for (const auto c : or_match_chars.view())      attrs[c] |= OR_MATCH;
            for (const auto c : char_set.view())            attrs[c] |= CHARSET;
            for (const auto c : char_set_inner.view())      attrs[c] |= CHARSET_INNER;
            for (const auto c : bk_char_set_inner.view())   attrs[c] |= CHARSET_INNER   | BK;
            for (const auto c : bk_reference_chars.view())  attrs[c] |= REFERENCE       | BK;
            for (const auto c : deny_chars.view())          attrs[c] |= DENY;
            attrs['\\'] |= OTHERS;  // 一文字しか該当しないため、直接比較すればよいが、一括で特殊文字判定する時のため、属性を指定しておく
            return std::to_array(attrs);
        }();

        /**
         * @fn
         * @brief Flagsに指定した全ての属性を合わせたビットがcに設定されている属性でも全て立っている場合のみ真
        */
        template <std::size_t... Flags>
        static constexpr bool check_attrs_conjuction(char c)
        {
            constexpr auto attr = (Flags | ...);
            return (attributes[c] & attr) == attr;
        }

        /**
         * @fn
         * @brief Flagsに指定したビットのうちいずれかの属性が一つでもcに設定されていれば真
        */
        template <std::size_t... Flags>
        static constexpr bool check_attrs_disjuction(char c)
        {
            constexpr auto attr = (Flags | ...);
            return bool(attributes[c] & attr);
        }
    };

    /**
     * @fn
     * @brief 正しくない書き方がされているバックスラッシュが存在する場合偽が返る
    */
    inline constexpr bool is_collect_regex_back_slash(std::string_view target)
    {
        for (int i = 0; i < target.size(); i++)
            if (target[i] == '\\' && (i == target.size() - 1 || !regex_char_attribute::attributes[target[++i]]))
                return false;
        return true;
    }

    /**
     * @fn
     * @brief indexの位置の一つ前の文字が指定の文字か確認する
    */
    inline constexpr bool eq_before_char(std::string_view target, int index, char ch)
    {
        return index > 0 && target[(std::max)(index - 1, 0)] == ch;
    }
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_COMMON_HPP
