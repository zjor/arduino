#ifndef PID_H
#define PID_H

class PID {
    public:
        PID(float Kp, float Kd, float Ki, float target);
        PID(float Kp, float Kd, float Ki, float target, float min_u, float max_u);
        float getControl(float value, float dt_seconds);
        void setTarget(float target);
    private:
        float _min_u = __FLT_MIN__;
        float _max_u = __FLT_MAX__;
        float _Kp;
        float _Kd;
        float _Ki;
        float _lastError;
        float _integralError;
        float _target;
        float _last_value;
        bool _has_last_value = false;
};

#endif