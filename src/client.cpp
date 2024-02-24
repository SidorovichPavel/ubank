#include "client.hpp"

#include "client_info.hpp"

#include <charconv>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/database.hpp>
#include <userver/storages/postgres/io/date.hpp>
#include <userver/utils/assert.hpp>
#include <userver/utils/datetime/date.hpp>

namespace ubank {

namespace {

class HttpPostClient : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "post-client";

  HttpPostClient(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const override {
    ClientInfo info = Deserialize(json["client"]);

    auto pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO bank_schema.clients(first_name, "
        "middle_name,last_name,gender,passport_series,passport_number, "
        "issuing,issuing_date,id_number, "
        "birth_address,current_city,current_address, "
        "home_number,mobile_number,email,post,place_of_work, "
        "city_of_residence,residence_address, "
        "family_status,citizenship,disability,retiree,monthly_income, "
        "conscription) "
        "VALUES("
        "$1,$2,$3,"
        "$4,$5,$6,"
        "$7,$8,$9,"
        "$10,$11,$12,"
        "$13,$14,$15,$16,$17,"
        "$18,$19,"
        "$20,$21,$22,"
        "$23,$24,$25)"
        "RETURNING id",
        info.first_name, info.middle_name, info.last_name,
        static_cast<std::int16_t>(info.gender), info.passport_series,
        info.passport_number, info.issuing, info.issuing_date, info.id_number,
        info.birth_address, info.current_city, info.current_address,
        info.otp_home_number, info.opt_mobile_number, info.opt_email,
        info.opt_position, info.opt_place_of_work, info.city_of_residence,
        info.residence_address, static_cast<std::int16_t>(info.family_status),
        info.citizenship, static_cast<std::int16_t>(info.disability),
        info.retiree, info.monthly_income, info.conscription);

    // auto size = pg_result.Size();
    int32_t id = pg_result[0].As<int>();

    // request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kBadRequest);
    userver::formats::json::ValueBuilder json_builder;
    json_builder["client"] = info;
    json_builder["client"]["id"] = id;
    return json_builder.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

class HttpPutClient : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "put-client";

  HttpPutClient(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context)
      : userver::server::handlers::HttpHandlerJsonBase(config,
                                                       component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const override {
    auto info = Deserialize(json["client"]);
    auto sid = request.GetPathArg("id");
    std::int64_t id{};
    std::from_chars(sid.data(), sid.data() + sid.length(), id);

    auto pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE bank_schema.clients SET ("
        "first_name,middle_name,last_name,"
        "gender,passport_series,passport_number,"
        "issuing,issuing_date,id_number,"
        "birth_address,current_city,current_address,"
        "home_number,mobile_number,email,post,place_of_work,"
        "city_of_residence,residence_address,"
        "family_status,citizenship,disability,"
        "retiree,monthly_income,conscription)=("
        "$1,$2,$3,"
        "$4,$5,$6,"
        "$7,$8,$9,"
        "$10,$11,$12,"
        "$13,$14,$15,$16,$17,"
        "$18,$19,"
        "$20,$21,$22,"
        "$23,$24,$25) WHERE id = $26",
        info.first_name, info.middle_name, info.last_name,
        static_cast<std::int16_t>(info.gender), info.passport_series,
        info.passport_number, info.issuing, info.issuing_date, info.id_number,
        info.birth_address, info.current_city, info.current_address,
        info.otp_home_number, info.opt_mobile_number, info.opt_email,
        info.opt_position, info.opt_place_of_work, info.city_of_residence,
        info.residence_address, static_cast<std::int16_t>(info.family_status),
        info.citizenship, static_cast<std::int16_t>(info.disability),
        info.retiree, info.monthly_income, info.conscription, id);
    userver::formats::json::ValueBuilder json_builder;
    json_builder["client"] = info;
    json_builder["client"]["id"] = id;
    return json_builder.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

class HttpGetClient : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "get-client";

  HttpGetClient(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context)
      : userver::server::handlers::HttpHandlerJsonBase(config,
                                                       component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const override {
    return GetAllClients();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;

  userver::formats::json::Value GetAllClients() const {
    auto pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT (id) FROM bank_schema.clients");

    userver::formats::json::ValueBuilder json_builder;
    for (auto row : pg_result) {
      json_builder.PushBack(row["id"].As<std::int64_t>());
    }
    if (json_builder.IsEmpty())
      json_builder = userver::formats::json::MakeArray();
    return json_builder.ExtractValue();
  }
};

class HttpGetSingleClient
    : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "get-id-client";

  HttpGetSingleClient(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& component_context)
      : userver::server::handlers::HttpHandlerJsonBase(config,
                                                       component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const override {
    auto sid = request.GetPathArg("id");
    std::int64_t id;
    auto [ptr, errc] =
        std::from_chars(sid.data(), sid.data() + sid.length(), id);
    if (errc == std::errc::invalid_argument)
      throw std::runtime_error("parse error. value of id is invalid");

    auto pg_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * FROM bank_schema.clients WHERE id = $1", id);

    userver::formats::json::ValueBuilder json_builder;

    if (pg_result.Size() == 1u) {
      json_builder["client"] = Deserialize(*pg_result.begin());
      json_builder["client"]["id"] = id;
    }
    return json_builder.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendClient(userver::components::ComponentList& component_list) {
  component_list.Append<HttpPostClient>();
  component_list.Append<HttpPutClient>();
  component_list.Append<HttpGetClient>();
  component_list.Append<HttpGetSingleClient>();
}

}  // namespace ubank