import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_DATA,
    CONF_TYPE,
    CONF_ID,
)
from esphome.core import CORE
from esphome.components.remote_base import (
    declare_protocol,
    register_binary_sensor,
    register_trigger,
    register_dumper,
    register_action,
)

AUTO_LOAD = ["remote_base"]
CODEOWNERS = ["@Weissnix4711"]

CONF_MSG_TYPE = "message_type"
CONF_MSG_ID = "message_id"

# OpenTherm
(
    OpenThermData,
    OpenThermBinarySensor,
    OpenThermTrigger,
    OpenThermAction,
    OpenThermDumper,
) = declare_protocol("OpenTherm")
OPENTHERM_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_TYPE): cv.hex_int_range(min=0, max=7), # type
        cv.Required(CONF_ID): cv.hex_uint8_t, # id
        cv.Required(CONF_DATA): cv.hex_uint16_t, # data
        #cv.Optional(CONF_SCAN_SLAVE_STATUS, default=False): cv.boolean,
        # This shoudl run a R ID3 and dump the output.
        # Similar to i2c scan option.
    }
)

@register_binary_sensor("opentherm", OpenThermBinarySensor, OPENTHERM_SCHEMA)
def opentherm_binary_sensor(var, config):
    cg.add(
        var.set_data(
            cg.StructInitializer(
                OpenThermData,
                ("type", config[CONF_TYPE]),
                ("id", config[CONF_ID]),
                ("data", config[CONF_DATA]),
            )
        )
    )

@register_trigger("opentherm", OpenThermTrigger, OpenThermData)
def opentherm_trigger(var, config):
    pass


@register_dumper("opentherm", OpenThermDumper)
def opentherm_dumper(var, config):
    pass

@register_action("opentherm", OpenThermAction, OPENTHERM_SCHEMA)
async def opentherm_action(var, config, args):
    template_ = await cg.templatable(config[CONF_TYPE], args, cg.uint8)
    cg.add(var.set_type(template_))
    template_ = await cg.templatable(config[CONF_ID], args, cg.uint8)
    cg.add(var.set_id(template_))
    template_ = await cg.templatable(config[CONF_DATA], args, cg.uint16)
    cg.add(var.set_data(template_))

opentherm_ns = cg.esphome_ns.namespace("opentherm")
OPENTHERM = opentherm_ns.class_(
    "OpenThermComponent", cg.Component
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(OPENTHERM),
        }
    )
    .extend(cv.COMPONENT_SCHEMA),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
