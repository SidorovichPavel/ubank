#include "client_info.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <userver/storages/postgres/database.hpp>
#include <userver/storages/postgres/io/date.hpp>
#include <userver/storages/postgres/io/uuid.hpp>
#include <userver/utils/datetime/date.hpp>
#include <userver/formats/json/serialize.hpp>

namespace ubank {
enum class Gender : std::int16_t { Male, Female, Other };

constexpr userver::utils::TrivialBiMap kGenderEnumDescription =
    [](auto selector) {
      return selector()
          .Case("Male", Gender::Male)
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
  builder["id"] = boost::uuids::to_string(info.id);
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
      boost::uuids::nil_generator{}(),
      json["first_name"].As<std::string>(),
      json["middle_name"].As<std::string>(),
      json["last_name"].As<std::string>(),

      get_mapped_field_data(json, "gender", kGenderEnumDescription),

      json["passport_series"].As<std::string>(),
      json["passport_number"].As<std::string>(),
      json["issuing"].As<std::string>(),

      std::chrono::system_clock::from_time_t(
          static_cast<std::time_t>(json["issuing_date"].As<std::int64_t>())),

      json["id_number"].As<std::string>(),
      json["birth_address"].As<std::string>(),
      json["current_city"].As<std::string>(),
      json["current_address"].As<std::string>(),

      json["home_number"].As<std::optional<std::string>>(),
      json["mobile_number"].As<std::optional<std::string>>(),
      json["email"].As<std::optional<std::string>>(),
      json["position"].As<std::optional<std::string>>(),
      json["place_of_work"].As<std::optional<std::string>>(),

      json["city_of_residence"].As<std::string>(),
      json["residence_address"].As<std::string>(),

      get_mapped_field_data(json, "family_status",
                            kFamilyStatusEnumDescription),

      json["citizenship"].As<std::string>(),

      get_mapped_field_data(json, "disability", kDisabilityEnumDescription),

      json["retiree"].As<bool>(),
      json["monthly_income"].As<int>(),
      json["conscription"].As<bool>()};
}

ClientInfo Deserialize(const userver::storages::postgres::Row& row) {

  return ClientInfo{
      row["id"].As<boost::uuids::uuid>(),
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

      row["home_number"].As<std::optional<std::string>>(),
      row["mobile_number"].As<std::optional<std::string>>(),
      row["email"].As<std::optional<std::string>>(),
      row["post"].As<std::optional<std::string>>(),
      row["place_of_work"].As<std::optional<std::string>>(),

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