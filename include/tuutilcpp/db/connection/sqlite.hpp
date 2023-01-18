///----------------------------------
/// @file db/query/sqlite.hpp
/// @brief sqliteクエリ生成
/// @details 本ファイルを使用する場合、sqlite3.hがインクルード可能で、sqlite3.libがリンク済みでない場合、未定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP

#if __has_include(<sqlite3.h>)  // sqlite3のヘッダが読み込めなければいけない
#include <sqlite3.h>
#include <tuutilcpp/str.hpp>

namespace tuutil::db::connection
{
    /**
     * @class
     * @brief sqlite3のラッパー
     * @tparam DbFileName
    */
    template <str::cstr DbFileName>
    class sqlite
    {
    public:
        using sqlite_callback_t = int (*)(void*, int, char**, char**);

        sqlite() { this->open(); }

        ~sqlite()
        {
            // sqlステートメントの終了処理も書く必要あり
            sqlite3_close(this->con);
        }

        /**
         * @fn
         * @brief テンプレート引数で指定されたDBファイルが存在委するか判定
        */
        bool exists() const { return std::ifstream(DbFileName).is_open(); }

        /**
         * @fn
         * @brief 文字列として構築されたsqlクエリを実行
         * @tparam Func nullptr or ラムダ式 or 関数ポインタ
         * @param sql クエリ文字列
         * @param data [out] データ出力バッファ
         * @param errmsg [out] エラー取得バッファ
        */
        template <auto Func>
        void exec(const std::string& sql, void* data, char** errmsg = nullptr) const
        {
            this->error(
                sqlite3_exec(this->con, sql.c_str(), nullptr, data, nullptr)
            );
        }
        template <auto Func>
        requires (std::invocable<decltype(Func) ,void*, int, char**, char**>)
        void exec(const std::string& sql, void* data, char** errmsg = nullptr) const
        {
            this->error(
                sqlite3_exec(this->con, sql.c_str(), &exec_callback_template<Func>, data, nullptr)
            );
        }

    private:
        sqlite3* con = nullptr;

        /**
         * @fn
         * @brief DBとの接続を開く
        */
        void open()
        {
            this->error(
                sqlite3_open_v2(std::string(DbFileName).c_str(), &this->con, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr)
            );
        }

        /**
         * @fn
         * @brief 例外送出
        */
        void error(int code) const
        {
            if (code != SQLITE_OK)
                throw std::runtime_error(
                    std::string("[sqlite3 error]code ")
                    + std::to_string(code) + ", "
                    + '"' + sqlite3_errmsg(this->con) + '"'
                );
        }

        /**
         * @fn
         * @brief コールバック関数の型特性吸収テンプレート
         * @tparam Func ラムダ式、関数ポインタ等
         * @param data データ出力バッファ
         * @param argc selectした場合列数
         * @param argv selectした場合一行の結果の値配列
         * @param asColName selectした場合カラム名の配列
         * 
        */
        template <std::invocable<void*, int, char**, char**> auto Func>
        static int exec_callback_template(void* data, int argc, char **argv, char **azColName)
        {
            Func(data, argc, argv, azColName);
            return SQLITE_OK;
        }
    };
}

#else
# warning <sqlite3.h> is not found
#endif  // __has_include(<sqlite3.h>)

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP
