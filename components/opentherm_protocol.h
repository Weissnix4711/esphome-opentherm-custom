#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

enum OpenThermDataType : unsigned int : 3 {
  OPENTHERM_MSG_TYPE_READ_DATA = 0b000,
  OPENTHERM_MSG_TYPE_WRITE_DATA = 0b001,
  OPENTHERM_MSG_TYPE_INVALID_DATA = 0b010,
  OPENTHERM_MSG_TYPE_READ_ACK = 0b100,
  OPENTHERM_MSG_TYPE_WRITE_ACK = 0b101,
  OPENTHERM_MSG_TYPE_DATA_INVALID = 0b110,
  OPENTHERM_MSG_TYPE_UNKNOWN_DATAID = 0b111,
}

struct OpenThermData {
  OpenThermDataType type;
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