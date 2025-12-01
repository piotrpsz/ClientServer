// MIT License
//
// Copyright (c) 2024 Piotr Pszczółkowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Piotr Pszczółkowski
// Date: 28.11.2025
// E-mail: piotr@beesoft.pl.

/*------- include files:
-------------------------------------------------------------------*/
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
        std::println(std::cerr, "{}", retv.error());
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

        std::println(std::cerr, "{}", retv.error());
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
        auto retv = Database::self().select(Query{"SELECT * FROM person"});
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

        persons.shrink_to_fit();
        return persons;
    }

    bool Person::remove(i64 const id) noexcept {
        auto retval = Database::self().exec("DELETE FROM person WHERE id=?", id);
        if (retval)
            return true;

        std::println(std::cerr, "{}", retval.error());
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
        if (retval)
            return true;

        std::println(std::cerr, "{}", retval.error());
        return false;
    }

}
