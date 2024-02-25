#include "deposit_info.hpp"

#include <userver/utils/trivial_map.hpp>

namespace ubank {

enum class Currency : std::int16_t {
  USD,
  EUR,
  BYN,
};

constexpr auto kCurrencyEnumDescription =
    userver::utils::TrivialBiMap([](auto selector) {
      return selector.Case("USD", Currency::USD)
          .Case("EUR", Currency::EUR)
          .Case("BYN", Currency::BYN);
    });

}  // namespace ubank