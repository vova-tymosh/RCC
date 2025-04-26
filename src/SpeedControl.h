#pragma once
// #include <ArduPID.h>

class SpeedControl
{
private:
    // ArduPID pid;

    double setpoint;
    double input;
    double output;

public:
    double p;
    double i;
    double d;
    float upper;

    void setP(float value)
    {
        p = value;
        // pid.setCoefficients(p, i, d);
        // pid.reset();
    }

    void setI(float value)
    {
        i = value;
        // pid.setCoefficients(p, i, d);
        // pid.reset();
    }

    void setD(float value)
    {
        d = value;
        // pid.setCoefficients(p, i, d);
        // pid.reset();
    }

    void setUpper(float value)
    {
        this->upper = value;
    }

    void setMeasured(float value)
    {
        this->input = value;
    }

    void setDesired(float value)
    {
        this->setpoint = value;
    }

    float read()
    {
        // pid.compute();
        return this->output;
    }

    float scale(float value)
    {
        value = constrain(value, 0, upper);
        return map(value, 0, upper, 0, 100);
    }

    void begin(float _p, float _i, float _d, float _m)
    {
        p = _p;
        i = _i;
        d = _d;
        upper = _m;
        // pid.begin(&input, &output, &setpoint, p, i, d);
        // pid.setOutputLimits(0, 100);
    }
};
