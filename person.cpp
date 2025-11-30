//
// Created by Piotr Pszczolkowski on 08/11/2025.
//

#include "person.h"
#include <iostream>
#include "shared4cx/shared.h"
#include "sqlite4cx/sqlite4cx.hpp"

namespace bee {

    Person::Person(Row &&row) noexcept {
        if (auto const f = row["first_name"])
            first_name_ = f.value().get<String>();
        if (auto f= row["middle_name"])
            middle_name_ = f.value().get_if<String>();
        if (auto const f = row["last_name"])
            last_name_ = f.value().get<String>();
        if (auto const f = row["age"])
            age_ = f.value().get<u32>();
        if (auto const i = row["id"])
            id_ = i.value().get<i64>();
    }

    bool Person::insert() noexcept {
        auto query = Query("INSERT INTO person (first_name, middle_name, last_name, age) VALUES (?, ?, ?, ?)");
        query
            .add(first_name_)
            .add(middle_name_)
            .add(last_name_)
            .add(age_);

        auto const retv = Database::self().insert(std::move(query));
        if (retv) {
            id_ = retv.value();
            return true;
        }
        std::println("{}", retv.error());
        return false;
    }

    bool Person::update() const noexcept {
        Query query("UPDATE person SET first_name=?, middle_name=?, last_name=?, age=? WHERE id=?"
            , first_name_
            , middle_name_
            , last_name_
            , age_
            , id_
        );
        auto const retv = Database::self().update(std::move(query));
        if (retv)
            return true;

        std::println("{}", retv.error());
        return false;
    }

    Option<Person> Person::with_id(i64 id) noexcept {
        Query query("SELECT * FROM person WHERE id=?", id);
        if (auto retv = Database::self().select(std::move(query))) {
            for (auto&& row : retv.value()) {
                return Person{std::move(row)};
            }
        }

        return {};
    }

    Option<Vector<Person>> Person::all() noexcept {
        Query query("SELECT * FROM person");
        auto retv = Database::self().select(std::move(query));
        if (!retv) {
            std::println(std::cerr, "{}", retv.error());
            return {};
        }

        Vector<Person> persons;
        persons.reserve(retv.value().size());
        for (auto&& row : retv.value())
            persons.emplace_back(std::move(row));
        return persons;
    }

    Option<Vector<Person>> Person::result_of(Query&& query) noexcept {
        auto retv = Database::self().select(std::move(query));
        if (!retv) {
            std::println(std::cerr, "{}", retv.error());
            return {};
        }
        Vector<Person> persons;
        persons.reserve(retv.value().size());
        for (auto&& row : retv.value())
            persons.emplace_back(std::move(row));
        return persons;
    }

    bool Person::remove(i64 const id) noexcept {
        auto retval = Database::self().exec("DELETE FROM person WHERE id=?", id);
        if (retval) return true;
        std::println("{}", retval.error());
        return false;
    }

    bool Person::remove(Vector<i64>&& vec_i64) noexcept {
        Vector<String> vec_str;
        vec_str.reserve(vec_i64.size());
        for (auto const id : vec_i64)
            vec_str.push_back(std::to_string(id));
        auto ids = join(vec_str, ',');

        Query query(std::format("DELETE FROM person WHERE id IN ({})", ids));
        auto retval = Database::self().exec(std::move(query));
        if (retval) return true;
        std::println("{}", retval.error());
        return false;
    }

}
