from esphome import components
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch, output, number, climate, sensor
from esphome.const import CONF_ID, CONF_VOLTAGE, CONF_CURRENT, CONF_STATE

tplink_ns = cg.esphome_ns.namespace('blender')
BlenderComponent = tplink_ns.class_('BlenderComponent', cg.Component)


# ******************************************************************************
#
# ******************************************************************************
CONF_OUT_DAC = 'out_dac'
CONF_IN_ADC = 'in_adc'
CONF_MANUAL_VALVE = 'manual_input'
CONF_OUT_RANGE_LL = 'out_lower_limit'
CONF_OUT_RANGE_UL = 'out_upper_limit'
CONF_OUT_DEFROST = 'out_defrost'

VALVE_SCHEMA = cv.Schema({
    cv.Required(CONF_OUT_DAC): cv.use_id(output.FloatOutputPtr),
    cv.Required(CONF_IN_ADC): cv.use_id(sensor.Sensor),
    cv.Required(CONF_MANUAL_VALVE): cv.use_id(number.Number),
    cv.Optional(CONF_OUT_RANGE_LL): cv.int_range(0, 100),
    cv.Optional(CONF_OUT_RANGE_UL): cv.int_range(0, 100),
    cv.Optional(CONF_OUT_DEFROST): cv.int_range(0, 100),
})

# ******************************************************************************
#
# ******************************************************************************
CONF_MANUAL_SWITCH = 'manual_switch'
CONF_DEFROST_SWITCH = 'defrost_switch'
CONF_CALIBRATE_SWITCH = 'calibrate_switch'
CONF_FLOW_STRENGTH = 'flow_strength_in'
CONF_CONTROL = 'pid_controller_in'
CONF_HOT_VALVE = 'hot_valve'
CONF_COLD_VALVE = 'cold_valve'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BlenderComponent),

    cv.Required(CONF_MANUAL_SWITCH): cv.use_id(switch.Switch),
    cv.Required(CONF_DEFROST_SWITCH): cv.use_id(switch.Switch),
    cv.Required(CONF_CALIBRATE_SWITCH): cv.use_id(switch.Switch),
    cv.Required(CONF_CONTROL): cv.use_id(climate.Climate),
    cv.Required(CONF_FLOW_STRENGTH): cv.use_id(number.Number),
    cv.Required(CONF_HOT_VALVE): VALVE_SCHEMA,
    cv.Required(CONF_COLD_VALVE): VALVE_SCHEMA,
})

# ******************************************************************************
#
# ******************************************************************************
def valve_to_code(var, config):
    ptr = yield cg.get_variable(config[CONF_OUT_DAC])
    cg.add(var.set_output(ptr))

    ptr = yield cg.get_variable(config[CONF_IN_ADC])
    cg.add(var.set_input_adc(ptr))

    ptr = yield cg.get_variable(config[CONF_MANUAL_VALVE])
    cg.add(var.set_manual_override_in(ptr))

    if CONF_OUT_RANGE_LL in config:
        cg.add(var.set_ll(config[CONF_OUT_RANGE_LL]))
    if CONF_OUT_RANGE_UL in config:
        cg.add(var.set_ul(config[CONF_OUT_RANGE_UL]))
    if CONF_OUT_DEFROST in config:
        cg.add(var.set_defrost(config[CONF_OUT_DEFROST]))

# ******************************************************************************
#
# ******************************************************************************
def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)

    ptr = yield cg.get_variable(config[CONF_CALIBRATE_SWITCH])
    cg.add(var.set_calibrate_in(ptr))

    ptr = yield cg.get_variable(config[CONF_MANUAL_SWITCH])
    cg.add(var.set_manual_in(ptr))

    ptr = yield cg.get_variable(config[CONF_DEFROST_SWITCH])
    cg.add(var.set_defrost_in(ptr))

    ptr = yield cg.get_variable(config[CONF_CONTROL])
    cg.add(var.set_pid_controller(ptr))

    ptr = yield cg.get_variable(config[CONF_FLOW_STRENGTH])
    cg.add(var.set_flow_strength_in(ptr))

    yield valve_to_code(var.hot_valve(), config[CONF_HOT_VALVE])
    yield valve_to_code(var.cold_valve(), config[CONF_COLD_VALVE])
