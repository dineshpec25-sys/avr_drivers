#include "../include/seg7.h"
#include "../include/gpio.h"

static seg7_map_t seg7;

void seg7_init(seg7_map_t map)
{
    seg7 = map;

    gpio_mode(seg7.a.port, seg7.a.pin, GPIO_OUTPUT);
    gpio_mode(seg7.b.port, seg7.b.pin, GPIO_OUTPUT);
    gpio_mode(seg7.c.port, seg7.c.pin, GPIO_OUTPUT);
    gpio_mode(seg7.d.port, seg7.d.pin, GPIO_OUTPUT);
    gpio_mode(seg7.e.port, seg7.e.pin, GPIO_OUTPUT);
    gpio_mode(seg7.f.port, seg7.f.pin, GPIO_OUTPUT);
    gpio_mode(seg7.g.port, seg7.g.pin, GPIO_OUTPUT);

    seg7_clear();
}

void seg7_clear(void)
{
    gpio_write(seg7.a.port, seg7.a.pin, LOW);
    gpio_write(seg7.b.port, seg7.b.pin, LOW);
    gpio_write(seg7.c.port, seg7.c.pin, LOW);
    gpio_write(seg7.d.port, seg7.d.pin, LOW);
    gpio_write(seg7.e.port, seg7.e.pin, LOW);
    gpio_write(seg7.f.port, seg7.f.pin, LOW);
    gpio_write(seg7.g.port, seg7.g.pin, LOW);
}

static void zero()
{
	gpio_write(seg7.a.port, seg7.a.pin, HIGH);
        gpio_write(seg7.b.port, seg7.b.pin, HIGH);
        gpio_write(seg7.c.port, seg7.c.pin, HIGH);
        gpio_write(seg7.d.port, seg7.d.pin, HIGH);
        gpio_write(seg7.e.port, seg7.e.pin, HIGH);
        gpio_write(seg7.f.port, seg7.f.pin, HIGH);
}

static void one()
{
	gpio_write(seg7.b.port, seg7.b.pin, HIGH);
        gpio_write(seg7.c.port, seg7.c.pin, HIGH);
}

static void two()
{
	gpio_write(seg7.a.port, seg7.a.pin, HIGH);
	gpio_write(seg7.b.port, seg7.b.pin, HIGH);
        gpio_write(seg7.d.port, seg7.d.pin, HIGH);
        gpio_write(seg7.e.port, seg7.e.pin, HIGH);
        gpio_write(seg7.g.port, seg7.g.pin, HIGH);
}

static void three()
{
	gpio_write(seg7.a.port, seg7.a.pin, HIGH);
        gpio_write(seg7.b.port, seg7.b.pin, HIGH);
        gpio_write(seg7.c.port, seg7.c.pin, HIGH);
        gpio_write(seg7.d.port, seg7.d.pin, HIGH);
        gpio_write(seg7.g.port, seg7.g.pin, HIGH);
}

static void four()
{
	gpio_write(seg7.b.port, seg7.b.pin, HIGH);
        gpio_write(seg7.c.port, seg7.c.pin, HIGH);
        gpio_write(seg7.f.port, seg7.f.pin, HIGH);
        gpio_write(seg7.g.port, seg7.g.pin, HIGH);
}

static void five()
{
	gpio_write(seg7.a.port, seg7.a.pin, HIGH);
        gpio_write(seg7.c.port, seg7.c.pin, HIGH);
        gpio_write(seg7.d.port, seg7.d.pin, HIGH);
        gpio_write(seg7.f.port, seg7.f.pin, HIGH);
        gpio_write(seg7.g.port, seg7.g.pin, HIGH);
}

static void six()
{
	gpio_write(seg7.a.port, seg7.a.pin, HIGH);
        gpio_write(seg7.c.port, seg7.c.pin, HIGH);
        gpio_write(seg7.d.port, seg7.d.pin, HIGH);
        gpio_write(seg7.e.port, seg7.e.pin, HIGH);
        gpio_write(seg7.f.port, seg7.f.pin, HIGH);
        gpio_write(seg7.g.port, seg7.g.pin, HIGH);
}

static void seven()
{
	gpio_write(seg7.a.port, seg7.a.pin, HIGH);
        gpio_write(seg7.b.port, seg7.b.pin, HIGH);
	gpio_write(seg7.c.port, seg7.c.pin, HIGH);
}

static void eight()
{
	gpio_write(seg7.a.port, seg7.a.pin, HIGH);
        gpio_write(seg7.b.port, seg7.b.pin, HIGH);
        gpio_write(seg7.c.port, seg7.c.pin, HIGH);
        gpio_write(seg7.d.port, seg7.d.pin, HIGH);
        gpio_write(seg7.e.port, seg7.e.pin, HIGH);
        gpio_write(seg7.f.port, seg7.f.pin, HIGH);
        gpio_write(seg7.g.port, seg7.g.pin, HIGH);
}

static void nine()
{
	gpio_write(seg7.a.port, seg7.a.pin, HIGH);
        gpio_write(seg7.b.port, seg7.b.pin, HIGH);
        gpio_write(seg7.c.port, seg7.c.pin, HIGH);
        gpio_write(seg7.d.port, seg7.d.pin, HIGH);
        gpio_write(seg7.f.port, seg7.f.pin, HIGH);
        gpio_write(seg7.g.port, seg7.g.pin, HIGH);
}

static void seg7_display(uint8_t num)
{
    seg7_clear();

    switch (num)
    {
        case 0: zero(); break;
        case 1: one(); break;
        case 2: two(); break;
        case 3: three(); break;
        case 4: four(); break;
        case 5: five(); break;
        case 6: six(); break;
	case 7:	seven(); break;
        case 8: eight(); break;
        case 9: nine(); break;
        default: seg7_clear(); break;
    }
}
