import esphome.codegen as cg
import esphome.config_validation as cv

# OpenTherm
(
    OpenThermData,
    OpenThermBinarySensor,
    OpenThermTrigger,
    OpenThermAction,
    OpenThermDumper,
) = declare_protocol("OpenTherm")
OPENTHERM_SCHEMA = cv.schema(
    {
        cv.Required(CONF_IDK): cv.hex_int_range(min=0, max=7), # type
        cv.Required(CONF_IDK): cv.hex_uint8_t# id
        cv.Required(CONF_IDK): cv.hex_uint16_t # data
    }
)

@register_binary_sensor("opentherm", OpenThermBinarySensor, OPENTHERM_SCHEMA)
def opentherm_binary_sensor(var, config):
    cg.add(
        var.set_data(
            cg.StructInitializer(
                OpenThermData,
                ("type", config[CONF_IDK]),
                ("id", config[CONF_IDK]),
                ("data", config[CONF_IDK]),
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
async def sony_action(var, config, args):
    template_ = await cg.templatable(config[CONF_IDK], args, cg.uint8)
    cg.add(var.set_type(template_))
    template_ = await cg.templatable(config[CONF_NBITS], args, cg.uint8)
    cg.add(var.set_id(template_))
    template_ = await cg.templatable(config[CONF_IDK], args, cg.uint16)
    cg.add(var.set_data(template_))
