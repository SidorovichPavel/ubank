#include "user.hpp"

#include <optional>
#include <string>

#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/utils/assert.hpp>

namespace ubank {

namespace {

enum class Gender { Female, Man, Other };

enum class FamilyStatus { Single, Married };

enum class Disability { FirstGroup, SecondGroup, ThirdGroup };

struct UserTable {
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
      : HttpHandlerJsonBase(config, component_context) {}
  
  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request, const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const override {
    
    userver::formats::json::ValueBuilder result;
    result["status"] = 200;

    return result.ExtractValue();
  }

 private:
};
}  // namespace

void AppendUser(userver::components::ComponentList& component_list) {
  component_list.Append<HttpHandlerUser>();
}

}  // namespace ubank