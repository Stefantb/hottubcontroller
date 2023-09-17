from esphome import components
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch, pid, output, number, climate
from esphome.const import CONF_ID, CONF_VOLTAGE, CONF_CURRENT, CONF_STATE

tplink_ns = cg.esphome_ns.namespace('blender')
BlenderComponent = tplink_ns.class_('BlenderComponent', cg.Component)

CONF_HOT_OUT_DAC = 'hot_out_dac'
CONF_COLD_OUT_DAC = 'cold_out_dac'
CONF_FLOW_STRENGTH = 'flow_strength_in'
CONF_CONTROL = 'pid_controller_in'
CONF_MANUAL_SWITCH = 'manual_switch'
CONF_DEFROST_SWITCH = 'defrost_switch'
CONF_MANUAL_HOT_VALVE = 'manual_hot_valve'
CONF_MANUAL_COLD_VALVE = 'manual_cold_valve'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BlenderComponent),

    cv.Required(CONF_CONTROL): cv.use_id(climate.Climate),
    cv.Required(CONF_HOT_OUT_DAC): cv.use_id(output.FloatOutputPtr),
    cv.Required(CONF_COLD_OUT_DAC): cv.use_id(output.FloatOutputPtr),
    cv.Required(CONF_FLOW_STRENGTH): cv.use_id(number.Number),
    cv.Required(CONF_MANUAL_SWITCH): cv.use_id(switch.Switch),
    cv.Required(CONF_DEFROST_SWITCH): cv.use_id(switch.Switch),
    cv.Required(CONF_MANUAL_HOT_VALVE): cv.use_id(number.Number),
    cv.Required(CONF_MANUAL_COLD_VALVE): cv.use_id(number.Number),
})


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)

    if CONF_HOT_OUT_DAC in config:
        ptr = yield cg.get_variable(config[CONF_HOT_OUT_DAC])
        cg.add(var.set_hot_out_dac(ptr))

    if CONF_COLD_OUT_DAC in config:
        ptr = yield cg.get_variable(config[CONF_COLD_OUT_DAC])
        cg.add(var.set_cold_out_dac(ptr))

    if CONF_FLOW_STRENGTH in config:
        ptr = yield cg.get_variable(config[CONF_FLOW_STRENGTH])
        cg.add(var.set_flow_strength_in(ptr))

    if CONF_MANUAL_HOT_VALVE in config:
        ptr = yield cg.get_variable(config[CONF_MANUAL_HOT_VALVE])
        cg.add(var.set_manual_hot_valve(ptr))

    if CONF_MANUAL_COLD_VALVE in config:
        ptr = yield cg.get_variable(config[CONF_MANUAL_COLD_VALVE])
        cg.add(var.set_manual_cold_valve(ptr))

    if CONF_MANUAL_SWITCH in config:
        ptr = yield cg.get_variable(config[CONF_MANUAL_SWITCH])
        cg.add(var.set_manual_in(ptr))

    if CONF_MANUAL_SWITCH in config:
        ptr = yield cg.get_variable(config[CONF_DEFROST_SWITCH])
        cg.add(var.set_defrost_in(ptr))


    # not optional
    ptr = yield cg.get_variable(config[CONF_CONTROL])
    cg.add(var.set_pid_controller(ptr))
