#include "opentherm_protocol.h"
#include "esphome/core/log.h"

namespace esphome {

namespace remote_base {

static const char *const TAG = "remote.opentherm";

//static const uint32_t HEADER_HIGH_US = 400;
//static const uint32_t HEADER_LOW_US = 6100;
//static const uint32_t BIT_HIGH_US = 400;
//static const uint32_t BIT_ONE_LOW_US = 1700;
//static const uint32_t BIT_ZERO_LOW_US = 2800;

static const uint32_t BIT_TIME_US = 500
static const uint8_t NBITS = 34 // start + 32b frame + stop (manchester)

void OpenThermProtocol::encode(RemoteTransmitData *dst, const OpenThermData &data) {
  dst->set_carrier_frequency(0);

  dst->item(BIT_TIME_US, BIT_TIME_US); // Start bit

  uint32_t out_data = 0;
  out_data |= data.type << 28;
  out_data |= data.id << 16;
  out_data |= data.data;

  // Parity (total '1' bits in 32b frame should be even)
  uint32_t set = out_data
  set = set - ((set >> 1) & 0x55555555); // add pairs of bits
  set = (set & 0x33333333) + ((set >> 2) & 0x33333333); // quads
  set = (set + (set >> 4)) & 0x0F0F0F0F; // groups of 8
  set = (set * 0x01010101) >> 24; // horizontal sum of bytes
  out_data |= (((set % 2 == 0) ? 0U : 1U) << 31);

  //for (uint32_t mask = 1UL << (NBITS - 2); mask != 0; mask >>= 1) {
  for (uint32_t mask = 1UL << (32 - 1); mask != 0; mask >>= 1) {
    if (out_data & mask) {
      dst->mark(BIT_TIME_US);
      dst->space(BIT_TIME_US);
    } else {
      dst->space(BIT_TIME_US);
      dst->mark(BIT_TIME_US);
    }
  }

  dst->item(BIT_TIME_US, BIT_TIME_US); // Stop bit
}

optional<OpenThermData> OpenThermProtocol::decode(RemoteReceiveData src) {
  OpenThermData out{
      .type = 0,
      .id = 0,
      .data = 0,
  };

  // Start bit
  if (!src.expect_item(BIT_TIME_US, BIT_TIME_US)) {
    return {};
  }

  for (uint8_t mask)

  for (int bit = NBITS - 4; bit >= 1; bit--) {
    if (src.expect)
  }

  return out;
}

void OpenThermData::dump(const OpenThermData &data) {
  ESP_LOGD(TAG, "Recieved OpenTherm: type=0x%02X, id=0x%02X, value=0x%04X", data.type, data.id, data.data);
}

} // namespace remote_base
} // namespace esphome
