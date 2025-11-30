//
// Created by Piotr Pszczolkowski on 08/11/2025.
//

#pragma once
#include "shared4cx/types.h"
#include "shared4cx/datime.h"
#include "sqlite4cx/engine/database.h"
#include <format>

namespace bee {
    class Person {
        i64 id_{};
        String first_name_{};
        Option<String> middle_name_{};
        String last_name_{};
        u32 age_{};
        Datime created_{};
        Datime updated_{};

    public:
        Person() = default;
        ~Person() = default;
        Person(Person const&) = default;
        Person& operator=(Person const&) = default;
        Person(Person&&) = default;
        Person& operator=(Person&&) = default;

        [[nodiscard]] i64 id() const noexcept { return id_; }
        [[nodiscard]] String first_name() const noexcept { return first_name_; }
        [[nodiscard]] String middle_name() const noexcept { return middle_name_ ? middle_name_.value() : "NULL"; }
        [[nodiscard]] String last_name() const noexcept { return last_name_; }
        [[nodiscard]] u32 age() const noexcept { return age_; }
        [[nodiscard]] Datime created() const noexcept { return created_; }
        [[nodiscard]] Datime updated() const noexcept { return updated_; }
        explicit Person(Row&& row) noexcept;

        Person& first_name(String text) noexcept {
            first_name_ = std::move(text);
            return *this;
        }
        Person& middle_name(Option<String> text = std::nullopt) noexcept {
            middle_name_ = std::move(text);
            return *this;
        }
        Person& last_name(String text) noexcept {
            last_name_ = std::move(text);
            return *this;
        }
        Person& age(u32 age) noexcept {
            age_ = age;
            return *this;
        }

        bool insert() noexcept;
        [[nodiscard]] bool update() const noexcept;
        bool save() noexcept {
            return id_ ? update() : insert();
        }

        static Option<Person> with_id(i64 id) noexcept;
        static Option<Vector<Person>> all() noexcept;
        static Option<Vector<Person>> result_of(Query&& query) noexcept;
        static bool remove(i64 id) noexcept;
        static bool remove(Vector<i64>&& vec_i64) noexcept;
    public:
        static inline Vector<String> const CreationCmd = {
            {
                R"(
                    CREATE TABLE person (
                        id INTEGER PRIMARY KEY AUTOINCREMENT,
                        first_name TEXT NOT NULL COLLATE NOCASE,
                        middle_name TEXT COLLATE NOCASE,
                        last_name TEXT NOT NULL COLLATE NOCASE,
                        age INTEGER,
                        created DATETIME,
                        updated DATETIME
                    );
                )"
            },
            {
                R"(
                    CREATE UNIQUE INDEX person_fist_last_index ON person(first_name, last_name);
                )"
            },
           {
                R"(
                    CREATE TRIGGER insert_person AFTER INSERT ON person
                    BEGIN
                        UPDATE person
                        SET created = DATETIME('NOW', 'localtime')
                        WHERE ROWID = new.ROWID;
                    END;
                )"
           },
           {
                R"(
                    CREATE TRIGGER update_person AFTER UPDATE ON person
                    BEGIN
                        UPDATE person
                        SET updated = DATETIME('NOW', 'localtime')
                        WHERE ROWID = new.ROWID;
                    END;
                )"
            },
        };
    };
}

/// Funkcja formatująca obiekt (jako string).
template<>
struct std::formatter<bee::Person> : std::formatter<std::string> {
    auto format(bee::Person const& p, std::format_context& ctx) const {
        return formatter<std::string>::format(std::format("Person{{ ID: {}, first: {}, middle: {}, last: {}, age: {}, created: {}, updated: {} }}",
            p.id(),
            p.first_name(),
            p.middle_name(),
            p.last_name(),
            p.age(),
            p.created(),
            p.updated()
            ), ctx);
    }
};