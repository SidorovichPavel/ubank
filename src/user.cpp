#include "user.hpp"

#include <optional>
#include <string>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace ubank {

namespace {

enum class Gender { Female, Man, Other };

enum class FamilyStatus { Single, Married };

enum class Disability { FirstGroup, SecondGroup, ThirdGroup };

struct UserInfo {
  std::string first_name;
  std::string middle_name;
  std::string last_name;
  Gender gender;
  std::string passport_series;
  std::string passport_number;
  std::string issuing;
  std::string issuing_date;
  std::string id_number;
  std::string birth_address;
  std::string current_city;
  std::string current_address;
  std::optional<std::string> otp_home_number;
  std::optional<std::string> opt_mobile_number;
  std::optional<std::string> opt_email;
  std::optional<std::string> opt_position;
  std::optional<std::string> opt_place_of_work;
  std::string city_of_residence;
  std::string residence_address;
  FamilyStatus family_status;
  std::string citizenship;
  std::optional<Disability> otp_disability;
  bool retiree;
  int monthly_income;
  bool conscription;
};

class HttpHandlerUser : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-user";

  HttpHandlerUser(
      const userver::components::ComponentConfig& config,
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
    userver::formats::json::ValueBuilder result;
    result["status"] = 200;

    UserInfo user;
    
    pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO bank_schema.clients(first_name, "
        "middle_name,last_name,gender,passport_series,passport_number, "
        "issuing,issuing_date,id_number, "
        "birth_address,current_city,current_address,city_of_residence, "
        "residence_address,family_status,citizenship,retiree,monthly_income, "
        "conscription) "
        "VALUES('pavel','gennadevich','sidorovich',1,'KH','2462649',' "
        "Волковысский РОВД','2014-01-01','id_number', "
        "'birth_address','current_city','current_address','city_of_residence' "
        ",'residence_address',0,'citizenship',false,100,false) "
        "RETURNING id");

    // request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return result.ExtractValue();
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};
}  // namespace

void AppendUser(userver::components::ComponentList& component_list) {
  component_list.Append<HttpHandlerUser>();
  component_list.Append<userver::components::Postgres>("postgres-db-1");
  component_list.Append<userver::clients::dns::Component>();
}

}  // namespace ubank