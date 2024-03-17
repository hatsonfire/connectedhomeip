/*
 *
 *    Copyright (c) 2021-2023 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "LEDWidget.h"
#include "ColorFormat.h"
#if CONFIG_HAVE_DISPLAY
#include "ScreenManager.h"
#endif
#include "led_strip.h"

static const char TAG[] = "LEDWidget";

void LEDWidget::Init(void)
{
    mState      = false;
    mBrightness = UINT8_MAX;

    rmt_config_t config             = RMT_DEFAULT_CONFIG_TX((gpio_num_t) CONFIG_LED_GPIO, (rmt_channel_t) CONFIG_LED_RMT_CHANNEL);
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(100, (led_strip_dev_t) config.channel);

    config.clk_div = 2;
    rmt_config(&config);
    rmt_driver_install(config.channel, 0, 0);

    mStrip      = led_strip_new_rmt_ws2812(&strip_config);
    mHue        = 0;
    mSaturation = 0;
}

void LEDWidget::Set(bool state)
{
    ESP_LOGI(TAG, "Setting state to %d", state ? 1 : 0);
    if (state == mState)
        return;

    mState = state;

    DoSet();
}

void LEDWidget::Toggle()
{
    ESP_LOGI(TAG, "Toggling state to %d", !mState);
    mState = !mState;

    DoSet();
}

void LEDWidget::SetBrightness(uint8_t brightness)
{
    ESP_LOGI(TAG, "Setting brightness to %d", brightness);
    if (brightness == mBrightness)
        return;

    mBrightness = brightness;

    DoSet();
}

uint8_t LEDWidget::GetLevel()
{
    return this->mBrightness;
}

bool LEDWidget::IsTurnedOn()
{
    return this->mState;
}

void LEDWidget::SetColor(uint8_t Hue, uint8_t Saturation)
{
    if (Hue == mHue && Saturation == mSaturation)
        return;

    mHue        = Hue;
    mSaturation = Saturation;

    DoSet();
}

void LEDWidget::DoSet(void)
{
    uint8_t brightness = mState ? mBrightness : 0;

    if (mStrip)
    {
        HsvColor_t hsv = { mHue, mSaturation, brightness };
        RgbColor_t rgb = HsvToRgb(hsv);
        mStrip->set_pixel(mStrip, 0, rgb.r, rgb.g, rgb.b);
        mStrip->refresh(mStrip, 100);
    }
#if CONFIG_HAVE_DISPLAY
    if (mVirtualLEDIndex != -1)
    {
        ScreenManager::SetVLED(mVirtualLEDIndex, mState);
    }
#endif // CONFIG_HAVE_DISPLAY
}

#if CONFIG_DEVICE_TYPE_M5STACK
void LEDWidget::SetVLED(int id1)
{
    mVirtualLEDIndex = id1;
    if (mVirtualLEDIndex != -1)
    {
        ScreenManager::SetVLED(mVirtualLEDIndex, mState);
    }
}
#endif
