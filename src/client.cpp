#include "client.hpp"

#include "client_info.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/database.hpp>
#include <userver/utils/assert.hpp>

namespace ubank {

namespace {

class HttpPostClient : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-client";

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
        "VALUES($1,$2,$3,$4,$5,$6, "
        "$7,$8,$9, "
        "$10,$11,$12,$13,$14, "
        "$15,$16,$17,$18,$19,$20, "
        "$21,$22,$23,$24,$25) "
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
    userver::formats::json::ValueBuilder json_builder;

    return json_builder.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUser(userver::components::ComponentList& component_list) {
  component_list.Append<HttpPostClient>();
  component_list.Append<userver::components::Postgres>("postgres-db-1");
  component_list.Append<userver::clients::dns::Component>();
}

}  // namespace ubank