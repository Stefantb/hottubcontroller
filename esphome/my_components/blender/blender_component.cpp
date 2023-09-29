#include "blender_component.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome
{
namespace blender
{

static const char* TAG = "blender";

//*****************************************************************************
//
//*****************************************************************************
BlenderComponent::BlenderComponent()
{
}

//*****************************************************************************
//
//*****************************************************************************
BlenderComponent::~BlenderComponent()
{
}

//*****************************************************************************
//
//*****************************************************************************
void BlenderComponent::setup()
{
}

//*****************************************************************************
//
//*****************************************************************************
void BlenderComponent::loop()
{
    static uint32_t tally;
    if (tally > 1000)
    {
        float flow_strength = this->flow_strength_in_->state;
        ESP_LOGI(TAG, "flow strength %f", flow_strength);

        climate::ClimateMode mode = this->control_in_->mode;
        ESP_LOGI(TAG, "climate mode %s", climate::climate_mode_to_string(mode));

        float pid_out = this->control_in_->get_output_value();
        ESP_LOGI(TAG, "pid out %f", pid_out);

        ESP_LOGI(TAG, "hot out %f", this->hot_valve_.get_value());
        ESP_LOGI(TAG, "cold out %f", this->cold_valve_.get_value());

        tally = 0;
    }
    ++tally;

    bool is_defrost = this->defrost_in_->state;
    this->hot_valve_.defrost(is_defrost);


    bool is_manual = this->manual_in_->state;
    if (is_manual)
    {
        this->hot_valve_.use_manual();
        this->cold_valve_.use_manual();
    }
    else
    {
        climate::ClimateMode mode = this->control_in_->mode;
        if (mode != climate::CLIMATE_MODE_OFF)
        {
            float flow_strength = this->flow_strength_in_->state;
            float pid_out       = this->control_in_->get_output_value();
            this->adjust_outputs(pid_out, flow_strength, true);
        }
        else
        {
            this->adjust_outputs(0, 0, false);
        }
    }
}

//*****************************************************************************
// -1 <= ctrl <= 1
// 0 < flow <= 100
//*****************************************************************************
void BlenderComponent::adjust_outputs(float ctrl, float flow, bool automation)
{
    ctrl = clampf(ctrl, -1.0, 1.0);
    flow = clampf(flow, 0.0, 100.0);

    float hot  = flow + flow * ctrl;
    float cold = flow - flow * ctrl;

    if (hot > 100)
    {
        hot = 100;
    }
    if (cold > 100)
    {
        cold = 100;
    }

    this->hot_valve_.set_value(hot, automation);
    this->cold_valve_.set_value(cold, automation);
}

//*****************************************************************************
//
//*****************************************************************************
void BlenderComponent::dump_config()
{
    // ESP_LOGCONFIG(TAG, "TP-Link Plugs:");
    // ESP_LOGCONFIG(TAG, "  Plugs: %d", this->plugs_.size());
}

} // namespace blender
} // namespace esphome
