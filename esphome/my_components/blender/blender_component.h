#pragma once

#include "esphome/components/number/number.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/pid/pid_climate.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

#include <map>
#include <memory>
#include <set>

namespace esphome
{
namespace blender
{

//*****************************************************************************
//
//*****************************************************************************
static inline float clampf(float value, float min, float max)
{
    if (value > max)
    {
        return max;
    }
    if (value < min)
    {
        return min;
    }
    return value;
}

//*****************************************************************************
//
//*****************************************************************************
class Valve
{

    float lower_limit_ = 29;
    float upper_limit_ = 100;
    float out_value_   = 0;

    esphome::number::Number*      manual_override_ = nullptr;
    esphome::output::FloatOutput* out_dac_         = nullptr;

  public:
    void set_output(esphome::output::FloatOutput* out_dac)
    {
        this->out_dac_ = out_dac;
    }

    void set_manual_override_in(esphome::number::Number* in)
    {
        this->manual_override_ = in;
    }

    void set_value(float value)
    {
        value = clampf(value, 0.0, 100.0);

        this->out_value_ = value;

        // map the value on to the output range of the valve
        float out_range  = upper_limit_ - lower_limit_;
        float actual_out = lower_limit_ + value / 100 * out_range;

        if (this->out_dac_)
        {
            this->out_dac_->set_level(actual_out / 100);
        }
    }

    void use_manual()
    {
        float manual_value = this->manual_override_->state;
        if (this->out_dac_)
        {
            this->out_dac_->set_level(manual_value / 100);
        }
    }

    float get_value()
    {
        return this->out_value_;
    }

    void defrost(bool enable)
    {
        lower_limit_ = enable ? 32 : 29;
    }
};

class BlenderComponent : public esphome::Component
{
  public:
    BlenderComponent();
    ~BlenderComponent();

    void setup() override;
    void loop() override;
    void dump_config() override;

    void adjust_outputs(float ctrl, float flow);

    // float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

    void set_hot_out_dac(esphome::output::FloatOutput* out)
    {
        this->hot_valve_.set_output(out);
    }

    void set_cold_out_dac(esphome::output::FloatOutput* out)
    {
        this->cold_valve_.set_output(out);
    }

    void set_flow_strength_in(esphome::number::Number* in)
    {
        this->flow_strength_in_ = in;
    }

    void set_pid_controller(esphome::pid::PIDClimate* in)
    {
        this->control_in_ = in;
    }

    void set_manual_hot_valve(esphome::number::Number* in)
    {
        this->hot_valve_.set_manual_override_in(in);
    }

    void set_manual_cold_valve(esphome::number::Number* in)
    {
        this->cold_valve_.set_manual_override_in(in);
    }

    void set_manual_in(esphome::switch_::Switch* in)
    {
        this->manual_in_ = in;
    }

    void set_defrost_in(esphome::switch_::Switch* in)
    {
        this->defrost_in_ = in;
    }

  private:
    esphome::number::Number*  flow_strength_in_ = nullptr;
    esphome::pid::PIDClimate* control_in_       = nullptr;
    esphome::switch_::Switch* manual_in_        = nullptr;
    esphome::switch_::Switch* defrost_in_       = nullptr;

    Valve hot_valve_;
    Valve cold_valve_;
};

} // namespace blender
} // namespace esphome
