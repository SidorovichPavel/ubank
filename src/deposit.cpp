#include "deposit.hpp"
#include "deposit_info.hpp"

#include <random>
#include <string_view>

#include <boost/uuid/uuid_generators.hpp>

#include <userver/components/component.hpp>
#include <userver/formats/serialize/boost_uuid.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/date.hpp>
#include <userver/storages/postgres/io/uuid.hpp>
#include <userver/storages/postgres/postgres.hpp>

namespace ubank {

namespace {

class HttpPostDeposit : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "post-deposit";

  HttpPostDeposit(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      [[maybe_unused]] const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      [[maybe_unused]] userver::server::request::RequestContext&
          request_context) const override {
    boost::uuids::uuid idc{};
    userver::formats::json::ValueBuilder json_builder;
    try {
      auto sidc = request.GetPathArg("idc");
      idc = boost::uuids::string_generator{}(sidc);
    } catch (const std::runtime_error& re) {
      request.GetHttpResponse().SetStatus(
          userver::server::http::HttpStatus::kBadRequest);
      json_builder["code"] = 400;
      json_builder["message"] = re.what();
      return json_builder.ExtractValue();
    }

    Deposit deposit = DeserializeDeposit(json["deposit"]);
    deposit.id = boost::uuids::random_generator{}();
    deposit.id_clients = idc;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::int16_t> ud(1000, 2000);

    Account main{boost::uuids::random_generator{}(),
                 static_cast<std::int64_t>(gen()),
                 ud(gen),
                 "activity",
                 deposit.amount,
                 0,
                 0,
                 std::nullopt};
    auto pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO bank_schema.accounts ("
        "id,id_number, "
        "code, activity, "
        "debit, credit, balance, "
        "note) "
        "VALUES ("
        "$1, $2, "
        "$3, $4, "
        "$5, $6, $7, "
        "$8)",
        main.id, main.id_number, main.code, main.activity, main.debit,
        main.credit, main.balance, main.note);
    deposit.id_main_accounts = main.id;

    Account sec{boost::uuids::random_generator{}(),
                static_cast<std::int64_t>(gen()),
                ud(gen),
                "activity",
                0,
                0,
                0,
                std::nullopt};
    pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO bank_schema.accounts ("
        "id,id_number, "
        "code, activity, "
        "debit, credit, balance, "
        "note) "
        "VALUES ("
        "$1, $2, "
        "$3, $4, "
        "$5, $6, $7, "
        "$8)",
        sec.id, sec.id_number, sec.code, sec.activity, sec.debit, sec.credit,
        sec.balance, sec.note);
    deposit.id_sec_accounts = sec.id;

    pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO bank_schema.deposits ("
        "id, category, agreement_number, "
        "program_begin, program_end, "
        "agreement_begin, agreement_end, "
        "amount, interest, "
        "id_clients, id_main_accounts, id_sec_accounts)"
        "VALUES ("
        "$1, $2, $3, "
        "$4, $5, "
        "$6, $7, "
        "$8, $9, "
        "$10, $11, $12) "
        "RETURNING *",
        deposit.id, deposit.category, deposit.agreement_number,
        deposit.program_begin, deposit.program_end, deposit.agreement_begin,
        deposit.agreement_end, deposit.amount, deposit.interest,
        deposit.id_clients, deposit.id_main_accounts, deposit.id_sec_accounts);

    if (pg_result.IsEmpty()) {
      request.GetHttpResponse().SetStatus(
          userver::server::http::HttpStatus::kBadRequest);
      json_builder["code"] = 400;
      json_builder["message"] =
          "Bad request. The client with the required ID does not exist.";
      return json_builder.ExtractValue();
    }

    json_builder["deposit"] = DeserializeDeposit(*pg_result.begin());

    return json_builder.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

class HttpGetDeposits : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "get-deposits";

  HttpGetDeposits(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      [[maybe_unused]] const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      [[maybe_unused]] userver::server::request::RequestContext&
          request_context) const override {
    boost::uuids::uuid idc{};
    userver::formats::json::ValueBuilder json_builder;
    try {
      auto sidc = request.GetPathArg("idc");
      idc = boost::uuids::string_generator{}(sidc);
    } catch (const std::runtime_error& re) {
      request.GetHttpResponse().SetStatus(
          userver::server::http::HttpStatus::kBadRequest);
      json_builder["code"] = 400;
      json_builder["message"] = re.what();
      return json_builder.ExtractValue();
    }

    auto pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT id from bank_schema.deposits "
        "WHERE id_clients = $1",
        idc);

    if (pg_result.IsEmpty()) return userver::formats::json::MakeArray();

    for (auto&& row : pg_result)
      json_builder.PushBack(row["id"].As<boost::uuids::uuid>());

    return json_builder.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

class HttpGetDeposit : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "get-deposit";

  HttpGetDeposit(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      [[maybe_unused]] const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      [[maybe_unused]] userver::server::request::RequestContext&
          request_context) const override {
    boost::uuids::uuid idc, idd{};
    userver::formats::json::ValueBuilder json_builder;
    try {
      auto sidc = request.GetPathArg("idc");
      idc = boost::uuids::string_generator{}(sidc);
      auto sidd = request.GetPathArg("idd");
      idd = boost::uuids::string_generator{}(sidd);
    } catch (const std::runtime_error& re) {
      request.GetHttpResponse().SetStatus(
          userver::server::http::HttpStatus::kBadRequest);
      json_builder["code"] = 400;
      json_builder["message"] = re.what();
      return json_builder.ExtractValue();
    }

    auto pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * FROM bank_schema.deposits "
        "WHERE id = $1",
        idd);

    if (pg_result.IsEmpty()) {
      request.GetHttpResponse().SetStatus(
          userver::server::http::HttpStatus::kBadRequest);
      json_builder["code"] = 400;
      json_builder["message"] =
          "Bad request. The client with the required ID does not exist.";
      return json_builder.ExtractValue();
    }

    json_builder["deposit"] = DeserializeDeposit(*pg_result.begin());

    return json_builder.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

class HttpGetAccount : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "get-account-2";

  HttpGetAccount(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      [[maybe_unused]] const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      [[maybe_unused]] userver::server::request::RequestContext&
          request_context) const override {
    boost::uuids::uuid idc{}, idd{}, ida;
    userver::formats::json::ValueBuilder json_builder;
    try {
      auto sidc = request.GetPathArg("idc");
      idc = boost::uuids::string_generator{}(sidc);
      auto sidd = request.GetPathArg("idd");
      idd = boost::uuids::string_generator{}(sidd);
      auto sida = request.GetPathArg("ida");
      ida = boost::uuids::string_generator{}(sida);
    } catch (const std::runtime_error& re) {
      request.GetHttpResponse().SetStatus(
          userver::server::http::HttpStatus::kBadRequest);
      json_builder["code"] = 400;
      json_builder["message"] = re.what();
      return json_builder.ExtractValue();
    }

    auto pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * FROM bank_schema.accounts "
        "WHERE id = $1",
        ida);

    if (pg_result.IsEmpty()) {
      request.GetHttpResponse().SetStatus(
          userver::server::http::HttpStatus::kBadRequest);
      json_builder["code"] = 400;
      json_builder["message"] =
          "Bad request. The client with the required ID does not exist.";
      return json_builder.ExtractValue();
    }

    json_builder["account"] = DeserializeAccount(*pg_result.begin());

    return json_builder.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendDeposit(userver::components::ComponentList& component_list) {
  component_list.Append<HttpPostDeposit>();
  component_list.Append<HttpGetDeposits>();
  component_list.Append<HttpGetDeposit>();
  component_list.Append<HttpGetAccount>();
}

}  // namespace ubank