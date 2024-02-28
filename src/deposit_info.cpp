#include "deposit_info.hpp"

#include <boost/uuid/uuid_generators.hpp>

#include <userver/formats/json.hpp>
#include <userver/formats/serialize/boost_uuid.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/storages/postgres/io/date.hpp>
#include <userver/storages/postgres/io/optional.hpp>
#include <userver/storages/postgres/io/uuid.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/datetime/date.hpp>

namespace ubank {

userver::formats::json::Value Serialize(
    const Deposit& deposit,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder json_builder;
  json_builder["id"] = deposit.id;
  json_builder["category"] = deposit.category;
  json_builder["agreement_number"] = deposit.agreement_number;
  json_builder["agreement_begin"] = deposit.agreement_begin;
  json_builder["agreement_end"] = deposit.agreement_end;
  json_builder["program_begin"] = deposit.program_begin;
  json_builder["program_end"] = deposit.program_end;
  json_builder["amount"] = deposit.amount;
  json_builder["interest"] = deposit.interest;
  json_builder["id_main_accounts"] = deposit.id_main_accounts;
  json_builder["id_secondary_accounts"] = deposit.id_sec_accounts;
  json_builder["id_clients"] = deposit.id_clients;
  return json_builder.ExtractValue();
}

Deposit DeserializeDeposit(const userver::formats::json::Value& json) {
  return Deposit{boost::uuids::nil_generator{}(),
                 json["category"].As<std::string>(),
                 json["agreement_number"].As<std::string>(),
                 json["program_begin"].As<userver::utils::datetime::Date>(),
                 json["program_end"].As<userver::utils::datetime::Date>(),
                 json["agreement_begin"].As<userver::utils::datetime::Date>(),
                 json["agreement_end"].As<userver::utils::datetime::Date>(),
                 json["amount"].As<std::int64_t>(),
                 json["interest"].As<std::int16_t>(),
                 boost::uuids::nil_generator{}(),
                 boost::uuids::nil_generator{}(),
                 boost::uuids::nil_generator{}()};
}

Deposit DeserializeDeposit(const userver::storages::postgres::Row& row) {
  return Deposit{row["id"].As<boost::uuids::uuid>(),
                 row["category"].As<std::string>(),
                 row["agreement_number"].As<std::string>(),
                 row["program_begin"].As<userver::utils::datetime::Date>(),
                 row["program_end"].As<userver::utils::datetime::Date>(),
                 row["agreement_begin"].As<userver::utils::datetime::Date>(),
                 row["agreement_end"].As<userver::utils::datetime::Date>(),
                 row["amount"].As<std::int64_t>(),
                 row["interest"].As<std::int16_t>(),
                 row["id_clients"].As<boost::uuids::uuid>(),
                 row["id_main_accounts"].As<boost::uuids::uuid>(),
                 row["id_sec_accounts"].As<boost::uuids::uuid>()};
}

userver::formats::json::Value Serialize(
    const Account& account,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder json_builder;
  json_builder["id"] = account.id;
  json_builder["id_number"] = account.id_number;
  json_builder["code"] = account.code;
  json_builder["activity"] = account.activity;
  json_builder["debit"] = account.debit;
  json_builder["credit"] = account.credit;
  json_builder["balance"] = account.balance;
  json_builder["note"] = account.note;
  return json_builder.ExtractValue();
}

userver::formats::json::Value Serialize(
    const Transaction& tnx,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder json_builder;
  json_builder["id"] = tnx.id;
  json_builder["src_account"] = tnx.src_account;
  json_builder["dst_account"] = tnx.dst_account;
  json_builder["amount"] = tnx.amount;
  json_builder["txn_date"] = tnx.txn_date;
  return json_builder.ExtractValue();
}

Account DeserializeAccount(const userver::storages::postgres::Row& row) {
  return Account{row["id"].As<boost::uuids::uuid>(),
                 row["id_number"].As<std::int64_t>(),
                 row["code"].As<std::int16_t>(),
                 row["activity"].As<std::string>(),
                 row["debit"].As<std::int64_t>(),
                 row["debit"].As<std::int64_t>(),
                 row["debit"].As<std::int64_t>(),
                 row["note"].As<std::optional<std::string>>()};
}

}  // namespace ubank