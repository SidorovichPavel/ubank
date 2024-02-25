#pragma once

#include <optional>
#include <string>

#include <boost/uuid/uuid.hpp>

#include <userver/utils/datetime/date.hpp>

namespace ubank {

enum class Currency : std::int16_t;

struct DepositInfo {
  boost::uuids::uuid id;         // string
  std::string type;              // maybe will change to enum
  std::string agreement_number;  // maybe will change to int
  Currency currency;
  userver::utils::datetime::Date begin;
  userver::utils::datetime::Date end;
  userver::utils::datetime::Date agreement_begin;
  userver::utils::datetime::Date agreement_end;
  std::int64_t amount;
  std::int16_t interest;
  std::int32_t client_id;  // will be changed to string/uuid
};

struct Account {
  std::int64_t number;   // maybe need custom type?
  std::int16_t code;     // maybe need enum
  std::string activity;  // maybe need enum
  std::int64_t debit;
  std::int64_t credit;
  std::int64_t balance;
  std::optional<std::string> name;
  boost::uuids::uuid deposit_id;
};

}  // namespace ubank