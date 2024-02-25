#include "deposit.hpp"
#include "deposit_info.hpp"

#include <string_view>

#include <boost/uuid/uuid_generators.hpp>

#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/component.hpp>
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
        
    return userver::formats::json::MakeObject();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

class HttpGetDeposits : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "get-deposits-id";
};

class HttpGetDeposit : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "get-deposit";
};

class HttpGetAccount : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "get-account";
};

}  // namespace

void AppendDeposit(userver::components::ComponentList& component_list) {
  component_list.Append<HttpPostDeposit>();
}

}  // namespace ubank