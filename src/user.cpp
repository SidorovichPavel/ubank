#include "user.hpp"

#include <userver/server/handlers/handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/utils/assert.hpp>

namespace ubank {

namespace {

class HttpHandlerUser : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user";

  HttpHandlerUser(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest&,
      userver::server::request::RequestContext&) const override {
    
    return std::string("OK!\n");
  }
};
}  // namespace

void AppendUser(userver::components::ComponentList& component_list) {
  component_list.Append<HttpHandlerUser>();
}

}  // namespace ubank