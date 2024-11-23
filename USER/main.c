#include <string.h>
#include "stm32f10x.h"
#include "delay.h"
#include "serial.h"
#include "pwm.h"
#include "led.h"

#include "midi.h"

#define MIDI_MAGIC 0xbeefu

void decodeHeader(uint8_t byte);
void decodePayload(uint8_t byte);
void buzzerPlay(uint32_t us, uint32_t frequency, uint32_t duty);
void onMidiEvent(midi_context_t *ctx, midi_event_t *event);
void onMidiComplete(midi_context_t *ctx);

typedef struct {
    uint16_t magic;
    uint8_t seqid;
    uint8_t channel_id;
    uint8_t payload_size;
} __attribute__((packed)) MidiHeader;

typedef struct {
    MidiHeader header;
    uint8_t payload[32];
} __attribute__((packed)) MidiMessage;

uint8_t gHasNewMessage = 0;
uint8_t gDecodeLen = 0;
MidiMessage gMessage = {0};
OnReadableFunc gDecodeFunc = 0;
midi_context_t gMidiCtx = {0};

void decodeHeader(uint8_t byte)
{
    uint8_t *ptr = (uint8_t *)&(gMessage.header);
    
    ptr[gDecodeLen++] = byte;
    if (gDecodeLen < sizeof(MidiHeader)) {
        return;
    }

    while (gMessage.header.magic != (uint16_t)MIDI_MAGIC) ;

    gDecodeLen = 0;
    gDecodeFunc = decodePayload;
}

void decodePayload(uint8_t byte)
{
    uint8_t payload_size = gMessage.header.payload_size;
    uint8_t *ptr = gMessage.payload;

    ptr[gDecodeLen++] = byte;
    if (gDecodeLen < payload_size) {
        return;
    }

    gDecodeLen = 0;
    gDecodeFunc = decodeHeader;

    gHasNewMessage = 1;
}

void onReadable(uint8_t byte)
{
    gDecodeFunc(byte);
}

void buzzerPlay(uint32_t us, uint32_t frequency, uint32_t duty)
{
    delay_us(us);

    // the sysclock is 72MHz,
    // and prescaler=72 is set in pwm.c,
    // so we use 1000000 = 72MHz/72
    uint32_t period = 1000000 / frequency;
    PWM_SetAutoreload(period - 1);
    uint32_t compareValue = (period * duty) / 100;
    PWM_SetCompare1(compareValue);
}

void onMidiEvent(midi_context_t *ctx, midi_event_t *event)
{
    // simpler is better:
    // just focus on&off, ignore others
    // just play one channel, ignore others

    uint8_t channel = event->status & 0x0f;
    if (channel != gMessage.header.channel_id) {
        return;
    }
    
    uint32_t delta = event->delta;

    uint8_t type = event->status & 0xf0;
    if (type == NOTE_ON || type == NOTE_OFF) {
        uint32_t freq = midi_note_to_freq(event->param1);
        // the max duty is 127 in MIDI
        uint32_t duty = event->param2;
        if (duty > 127) {
            duty = 127;
        } else if (type == NOTE_OFF) {
            duty = 0;
        }
        duty = duty / 127. * 100;
        buzzerPlay(delta, freq, duty / 3);
    } else {
        delay_us(delta);
    }
}

void onMidiComplete(midi_context_t *ctx)
{
    gDecodeLen = 0;
    gDecodeFunc = decodeHeader;

    memset(ctx, 0, sizeof(*ctx));
    ctx->on_event = onMidiEvent;
    ctx->on_complete = onMidiComplete;
}

int main(void)
{
    LED_Init();

    uint8_t i = 0;

    gDecodeFunc = decodeHeader;
    Serial_Init(onReadable);
    PWM_Init();

    gMidiCtx.on_event = onMidiEvent;
    gMidiCtx.on_complete = onMidiComplete;

    // Test C4 Scale Notes
//    int _c[] = {262, 294, 330, 349, 392, 440, 494};
//    for (i = 0; i < sizeof(_c)/sizeof(_c[0]); ++i)
//    {
//        buzzerPlay(300000, _c[i], 30);
//    }
//    int c[] = {523, 587, 659, 698, 784, 880, 988};
//    for (i = 0; i < sizeof(c)/sizeof(c[0]); ++i)
//    {
//        buzzerPlay(300000, c[i], 30);
//    }
//    int c_[] = {1046, 1175, 1318, 1497, 1568, 1760, 1976};
//    for (i = 0; i < sizeof(c_)/sizeof(c_[0]); ++i)
//    {
//        buzzerPlay(300000, c_[i], 30);
//    }
//    buzzerPlay(0, 100, 0);
    
    while (1)
    {
        if (gHasNewMessage) {
            LED_Flash();
            int ret = midi_decode(&gMidiCtx, gMessage.payload, gMessage.header.payload_size);
            while (ret != MIDI_OK);
            gHasNewMessage = 0;
            Serial_SendByte(gMessage.header.seqid);
        }
    }
}
