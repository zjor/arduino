#include <Arduino.h>

#include "hue_led.h"

HueLed::HueLed(int r_pin, int g_pin, int b_pin) {
  this->r_pin = r_pin;
  this->g_pin = g_pin;
  this->b_pin = b_pin;
}

void HueLed::init() {
  ledcAttachPin(this->r_pin, R_CH);
  ledcAttachPin(this->g_pin, G_CH);
  ledcAttachPin(this->b_pin, B_CH);

  ledcSetup(R_CH, 1000, 8);
  ledcSetup(G_CH, 1000, 8);
  ledcSetup(B_CH, 1000, 8);
}

RgbColor hsv_to_rgb(HsvColor hsv)
{
    RgbColor rgb;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6; 

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            rgb.r = hsv.v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = hsv.v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = hsv.v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = hsv.v;
            break;
        default:
            rgb.r = hsv.v; rgb.g = p; rgb.b = q;
            break;
    }

    return rgb;
}

void HueLed::set_hsv(int h, int s, int v) {
  HsvColor hsv;
  hsv.h = h;
  hsv.s = s;
  hsv.v = v;

  RgbColor rgb = hsv_to_rgb(hsv);

  ledcWrite(R_CH, 255 - rgb.r);
  ledcWrite(G_CH, 255 - rgb.g);
  ledcWrite(B_CH, 255 - rgb.b);
}

void HueLed::set_rgb(int r, int g, int b) {
  ledcWrite(R_CH, 255 - r);
  ledcWrite(G_CH, 255 - g);
  ledcWrite(B_CH, 255 - b);
}