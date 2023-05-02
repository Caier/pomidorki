#include "task.h"
#include <Arduino.h>
#include "peripherals.h"

class : public Task { // Button task
    bool prevPres = digitalRead(button);

    void exec() override {
        if(digitalRead(button) && !prevPres)
            digitalWrite(pumps, 1);
        else if(!digitalRead(button) && prevPres)
            digitalWrite(pumps, 0);
        prevPres = digitalRead(button);
    }
} buttonTask;

class : public Task {
    unsigned long timerNonCorrosive = 2000, timerCorrosive = 30000;

    void exec() override {
        timerNonCorrosive += milis - prevMilis;
        timerCorrosive += milis - prevMilis;
        if(timerNonCorrosive > 2000) {
            Measurements.pumpLength = analRead(waterVolume) * 107.42 + 10000;
            Measurements.waterSurface = ultraMeasure();
            timerNonCorrosive = 0;
        }
        if(timerCorrosive > 30000) {
            digitalWrite(meterVcc, 1);
            delay(10);
            Measurements.met1 = analRead(meter1);
            Measurements.met2 = analRead(meter2);
            digitalWrite(meterVcc, 0);
            timerCorrosive = 0;
        }
    }
} measureTask;

class : public Task {
    int length = 0;
    bool toggled = false;

    void exec() override {
        if(length > 0)
            length -= milis - prevMilis;
        else if(toggled) {
            digitalWrite(pumps, 0);
            length = 0;
            toggled = false;
        }
    }
    public: bool pump(int length) {
        if(toggled)
            return 0;
        digitalWrite(pumps, 1);
        this->length = length;
        toggled = true;
        return 1;
    }
} onlinePump;

class : public Task {
    int lastHr = 0;
    unsigned long dangerTimer = 1800000;

    String updateTemplate(bool red = false) {
        config["webhookTemplate"]["embeds"][0]["fields"][0]["value"] = Measurements.met1;
        config["webhookTemplate"]["embeds"][0]["fields"][1]["value"] = Measurements.met2;
        config["webhookTemplate"]["embeds"][0]["color"] = !red ? 6807040 : 14877191;
        String out;
        serializeJson(config["webhookTemplate"], out);
        return out;
    }

    void exec() override {
        if(lastHr != hour() && (hour() == 9 || hour() == 18))
            HTTPpost(config["webhookURL"], updateTemplate());
        if(max(Measurements.met1, Measurements.met2) > (int)config["dangerThreshold"]) {
            dangerTimer += milis - prevMilis;
            if(dangerTimer > 1800000) {
                HTTPpost(config["webhookURL"], updateTemplate(true));
                dangerTimer = 0;
            }
        }
        else
            dangerTimer = 1800000;

        lastHr = hour();
    }
} notifyTask;