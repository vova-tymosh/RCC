

#if defined(ARDUINO_ARCH_NRF52)
class Ping
{
public:
    struct __attribute__((packed)) Packet {
        uint8_t code;
        uint16_t id;
        uint32_t ts;
        uint8_t pad[20];
    } packet;

    bool active = false;
    uint16_t recd = 0;
    uint16_t sent = 0;
    uint16_t sentLast = 0;
    uint16_t recdLast = 0;
    float avgTime = 0;
    const int maxCount = 20;
    Transport *transport;
    Timer printTimer;

    Timer shortTimer;
    Timer longTimer;


    void begin(Transport *transport)
    {
        this->transport = transport;
        active = true;
        sent = 0;
        recd = 0;
        shortTimer.start(100);
        longTimer.start(2090);
        printTimer.start(2090);
    }

    void end()
    {
        active = false;
    }

    void send()
    {
        packet.code = '0';
        packet.id = ++sent;
        packet.ts = millis();
        transport->write((uint8_t*)&packet, sizeof(packet));
    }

    void receive(char* value, uint8_t size)
    {
        if (size == sizeof(Packet)) {
            struct Packet *p = (struct Packet *)value;
            if (sent == p->id) {
                recd++;
                long time = millis() - p->ts;
                int c = (p->id > maxCount) ? maxCount : p->id;
                avgTime = (avgTime * c + time) / (c + 1);
                shortTimer.start();
            }
        }
    }

    void loop()
    {
        if (!active)
            return;

        if (shortTimer.hasFiredOnce()) {
            send();
            longTimer.start();
        }

        if (longTimer.hasFiredOnce()) {
            send();
            longTimer.start();
        }

        if (printTimer.hasFired()) {
            Serial.print("Ping avg: ");
            Serial.print(avgTime);
            Serial.print(", rate: ");
            Serial.print(recd - recdLast);
            Serial.print("/");
            Serial.println(sent - sentLast);
            sentLast = sent;
            recdLast = recd;
        }
    }
};
#endif
