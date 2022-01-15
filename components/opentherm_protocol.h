#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {

namespace remote_base {

struct OpenThermData {
  uint8_t type;
  uint8_t id;
  uint16_t data;

  bool operator==(const OpenThermData &rhs) const { return type == rhs.id && type == rhs.id && data == rhs.data; }
}

class OpenThermProtocol : public RemoteProtocol<OpenThermData> {
 private:
  void encode(RemoteTransmitData *dst, const OpenThermData &data) override;
  optional<OpenThermData> decode(RemoteReceiveData src) override;
  void dump(const OpenThermData &data) override;
};

DECLARE_REMOTE_PROTOCOL(OpenTherm)

template<typename... Ts> class OpenThermAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint8_t, type)
  TEMPLATABLE_VALUE(uint8_t, id)
  TEMPLATABLE_VALUE(uint16_t, data)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    OpenThermData data{};
    data.type = this->type_.value(x...);
    data.id = this->id_.value(x...);
    data.data = this->data_.value(x...);
    OpenThermProtocol().encode(dst, data);
  }
};

} // namespace remote_base
} // namespace esphome