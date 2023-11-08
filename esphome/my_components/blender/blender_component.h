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
    enum class DefrostState
    {
        INIT,
        ACTIVE,
        OFF
    };

    enum class CalibrationState
    {
        IDLE,
        INIT,
        ACTIVE,
    };

    esphome::number::Number*      manual_override_ = nullptr;
    esphome::output::FloatOutput* out_dac_         = nullptr; // dac writes 0.0 -> 1.0 (zero to 100%)
    esphome::sensor::Sensor*      in_adc_          = nullptr; // adc reads in voltage (0.0 -> 3.3) v.

    float            lower_limit_       = 0;
    float            upper_limit_       = 100;
    float            defrost_value_     = 0;
    float            out_value_         = 0;
    DefrostState     defrost_state_     = DefrostState::INIT;
    CalibrationState calibration_state_ = CalibrationState::IDLE;
    uint32_t         tally              = 0;

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

    void set_input_adc(esphome::sensor::Sensor* in_adc)
    {
        this->in_adc_ = in_adc;
    }

    void set_manual_override_in(esphome::number::Number* in)
    {
        this->manual_override_ = in;
    }

    void set_value(float value, bool automation)
    {
        value = clampf(value, 0.0, 100.0);

        this->out_value_ = value;

        float out_state = this->in_adc_->get_state() * 31.95;
        // if (tally > 1000)
        // {
        //     ESP_LOGI("blender valve", "out_state: %f", out_state);
        //     tally = 0;
        // }
        // ++tally;

        if (automation)
        {
            write_output(value, lower_limit_, upper_limit_);
            if (defrost_state_ != DefrostState::OFF)
            {
                defrost_state_ = DefrostState::INIT;
            }
            // ESP_LOGI("blender", "DefrostState::INIT");
        }
        else
        {
            float v = 0;
            switch (defrost_state_)
            {
            case DefrostState::OFF: break;
            default:
            case DefrostState::INIT:
                //
                v = 0;
                if (out_state < 20)
                {
                    defrost_state_ = DefrostState::ACTIVE;
                    ESP_LOGI("blender", "DefrostState::ACTIVE");
                }
                break;
            case DefrostState::ACTIVE:
                //
                v = defrost_value_;
                break;
            }
            write_output(v, 0, 100);
        }
    }

    void start_calibration()
    {
        this->calibration_state_ = CalibrationState::INIT;
        ESP_LOGI("valve", "CalibrationState::INIT;");
    }

    CalibrationState do_calibration()
    {
        switch (this->calibration_state_)
        {
        case CalibrationState::INIT: break;
        case CalibrationState::ACTIVE: break;
        case CalibrationState::IDLE: break;
        default: break;
        }

        // Untill this feature is actually implemented
        this->calibration_state_ = CalibrationState::IDLE;
        return this->calibration_state_;
    }

    void write_output(float value, float ll, float ul)
    {
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
        if (enable)
        {
            if (defrost_state_ == DefrostState::OFF)
            {
                defrost_state_ = DefrostState::INIT;
            }
        }
        else
        {
            defrost_state_ = DefrostState::OFF;
        }
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
    void set_calibrate_in(esphome::switch_::Switch* in)
    {
        this->calibrate_in_ = in;
    }

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
    esphome::switch_::Switch* calibrate_in_     = nullptr;

    Valve hot_valve_;
    Valve cold_valve_;

    bool calibrate_ = false;
};

} // namespace blender
} // namespace esphome
