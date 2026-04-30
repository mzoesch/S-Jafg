// Copyright mzoesch. All rights reserved.

#include "Storage/SaveFunctions.h"
#include "sqlite3.h"
#include "Serialization/MySqlite3.h"

#define STORAGE_NAME "sqlite3.db"

#define EMIT_ERROR_SQL(InDescription)                                                    \
    if (OutError)                                                                        \
    {                                                                                    \
        *OutError = ::Jafg::SprintF("SQL {}: [{}].", InDescription, sqlite3_errmsg(Con)); \
    }                                                                                    \
    else                                                                                 \
    {                                                                                    \
        LOG_ERROR(LogStorage, "SQL {}: [{}].", InDescription, sqlite3_errmsg(Con));      \
    }

#define EMIT_ERROR(Format, ...)                            \
    if (OutError)                                          \
    {                                                      \
        *OutError = ::Jafg::SprintF(Format __VA_OPT__(,) __VA_ARGS__); \
    }                                                      \
    else                                                   \
    {                                                      \
        LOG_ERROR(LogStorage, Format __VA_OPT__(,) __VA_ARGS__)       \
    }

namespace
{

struct LSql3Con final
{
    LSql3Con() noexcept
        : Db(nullptr)
    {
    }

    LSql3Con(const LPath& InPath, LString* OutError /* = nullptr */)
    {
        if (auto Rc{ Sqlite3Bindings::native_open((InPath / STORAGE_NAME).c_str(), &this->Db) }; Rc)
        {
            if (OutError)
            {
                *OutError = Jafg::SprintF("Failed to open database. Reason: [{}].", sqlite3_errmsg(Db));
            }
            else
            {
                LOG_ERROR(LogStorage, "Failed to open database. Reason: [{}].", sqlite3_errmsg(Db));
            }
        }

        return;
    }

    ~LSql3Con() noexcept
    {
        if (this->IsValid())
        {
            this->Close();
        }

        return;
    }

    FORCEINLINE bool IsValid() const noexcept
    {
        return this->Db != nullptr;
    }

    FORCEINLINE void Close() noexcept
    {
        check( this->IsValid() )
        sqlite3_close(this->Db);
        this->Db = nullptr;

        return;
    }

    FORCEINLINE operator        bool() const noexcept { return  this->IsValid(); }
    FORCEINLINE sqlite3*  operator *()       noexcept { return  this->Db; }
    FORCEINLINE sqlite3*  operator->()       noexcept { return  this->Db; }
    FORCEINLINE sqlite3** operator &()       noexcept { return &this->Db; }
    FORCEINLINE operator    sqlite3*()       noexcept { return  this->Db; }

    FORCEINLINE sqlite3* GetDb() noexcept { return this->Db; }

private:

    sqlite3* Db;
};

struct LSqlSmt final
{
    LSqlSmt() noexcept
        : Stmt(nullptr)
    {
    }

    ~LSqlSmt() noexcept
    {
        if (this->IsValid())
        {
            this->Close();
        }

        return;
    }

    FORCEINLINE bool IsValid() const noexcept
    {
        return this->Stmt != nullptr;
    }

    FORCEINLINE void Close() noexcept
    {
        check( this->IsValid() )
        sqlite3_finalize(this->Stmt);
        this->Stmt = nullptr;

        return;
    }

    FORCEINLINE operator             bool() const noexcept { return  this->IsValid(); }
    FORCEINLINE sqlite3_stmt*  operator *()       noexcept { return  this->Stmt; }
    FORCEINLINE sqlite3_stmt*  operator->()       noexcept { return  this->Stmt; }
    FORCEINLINE sqlite3_stmt** operator &()       noexcept { return &this->Stmt; }
    FORCEINLINE operator    sqlite3_stmt*()       noexcept { return  this->Stmt; }

    FORCEINLINE sqlite3_stmt* GetStmt() noexcept { return this->Stmt; }

private:

    sqlite3_stmt* Stmt;
};

} /* ~Anonymous Namespace */

bool Jafg::Saves::CreateNewSave(const LPath& InPath, const LMinimalMetaData& Meta, LString* OutError /* = nullptr */)
{
    if (Finder::DoesDirectoryExist(InPath))
    {
        EMIT_ERROR("Directory already exists: [{}]", InPath)
        return false;
    }

    Finder::CreateFile(InPath / STORAGE_NAME, true);

    LSql3Con Con(InPath, OutError);
    if (Con.IsValid() == false)
    {
        return false;
    }

    {
        const LString Sql
        {
            R"(CREATE TABLE Meta ()"
            R"(  DisplayName TEXT)"
            R"();)"
        };

        if (sqlite3_exec(Con, Sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            EMIT_ERROR_SQL("Creating Table")
            return false;
        }
    }

    {
        const LString Sql
        {
            Jafg::SprintF("INSERT INTO Meta VALUES ( \"{}\" );", Meta.DisplayName)
        };

        if (sqlite3_exec(Con, Sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            EMIT_ERROR_SQL("Insert values")
            return false;
        }
    }

    return true;
}

std::optional<LString> Jafg::Saves::GetDisplayName(const LPath& InPath, LString* OutError /* = nullptr */)
{
    LSql3Con Con(InPath, OutError);
    if (Con.IsValid() == false)
    {
        return { };
    }

    const LString Sql
    {
        "SELECT DisplayName FROM Meta "
        "WHERE EXISTS (SELECT 1 FROM sqlite_master WHERE type='table' AND name=?) LIMIT 1"
    };

    LSqlSmt Stmt;
    if (sqlite3_prepare_v2(Con, Sql.c_str(), -1, &Stmt, nullptr) != SQLITE_OK)
    {
        EMIT_ERROR_SQL("Prepare")
        return { };
    }

    if (sqlite3_bind_text(Stmt, 1, "Meta", -1, SQLITE_STATIC) != SQLITE_OK)
    {
        EMIT_ERROR_SQL("Bind")
        return { };
    }

    if (const int Rc = sqlite3_step(Stmt); Rc == SQLITE_ROW)
    {
        if (const unsigned char* Val = sqlite3_column_text(Stmt, 0); Val)
        {
            return { reinterpret_cast<const char*>(Val) };
        }
        EMIT_ERROR_SQL("Invalid DisplayName")
    }
    else if (Rc == SQLITE_DONE)
    {
        EMIT_ERROR_SQL("Rows Invalid")
    }
    else
    {
        EMIT_ERROR_SQL("Step")
    }

    return { };
}

#undef EMIT_ERROR_SQL
#undef STORAGE_NAME
