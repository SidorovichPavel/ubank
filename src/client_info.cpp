#include "client_info.hpp"

#include <userver/storages/postgres/database.hpp>
#include <userver/storages/postgres/io/date.hpp>
#include <userver/utils/datetime/date.hpp>

namespace ubank {
enum class Gender : std::int16_t { Man, Female, Other };

constexpr userver::utils::TrivialBiMap kGenderEnumDescription =
    [](auto selector) {
      return selector()
          .Case("Male", Gender::Man)
          .Case("Female", Gender::Female)
          .Case("Other", Gender::Other);
    };

enum class FamilyStatus : std::int16_t { Single, Married };

constexpr userver::utils::TrivialBiMap kFamilyStatusEnumDescription =
    [](auto selector) {
      return selector()
          .Case("Single", FamilyStatus::Single)
          .Case("Married", FamilyStatus::Married);
    };

enum class Disability : std::int16_t {
  None,
  FirstGroup,
  SecondGroup,
  ThirdGroup
};

constexpr userver::utils::TrivialBiMap kDisabilityEnumDescription =
    [](auto selector) {
      return selector()
          .Case("None", Disability::None)
          .Case("FirstGroup", Disability::FirstGroup)
          .Case("SecondGroup", Disability::SecondGroup)
          .Case("ThirdGroup", Disability::ThirdGroup);
    };

userver::formats::json::Value Serialize(
    const ClientInfo& info,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["first_name"] = info.first_name;
  builder["middle_name"] = info.middle_name;
  builder["last_name"] = info.last_name;
  builder["gender"] = *kGenderEnumDescription.TryFind(info.gender);
  builder["passport_series"] = info.passport_series;
  builder["passport_number"] = info.passport_number;
  builder["issuing"] = info.issuing;
  builder["issuing_date"] =
      std::chrono::system_clock::to_time_t(info.issuing_date);
  builder["id_number"] = info.id_number;
  builder["birth_address"] = info.birth_address;
  builder["current_city"] = info.current_city;
  builder["current_address"] = info.current_address;

  if (info.otp_home_number) builder["home_number"] = *info.otp_home_number;
  if (info.opt_mobile_number)
    builder["mobile_number"] = *info.opt_mobile_number;
  if (info.opt_email) builder["email"] = *info.opt_email;
  if (info.opt_position) builder["position"] = *info.opt_position;
  if (info.opt_place_of_work)
    builder["place_of_work"] = *info.opt_place_of_work;

  builder["city_of_residence"] = info.city_of_residence;
  builder["residence_address"] = info.residence_address;
  builder["family_status"] =
      *kFamilyStatusEnumDescription.TryFind(info.family_status);
  builder["citizenship"] = info.citizenship;
  builder["disability"] = *kDisabilityEnumDescription.TryFind(info.disability);

  builder["retiree"] = info.retiree;
  builder["monthly_income"] = info.monthly_income;
  builder["conscription"] = info.conscription;
  return builder.ExtractValue();
}

template <typename T>
std::optional<T> json_as_optional(const userver::formats::json::Value& json,
                                  std::string_view key) {
  if (json.HasMember(key))
    return std::make_optional(json[key].As<T>());
  else
    return std::nullopt;
}

template <typename Mapper>
auto get_mapped_field_data(const userver::formats::json::Value& json,
                           std::string_view key, const Mapper& mapper) {
  auto opt = mapper.TryFind(json[key].As<std::string>());
  if (!opt) {
    std::string error_str{"json parse error. field \""};
    error_str += key;
    error_str += "\" has undefined value";
    throw std::runtime_error(error_str);
  }
  return *opt;
}

ClientInfo Deserialize(const userver::formats::json::Value& json) {
  return ClientInfo{
      .first_name = json["first_name"].As<std::string>(),
      .middle_name = json["middle_name"].As<std::string>(),
      .last_name = json["last_name"].As<std::string>(),

      .gender = get_mapped_field_data(json, "gender", kGenderEnumDescription),

      .passport_series = json["passport_series"].As<std::string>(),
      .passport_number = json["passport_number"].As<std::string>(),
      .issuing = json["issuing"].As<std::string>(),

      .issuing_date = std::chrono::system_clock::from_time_t(
          static_cast<std::time_t>(json["issuing_date"].As<std::int64_t>())),

      .id_number = json["id_number"].As<std::string>(),
      .birth_address = json["birth_address"].As<std::string>(),
      .current_city = json["current_city"].As<std::string>(),
      .current_address = json["current_address"].As<std::string>(),

      .otp_home_number = json_as_optional<std::string>(json, "home_number"),
      .opt_mobile_number = json_as_optional<std::string>(json, "mobile_number"),
      .opt_email = json_as_optional<std::string>(json, "email"),
      .opt_position = json_as_optional<std::string>(json, "position"),
      .opt_place_of_work = json_as_optional<std::string>(json, "place_of_work"),

      .city_of_residence = json["city_of_residence"].As<std::string>(),
      .residence_address = json["residence_address"].As<std::string>(),

      .family_status = get_mapped_field_data(json, "family_status",
                                             kFamilyStatusEnumDescription),

      .citizenship = json["citizenship"].As<std::string>(),

      .disability =
          get_mapped_field_data(json, "disability", kDisabilityEnumDescription),

      .retiree = json["retiree"].As<bool>(),
      .monthly_income = json["monthly_income"].As<int>(),
      .conscription = json["conscription"].As<bool>()};
}

ClientInfo Deserialize(const userver::storages::postgres::Row& row) {
  auto optional_field =
      [&](const std::string& key) -> std::optional<std::string> {
    if (row[key].IsNull())
      return std::make_optional<std::string>(row[key].As<std::string>());
    else
      return std::nullopt;
  };

  return ClientInfo{
      row["first_name"].As<std::string>(),
      row["middle_name"].As<std::string>(),
      row["last_name"].As<std::string>(),

      static_cast<Gender>(row["gender"].As<std::int16_t>()),

      row["passport_series"].As<std::string>(),
      row["passport_number"].As<std::string>(),
      row["issuing"].As<std::string>(),
      row["issuing_date"].As<userver::utils::datetime::Date>().GetSysDays(),
      row["id_number"].As<std::string>(),

      row["birth_address"].As<std::string>(),
      row["current_city"].As<std::string>(),
      row["current_address"].As<std::string>(),

      optional_field("home_number"),
      optional_field("mobile_number"),
      optional_field("email"),
      optional_field("post"),
      optional_field("place_of_work"),

      row["city_of_residence"].As<std::string>(),
      row["residence_address"].As<std::string>(),
      static_cast<FamilyStatus>(row["family_status"].As<std::int16_t>()),
      row["citizenship"].As<std::string>(),
      static_cast<Disability>(row["disability"].As<std::int16_t>()),
      row["retiree"].As<bool>(),
      row["monthly_income"].As<int>(),
      row["conscription"].As<bool>()};
}

}  // namespace ubank