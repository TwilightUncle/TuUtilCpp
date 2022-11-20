///----------------------------------
/// @file utility.hpp
/// @brief 二項の引数を受け取る処理に対して繰り上げ処理を適用できるようなコンテナ
///----------------------------------
#pragma once
#include <array>

namespace tudb
{
    /**
     * @brief 二引数を渡して実行した結果、次のフォーマットで結果が返る -> pair{array{(T)桁上がりしていない部分, (T)桁上がりした部分}, (bool)桁上がりしたかどうか}
    */
    template <class F, class T>
    concept CarryOverCallable = std::is_invocable_r_v<std::pair<std::array<T, 2>, bool>, F, T, T>;

    template <class T, std::size_t Size>
    struct carry_over_container : std::array<T, Size>
    {
        /**
         * @fn
         * @brief 同じ型が渡されたとき
         * @param f 目的の処理
         * @param arg 引数
         * @param assign_offset 代入先のインデックスにずれがあるとき指定
        */
        template <std::size_t N>
        constexpr carry_over_container run(CarryOverCallable<T> auto f, const carry_over_container<T, N>& arg, const unsigned int assign_offset = 0) const
        {
            return run(f, arg, f, assign_offset);
        }

        /**
         * @fn
         * @brief 同じ型が渡されたとき
         * @param f 目的の処理
         * @param arg 引数
         * @param carry_over_process 桁上がりに対して実施される処理
         * @param assign_offset 代入先のインデックスにずれがあるとき指定
        */
        template <std::size_t N>
        constexpr carry_over_container run(
            CarryOverCallable<T> auto f,
            const carry_over_container<T, N>& arg,
            CarryOverCallable<T> auto carry_over_process,
            const unsigned int assign_offset = 0
        ) const {
            auto result = (*this);
            for (unsigned int i = 0; i < (std::min)(Size, N); i++)
                result = result.run(f, arg[i], carry_over_process, i, assign_offset);
            return result;
        }

        /**
         * @fn
         * @brief 桁上がりの計算をbufに適用する(桁上がりの計算が、最初と同じ場合 ※足し算のひっ算みたいな)
         * @param f 目的の処理
         * @param arg 引数
         * @param offset 計算対象のインデックスを指定
         * @param assign_offset 代入先のインデックスにずれがあるとき指定
        */
        constexpr carry_over_container run(CarryOverCallable<T> auto f, const T& arg, unsigned int offset = 0, const unsigned int assign_offset = 0) const
        {
            return run(f, arg, f, offset, assign_offset);
        }

        /**
         * @fn
         * @brief 桁上がりの計算をbufに適用する(桁上がりの計算が、最初と異なる場合 ※掛け算のひっ算みたいな)
         * @param f 目的の処理
         * @param arg 引数
         * @param carry_over_process 桁上がりに対して実施される処理
         * @param offset 計算対象のインデックスを指定
         * @param assign_offset 代入先のインデックスにずれがあるとき指定
        */
        constexpr carry_over_container run(
            CarryOverCallable<T> auto f,
            const T& arg,
            CarryOverCallable<T> auto carry_over_process,
            const unsigned int offset = 0,
            const unsigned int assign_offset = 0
        ) const {
            auto result = (*this);
            if (offset < Size) {
                if (assign_offset > 0)
                    return run(carry_over_process, arg, offset + assign_offset);

                auto calclated = f(this->data()[offset], arg);
                result[offset] = calclated.first[0];

                for (unsigned int i = offset + 1; i < Size && calclated.second; i++) {
                    calclated = carry_over_process(this->data()[i], calclated.first[1]);
                    result[i] = calclated.first[0];
                }
            }
            return result;
        }

        /**
         * @fn
         * @brief 自身と同じ型のオブジェクトによるたすき掛け
        */
        template <std::size_t N>
        constexpr carry_over_container run_all(CarryOverCallable<T> auto f, const carry_over_container<T, N>& arg) const
        {
            return run_all(f, arg, f);
        }

        /**
         * @fn
         * @brief 自身と同じ型のオブジェクトによるたすき掛け
        */
        template <std::size_t N>
        constexpr carry_over_container run_all(
            CarryOverCallable<T> auto f,
            const carry_over_container<T, N>& arg,
            CarryOverCallable<T> auto carry_over_process
        ) const {
            auto result = carry_over_container{};
            for (unsigned int i = 0; i < (std::min)(Size, N); i++)
                result = result.run(
                    carry_over_process,
                    run_all(f, arg[i], carry_over_process, i)
                );
            return result;
        }

        /**
         * @fn
         * @brief 引数をすべての要素に適用する
        */
        constexpr carry_over_container run_all(CarryOverCallable<T> auto f, const T& arg, const unsigned int assign_offset = 0) const
        {
            return run_all(f, arg, f, assign_offset);
        }

        /**
         * @fn
         * @brief 引数をすべての要素に適用する
        */
        constexpr carry_over_container run_all(
            CarryOverCallable<T> auto f,
            const T& arg,
            CarryOverCallable<T> auto carry_over_process,
            const unsigned int assign_offset = 0
        ) const {
            auto result = carry_over_container{};
            for (unsigned int i = 0; i < Size; i++) {
                const auto calclated = f(this->data()[i], arg);
                result = result.run(
                    carry_over_process,
                    carry_over_container<T, 2>{calclated.first[0], calclated.first[1]},
                    i + assign_offset
                );
            }
            return result;
        }
    };
}
