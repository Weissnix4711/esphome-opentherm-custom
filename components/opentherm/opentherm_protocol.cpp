#include "opentherm_protocol.h"
#include "esphome/core/log.h"

namespace esphome {

namespace remote_base {

static const char *const TAG = "remote.opentherm";

static const uint32_t BIT_TIME_US = 500;
static const uint8_t NBITS = 34; // start + 32b frame + stop (manchester)

void OpenThermProtocol::encode(RemoteTransmitData *dst, const OpenThermData &data) {
  dst->set_carrier_frequency(0);

  dst->item(BIT_TIME_US, BIT_TIME_US); // Start bit

  uint32_t out_data = 0;
  out_data |= data.type << 28;
  out_data |= data.id << 16;
  out_data |= data.data;

  // Parity (total '1' bits in 32b frame should be even)
  uint32_t set = out_data;
  set = set - ((set >> 1) & 0x55555555); // add pairs of bits
  set = (set & 0x33333333) + ((set >> 2) & 0x33333333); // quads
  set = (set + (set >> 4)) & 0x0F0F0F0F; // groups of 8
  set = (set * 0x01010101) >> 24; // horizontal sum of bytes
  out_data |= (((set % 2 == 0) ? 0U : 1U) << 31);

  for (uint32_t mask = 1UL << (NBITS - 3); mask != 0; mask >>= 1) {
    if (out_data & mask) {
      dst->mark(BIT_TIME_US);
      dst->space(BIT_TIME_US);
    } else {
      dst->space(BIT_TIME_US);
      dst->mark(BIT_TIME_US);
    }
  }

  dst->item(BIT_TIME_US, BIT_TIME_US); // Stop bit

  ESP_LOGD(TAG, "out_data: %08X", out_data);
}

optional<OpenThermData> OpenThermProtocol::decode(RemoteReceiveData src) {
  OpenThermData out{
      .type = 0,
      .id = 0,
      .data = 0,
  };

  // Start bit
  if (!(src.expect_mark(BIT_TIME_US) &&
      (src.expect_space(BIT_TIME_US) || src.peek_space(2 * BIT_TIME_US)))) {
    return {};
  }

  // 32b frame
  uint32_t out_data = 0;
  for (uint8_t bit = NBITS - 2; bit >= 1; bit--) {
    if ((src.expect_space(BIT_TIME_US) || src.expect_space(2 * BIT_TIME_US)) &&
        (src.expect_mark(BIT_TIME_US) || src.peek_mark(2 * BIT_TIME_US))) {
      out_data |= 0 << bit;
    } else if ((src.expect_mark(BIT_TIME_US) || src.expect_mark(2 * BIT_TIME_US)) &&
               (src.expect_space(BIT_TIME_US) || src.peek_space(2 * BIT_TIME_US))) {
      out_data |= 1 << bit;
    } else {
      return {};
    }
  }

  // End bit must be '1' (high -> low)
  if (!(src.expect_mark(BIT_TIME_US) || src.expect_mark(2 * BIT_TIME_US))) {
    return {};
  }

  out.type = (out_data >> 28) & 0x7;
  out.id = (out_data >> 16) & 0xFF;
  out.data = out_data & 0xFFFF;
  return out;
}

void OpenThermProtocol::dump(const OpenThermData &data) {
  ESP_LOGD(TAG, "Recieved OpenTherm: type=0x%02X, id=0x%02X, value=0x%04X", data.type, data.id, data.data);
}

} // namespace remote_base
} // namespace esphome
