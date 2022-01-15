#include "opentherm_protocol.h"
#include "esphome/core/log.h"

namespace esphome {

namespace remote_base {

static const char *const TAG = "remote.opentherm";

void OpenThermProtocol::encode(RemoteTransmitData *dst, const OpenThermData &data) {
  dst->set_carrier_frequency();
}

optional<OpenThermData> OpenThermProtocol::decode(RemoteReceiveData src) {
  OpenThermData out{
      .type = 0,
      .id = 0,
      .data = 0,
  };

  return out;
}

void OpenThermData::dump(const OpenThermData &data) {
  ESP_LOGD(TAG, "Recieved OpenTherm: type=0x%02X, id=0x%02X, value=0x%04X", data.type, data.id, data.data);
}

} // namespace remote_base
} // namespace esphome
