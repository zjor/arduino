#ifndef __HUE_LED__
#define __HUE_LED__

#define R_CH 0
#define G_CH 1
#define B_CH 2

typedef struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

typedef struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;

class HueLed {
  public:
    HueLed(int r_pin, int g_pin, int b_pin);
    void init();
    void set_hsv(int h, int s, int v);
    void set_rgb(int r, int g, int b);
  private:
    int r_pin;
    int g_pin;
    int b_pin;
};

#endif