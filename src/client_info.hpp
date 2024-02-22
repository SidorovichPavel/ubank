#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

#include <userver/formats/json.hpp>
#include <userver/storages/postgres/database.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/utils/trivial_map.hpp>

namespace ubank {

enum class Gender : std::int16_t;

enum class FamilyStatus : std::int16_t;

enum class Disability : std::int16_t;

struct ClientInfo {
  std::string first_name;
  std::string middle_name;
  std::string last_name;
  Gender gender;
  std::string passport_series;
  std::string passport_number;
  std::string issuing;
  std::chrono::system_clock::time_point issuing_date;
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
  Disability disability;
  bool retiree;
  int monthly_income;
  bool conscription;
};

userver::formats::json::Value Serialize(
    const ClientInfo& info,
    userver::formats::serialize::To<userver::formats::json::Value>);

ClientInfo Deserialize(const userver::formats::json::Value& json);
ClientInfo Deserialize(const userver::storages::postgres::Row& row);

}  // namespace ubank