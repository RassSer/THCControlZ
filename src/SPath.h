#ifndef _SPATH_H
#define _SPATH_H

#include <Arduino.h>

class SCurveSmoother
{
private:
    float target;
    float current;


    float vel;
    unsigned long lastTime;

public:
    float maxSpeed;
    float acceleration;
    SCurveSmoother(float startValue = 0, float maxVel = 100.0, float accel = 50)
        : target(startValue), current(startValue), maxSpeed(maxVel), acceleration(accel), vel(0)
    {
        lastTime = micros();
    }

    void setTarget(float newTarget)
    {
        target = newTarget;
    }

    float getCurrent()
    {
        update();
        return current;
    }

    bool isTargetReached(float epsilon = 1.0)
    {
        return fabs(current - target) < epsilon;
    }

private:
    void update()
    {
        unsigned long now = micros();
        float dt = (now - lastTime) * 1e-6f;
        if (dt > 0.01f)
            dt = 0.01f;
        lastTime = now;

        float error = target - current;
        float distance = fabs(error);
        float direction = (error > 0) ? 1.0f : -1.0f;

        // ===== ГЛАВНОЕ ПРАВИЛО - НИКОГДА НЕ ПЕРЕЛЕТАТЬ =====
        // 1. Сначала проверяем, не слишком ли мы близко к цели
        if (distance < 1.0f)
        {
            current = target;
            vel = 0;
            return;
        }

        // 2. Рассчитываем максимальную скорость, с которой мы можем
        //    безопасно затормозить на оставшемся расстоянии
        float maxBrakingSpeed = sqrtf(10.0f * acceleration * distance * 0.95f)*10;

        // 3. Целевая скорость - не больше безопасной и не больше максимальной
        float targetVel = direction * min(maxSpeed, maxBrakingSpeed);

        // 4. Плавно меняем скорость с заданным ускорением
        float velDiff = targetVel - vel;
        float maxChange = acceleration * dt;

        if (velDiff > maxChange)
        {
            vel += maxChange;
        }
        else if (velDiff < -maxChange)
        {
            vel -= maxChange;
        }
        else
        {
            vel = targetVel;
        }

        // 5. Обновляем позицию
        float newCurrent = current + vel * dt;

        // 6. Финальная проверка - не перелетели ли
        if ((direction > 0 && newCurrent > target) || (direction < 0 && newCurrent < target))
        {
            current = target;
            vel = 0;
        }
        else
        {
            current = newCurrent;
        }
    }
};

#endif