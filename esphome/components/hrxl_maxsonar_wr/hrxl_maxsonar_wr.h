#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"


namespace esphome {
namespace hrxl_maxsonar_wr {

 // Simple enum to represent models.
 enum Model {
   HRXL = 0,  // mm resolution models
   XL = 1,    // cm resolution models
 };

class HrxlMaxsonarWrComponent : public sensor::Sensor, public Component, public uart::UARTDevice {
 public:
  // Nothing really public.

  // ========== INTERNAL METHODS ==========
  void setup() override;
  void set_model(Model model) { this->model_ = model; };
  void loop() override;
  void dump_config() override;

 protected:
  void check_buffer_();
  std::string buffer_;
  Model model_{HRXL};
};

}  // namespace hrxl_maxsonar_wr
}  // namespace esphome
