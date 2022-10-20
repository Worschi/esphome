import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    STATE_CLASS_MEASUREMENT,
    UNIT_METER,
    ICON_ARROW_EXPAND_VERTICAL,
    CONF_MODEL
)

CODEOWNERS = ["@netmikey"]
DEPENDENCIES = ["uart"]

hrxlmaxsonarwr_ns = cg.esphome_ns.namespace("hrxl_maxsonar_wr")
Model = hrxlmaxsonarwr_ns.enum("Model")
MODELS = {
    "HRXL": Model.HRXL,
    "XL": Model.XL,
}
HrxlMaxsonarWrComponent = hrxlmaxsonarwr_ns.class_(
    "HrxlMaxsonarWrComponent", sensor.Sensor, cg.Component, uart.UARTDevice
)

CONFIG_SCHEMA = sensor.sensor_schema(
    HrxlMaxsonarWrComponent,
    unit_of_measurement=UNIT_METER,
    icon=ICON_ARROW_EXPAND_VERTICAL,
    accuracy_decimals=3,
    state_class=STATE_CLASS_MEASUREMENT,
).extend(uart.UART_DEVICE_SCHEMA)

model_option = cv.Schema(
    {cv.Optional(CONF_MODEL, default="HRXL"): cv.enum(MODELS, upper=True)}
)
CONFIG_SCHEMA = CONFIG_SCHEMA.extend(model_option)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    cg.add(var.set_maxsonar_model(config[CONF_MODEL]))
