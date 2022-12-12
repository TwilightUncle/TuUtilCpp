///----------------------------------
/// @file regex/common.hpp 
/// @brief regex.hppの関連
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_COMMON_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_COMMON_HPP

namespace tustr
{
    enum {
        // 通常範囲に置ける特殊文字の性質
        REGEX_SPECIAL       = 0x0001u,  // 特殊文字(バックスラッシュでエスケープ)
        REGEX_BK_SPECIAL     = 0x0002u,  // 特殊文字(バックスラッシュを付与した場合)
        REGEX_BRANCKET_BE   = 0x0004u,  // 括弧始まり
        REGEX_BRANCKET_EN   = 0x0008u,  // 括弧終わり
        REGEX_PREV_REF      = 0x0010u,  // 前の文字を参照する
        REGEX_NEXT_REF      = 0x0020u,  // 次の文字を参照する
        REGEX_BEGIN         = 0x0040u,  // 先頭にしか指定できない
        REGEX_END           = 0x0080u,  // 末尾にしか指定できない

        // []内における特殊文字の性質
        // REGEX_CLASSES_SPECIAL以外にビットがったていなければバックスラッシュでエスケープが必要、
        // REGEX_CLASSES_SPECIAL以外にビットが立っている場合、該当する全てを満たしていない場合では特殊文字として認識してはいけない
        REGEX_CLASSES_SPECIAL       = 0x0100u,  // 特殊文字([]内における)
        REGEX_CLASSES_BK_SPECIAL    = 0x0200u,
        REGEX_CLASSES_PREV_REF      = 0x0400u,
        REGEX_CLASSES_NEXT_REF      = 0x0800u,
        REGEX_CLASSES_BEGIN         = 0x1000u,  // 先頭
        REGEX_CLASSES_END           = 0x2000u,

        // 特殊文字かどうかの判定用マスク
        REGEX_SPECIAL_MASK = REGEX_SPECIAL | REGEX_BK_SPECIAL | REGEX_CLASSES_SPECIAL | REGEX_CLASSES_BK_SPECIAL,
    };

    /**
     * @fn
     * @brief 指定した文字の正規表現としての文法的な性質を示す属性のビット列を取得する
     * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions/Cheatsheet
    */
    inline constexpr auto get_regex_character_attribute(char ch)
    {
        std::uint32_t attrs[(std::numeric_limits<char>::max)() + 1] = {};
        // Character classes
        attrs['['] |= REGEX_SPECIAL | REGEX_BRANCKET_BE | REGEX_CLASSES_SPECIAL;
        attrs[']'] |= REGEX_SPECIAL | REGEX_BRANCKET_EN | REGEX_CLASSES_SPECIAL;
        attrs['-'] |= REGEX_CLASSES_SPECIAL | REGEX_CLASSES_PREV_REF | REGEX_CLASSES_NEXT_REF;
        attrs['^'] |= REGEX_CLASSES_SPECIAL | REGEX_CLASSES_BEGIN;
        attrs['.'] |= REGEX_SPECIAL;
        attrs['d'] |= REGEX_BK_SPECIAL;
        attrs['D'] |= REGEX_BK_SPECIAL;
        attrs['w'] |= REGEX_BK_SPECIAL;
        attrs['W'] |= REGEX_BK_SPECIAL;
        attrs['s'] |= REGEX_BK_SPECIAL;
        attrs['S'] |= REGEX_BK_SPECIAL;
        attrs['t'] |= REGEX_BK_SPECIAL;
        attrs['r'] |= REGEX_BK_SPECIAL;
        attrs['n'] |= REGEX_BK_SPECIAL;
        attrs['v'] |= REGEX_BK_SPECIAL;
        attrs['f'] |= REGEX_BK_SPECIAL;
        attrs['b'] |= REGEX_CLASSES_BK_SPECIAL;
        attrs['0'] |= REGEX_BK_SPECIAL;
        attrs['c'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['x'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['u'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['\\'] |= REGEX_SPECIAL | REGEX_NEXT_REF | REGEX_CLASSES_SPECIAL | REGEX_CLASSES_NEXT_REF;
        attrs['|'] |= REGEX_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        // Assertions
        attrs['^'] |= REGEX_SPECIAL | REGEX_BEGIN;
        attrs['$'] |= REGEX_SPECIAL | REGEX_END;
        attrs['b'] |= REGEX_BK_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        attrs['B'] |= REGEX_BK_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        attrs['?'] |= REGEX_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        // Groups and backreferences
        attrs['('] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        attrs[')'] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        for (const auto c : std::views::iota('1', '9'))
            attrs[c] |= REGEX_BK_SPECIAL;
        attrs['k'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        // Quantifiers
        attrs['*'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['+'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['?'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['{'] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        attrs['}'] |= REGEX_SPECIAL | REGEX_BRANCKET_EN;
        // Unicode property escapes
        attrs['p'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['P'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;

        return attrs[ch];
    }

    struct regex_char_attribute
    {
        // 数量子の起点
        static constexpr auto quantifier_chars = cstr{"*+?{"};

        // アンカーの起点
        static constexpr auto anchor_chars = cstr{"^$("};
        static constexpr auto bk_anchor_chars = cstr{"bB"};

        // 文字クラス
        static constexpr auto class_chars = cstr{"."};
        static constexpr auto bk_class_chars = cstr{"dDwWsStrnvf0\\"};

        // (キャプチャ/非キャプチャ)グループ
        static constexpr auto capture_chars = cstr{"("};

        // or
        static constexpr auto or_match_chars = cstr{"|"};

        // 文字集合
        static constexpr auto char_set = cstr{"["};

        // キャプチャグループ参照
        static constexpr auto bk_reference_chars = cstr{"123456789"};
    };

    /**
     * @fn
     * @brief 正しくない書き方がされているバックスラッシュが存在する場合偽が返る
    */
    inline constexpr bool is_collect_regex_back_slash(const std::string_view& target)
    {
        for (int i = 0; i < target.size(); i++)
            if (target[i] == '\\' && (i == target.size() - 1 || !get_regex_character_attribute(target[++i])))
                return false;
        return true;
    }

    /**
     * @fn
     * @brief 許可リストまたは拒否リストとして指定したchar_listを参照し、targetが有効か検証する。バックスラッシュを含む場合、二つで一文字と認識する
     * @param target 検証する文字列
     * @param allow_or_deny allowの場合trueを指定、denyの場合falseを指定
     * @param char_list 許可または拒否文字のリスト。allow_or_denyによって拒否リストか許可リストか制御する
    */
    inline constexpr bool is_allowed_string(
        const std::string& target,
        bool allow_or_deny,
        const std::string& char_list,
        const std::string& bk_char_list,
        bool is_bk_escape = true
    ) {
        const auto collate_list = [&allow_or_deny](const std::string& list, char ch)->bool {
            // 許可リストの中から未発見のものがあった場合または、
            // 拒否リストの中から発見されたものがあった場合、失敗
            return (list.find_first_of(ch) == std::string::npos) == allow_or_deny;
        };

        for (int i = 0; i < target.size(); i++) {
            if ((target[i] != '\\' || !is_bk_escape) && collate_list(char_list, target[i])) return false;
            else if (target[i] == '\\' && i < target.size() - 1 && collate_list(bk_char_list, target[++i])) return false;
        }
        return true;
    }

    /**
     * @fn
     * @brief indexの位置の一つ前の文字が指定の文字か確認する
    */
    inline constexpr bool eq_before_char(const std::string_view& target, int index, char ch)
    {
        return index > 0 && target[(std::max)(index - 1, 0)] == ch;
    }
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_COMMON_HPP
