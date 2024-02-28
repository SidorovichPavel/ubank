#pragma once

#include <chrono>
#include <optional>
#include <string>

#include <boost/uuid/uuid.hpp>

#include <userver/formats/json.hpp>
#include <userver/utils/datetime/date.hpp>
#include <userver/storages/postgres/database.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace ubank {

struct Deposit {
  boost::uuids::uuid id;         // string
  std::string category;          // maybe will change to enum
  std::string agreement_number;  // maybe will change to intka
  userver::utils::datetime::Date program_begin;
  userver::utils::datetime::Date program_end;
  userver::utils::datetime::Date agreement_begin;
  userver::utils::datetime::Date agreement_end;
  std::int64_t amount;
  std::int16_t interest;
  boost::uuids::uuid id_clients;  // will be changed to string/uuid
  boost::uuids::uuid id_main_accounts;
  boost::uuids::uuid id_sec_accounts;
};

userver::formats::json::Value Serialize(
    const Deposit& deposit,
    userver::formats::serialize::To<userver::formats::json::Value>);

Deposit DeserializeDeposit(const userver::formats::json::Value& json);
Deposit DeserializeDeposit(const userver::storages::postgres::Row& row);

struct Account {
  boost::uuids::uuid id;   // string
  std::int64_t id_number;  // maybe need custom type?
  std::int16_t code;       // maybe need enum
  std::string activity;    // maybe need enum
  std::int64_t debit;
  std::int64_t credit;
  std::int64_t balance;
  std::optional<std::string> note;
};

userver::formats::json::Value Serialize(
    const Account& account,
    userver::formats::serialize::To<userver::formats::json::Value>);

Account DeserializeAccount(const userver::storages::postgres::Row& row);

struct Transaction {
  boost::uuids::uuid id;
  boost::uuids::uuid src_account;
  boost::uuids::uuid dst_account;
  std::int64_t amount;
  userver::utils::datetime::Date txn_date;
};

userver::formats::json::Value Serialize(
    const Transaction& tnx,
    userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace ubank