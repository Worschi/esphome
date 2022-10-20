// Official Datasheet:
//   https://www.maxbotix.com/documents/HRXL-MaxSonar-WR_Datasheet.pdf
//
// This implementation is designed to work with the TTL Versions of the
// MaxBotix HRXL MaxSonar WR sensor series. The sensor's TTL Pin (5) should be
// wired to one of the ESP's input pins and configured as uart rx_pin.

#include "hrxl_maxsonar_wr.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hrxl_maxsonar_wr {

static const uint8_t ASCII_CR = 0x0D;
static const uint8_t ASCII_NBSP = 0xFF;
static int MAX_DATA_LENGTH_BYTES = 6;  // = 6
static float ACCURACY = 0.001; 
static const char* TAG = "hrxl.maxsonar.wr.sensor";


/**
 * The high resolution (hr) models output something like "R1234\r" at a fixed 
 * rate of 6 Hz. Where 1234 means a distance of 1,234 m.
 * 
 * The normal resolution models output something like "R123\r" at a fixed 
 * rate of 6 Hz to 10 Hz. Where 123 means a distance of 1,23 m. 
 */
void HrxlMaxsonarWrComponent::loop() {
  uint8_t data;

  while (this->available() > 0) {
    if (this->read_byte(&data)) {
      buffer_ += (char) data;
      this->check_buffer_();
    }
  }
}

void HrxlMaxsonarWrComponent::setup() {
  this->set_maxsonar_model(this->model_);
}

void HrxlMaxsonarWrComponent::check_buffer_() {
  // The sensor seems to inject a rogue ASCII 255 byte from time to time. Get rid of that.
  if (this->buffer_.back() == static_cast<char>(ASCII_NBSP)) {
    this->buffer_.pop_back();
    return;
  }

  // Stop reading at ASCII_CR. Also prevent the buffer from growing
  // indefinitely if no ASCII_CR is received after MAX_DATA_LENGTH_BYTES.
  if (this->buffer_.back() == static_cast<char>(ASCII_CR) || this->buffer_.length() >= MAX_DATA_LENGTH_BYTES) {
    ESP_LOGV(TAG, "Read from serial: %s", this->buffer_.c_str());

    if (this->buffer_.length() == MAX_DATA_LENGTH_BYTES && this->buffer_[0] == 'R' &&
        this->buffer_.back() == static_cast<char>(ASCII_CR)) {
      int result = parse_number<int>(this->buffer_.substr(1, MAX_DATA_LENGTH_BYTES - 2)).value_or(0);
      float meters = float(result) * ACCURACY;
      if (this->model_ == XL) {
        ESP_LOGV(TAG, "Distance from sensor: %d cm, %f m", result, meters);
      } else {
        ESP_LOGV(TAG, "Distance from sensor: %d mm, %f m", result, meters);
      }
      this->publish_state(meters);
    } else {
      ESP_LOGW(TAG, "Invalid data read from sensor: %s", this->buffer_.c_str());
    }
    this->buffer_.clear();
  }
}

void HrxlMaxsonarWrComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "HRXL MaxSonar WR Sensor:");
  LOG_SENSOR("  ", "Distance", this);
  // As specified in the sensor's data sheet
  this->check_uart_settings(9600, 1, esphome::uart::UART_CONFIG_PARITY_NONE, 8);
}

void HrxlMaxsonarWrComponent::set_maxsonar_model(Model model) {

  switch (this->model_) {
    
  case XL: 
    TAG = "xl.maxsonar.wr.sensor";
    MAX_DATA_LENGTH_BYTES = 5;
    ACCURACY = 0.01;

  default:
    TAG = "hrxl.maxsonar.wr.sensor";
    MAX_DATA_LENGTH_BYTES = 6;
    ACCURACY = 0.001;
  } 
}

}  // namespace hrxl_maxsonar_wr
}  // namespace esphome
