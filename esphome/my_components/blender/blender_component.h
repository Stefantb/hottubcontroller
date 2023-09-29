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
struct Valve
{
    esphome::number::Number*      manual_override_ = nullptr;
    esphome::output::FloatOutput* out_dac_         = nullptr;

    float lower_limit_   = 0;
    float upper_limit_   = 100;
    float defrost_value_ = 0;
    float out_value_     = 0;
    bool  last_automation_   = false;
    bool  defrost_       = false;

    void set_ll(float v)
    {
        lower_limit_ = v;
    }
    void set_ul(float v)
    {
        upper_limit_ = v;
    }
    void set_defrost(float v)
    {
        defrost_value_ = v;
    }

    void set_output(esphome::output::FloatOutput* out_dac)
    {
        this->out_dac_ = out_dac;
    }

    void set_manual_override_in(esphome::number::Number* in)
    {
        this->manual_override_ = in;
    }

    void set_value(float value, bool automation)
    {
        value = clampf(value, 0.0, 100.0);

        this->out_value_ = value;

        float ll = defrost_ ? defrost_value_ : 0;
        float ul = 100;
        if (automation)
        {
            ll = lower_limit_;
            ul = upper_limit_;
        }
        // else if (last_automation_)
        // {
        //     // going out of automation
        //     // the idea is to maybe delay the defrost output for a little while.
        // }
        last_automation_ = automation;

        // map the value on to the output range of the valve
        float out_range  = ul - ll;
        float actual_out = ll + value / 100 * out_range;
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
        defrost_ = enable;
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

    void adjust_outputs(float ctrl, float flow, bool automation);

    // float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

    void set_manual_in(esphome::switch_::Switch* in)
    {
        this->manual_in_ = in;
    }

    void set_defrost_in(esphome::switch_::Switch* in)
    {
        this->defrost_in_ = in;
    }

    void set_pid_controller(esphome::pid::PIDClimate* in)
    {
        this->control_in_ = in;
    }

    void set_flow_strength_in(esphome::number::Number* in)
    {
        this->flow_strength_in_ = in;
    }

    Valve& hot_valve()
    {
        return hot_valve_;
    }

    Valve& cold_valve()
    {
        return cold_valve_;
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
