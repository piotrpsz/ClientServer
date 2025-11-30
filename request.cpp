//
// Created by Piotr Pszczolkowski on 28/11/2025.
//

#include "request.h"
#include "common/socket/connector.h"

using namespace bee;

/// Wysłanie żądania poprzez konektor.
/// \param conn Konektor, poprzez który wysyłane jest żądanie.
/// \return Albo odpowiedź na żądanie lub błąd errc (jeśli coś nie wyszło).
Result<Response,std::errc> Request::write(Connector const& conn) const noexcept {
    std::println("Request::write");
    std::println(" - request: {}", *this);

    if (auto json = toJSON()) {
        if (auto const stat = conn.write(std::move(json.value())); not stat)
            return Failure(stat.error());
        auto const answer = conn.read();
        if (not answer)
            return Failure(answer.error());
        if (auto const response = Response::fromJSON(answer.value())) {
            std::println(" - response: {}", response.value());
            return response.value();
        }
    }
    return Failure(std::errc::bad_message);
}

Result<Request,std::errc> Request::read(Connector const& conn) noexcept {
    std::println("Request::read");

    auto const data = conn.read();
    if (not data)
        return Failure(data.error());

    auto request = Request::fromJSON(data.value());
    if (not request)
        return Failure(std::errc::bad_message);

    std::println(" - request: {}", request.value());
    return request.value();
}
