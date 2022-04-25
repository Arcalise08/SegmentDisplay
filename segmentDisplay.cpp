#include <pico/stdlib.h>
#include <string>
#include <map>
#include "hardware/spi.h"
#include <algorithm>
#include <math.h>

#define ON 1
#define OFF 0

#define pin_pico_led 25
#define pin_buzzer 4
#define pin_sign_pwr 6
#define pin_toggle 2
#define pin_decrement 1
#define pin_increment 0


#define digit1 0x01
#define digit2 0x02
#define digit3 0x03
#define digit4 0x04
#define digit5 0x05
#define digit6 0x06
#define digit7 0x07
#define digit8 0x08


#define zero 0x7E
#define one 0x30
#define two 0x6D
#define three 0x79
#define four 0x33
#define five 0x5B
#define six 0x5F
#define seven 0x70
#define eight 0x7F
#define nine 0x7B
#define line 0x01
#define charE 0x4F
#define charH 0x37
#define charL 0x0E
#define charP 0x67
#define blank 0x00

#define DEFAULT_COUNTER 180

std::map<int, int> registerMap {
    {0, digit1},
    {1, digit2},
    {2, digit3},
    {3, digit4},
    {4, digit5},
    {5, digit6},
    {6, digit7},
    {7, digit8},
};
std::map<int, int> numMap {
    {0, zero},
    {1, one},
    {2, two},
    {3, three},
    {4, four},
    {5, five},
    {6, six},
    {7, seven},
    {8, eight},
    {9, nine},
};

uint16_t counter = DEFAULT_COUNTER;
bool handlingClick = false;
bool cancelledTimer = false;

void padLeft(std::string &str, const size_t num, const char paddingChar = ' ')
{
    if(num > str.size())
        str.insert(0, num - str.size(), paddingChar);
}

void padRight(std::string &str, const size_t num, const char paddingChar = ' ') {
    if (str.size() < num)
        str.insert(str.size(), num - str.size(), paddingChar);
}


void setupDisplay() {
    uint16_t data[1];
    data[0] = 0x0C01;
    //data[1] = 0x9000;
    data[1] = 0x0B07;
    spi_write16_blocking(spi0, data, 2);
    sleep_ms(30);
    
}
void writeToReg(uint16_t reg, uint16_t tb)  {
    uint16_t data = (reg << 8)|(tb);
    spi_write16_blocking(spi0, &data, 1);
}

void writeAll(uint16_t tb)  {
    uint16_t data[7];
    data[0] = (digit1 << 8)|(tb);
    data[1] = (digit2 << 8)|(tb);
    data[2] = (digit3 << 8)|(tb);
    data[3] = (digit4 << 8)|(tb);
    data[4] = (digit5 << 8)|(tb);
    data[5] = (digit6 << 8)|(tb);
    data[6] = (digit7 << 8)|(tb);
    data[7] = (digit8 << 8)|(tb);
    spi_write16_blocking(spi0, data, 8);
}

std::string counterToTime() {
    if (counter < 60) {
        std::string count = std::to_string(counter);
        padLeft(count, 8, '0');
        std::reverse(count.begin(), count.end());
        return count;
    }
    
    uint16_t minutes = counter / 60;
    uint16_t seconds = counter - (floor(minutes * 60));
    std::string m = std::to_string(minutes);
    std::string s = std::to_string(seconds);
    padLeft(s, 2, '0');
    m.append(s);
    padLeft(m, 8, '0');
    std::reverse(m.begin(), m.end());
    return m;
}

void displayCounter() {
    std::string count = counterToTime();

    for (int i=0; i < 8; i++) {
        char c = count[i];
        uint16_t bt = (uint16_t)strtol(&c, NULL, 0);
        writeToReg(registerMap[i], numMap[bt] );
    }
    
}

void partyTime(int timesRan = 0) {
    int i = 0;

    while (i < 2) {
        gpio_put(pin_buzzer, ON);
        writeAll(numMap[0]);
        sleep_ms(350);
        gpio_put(pin_buzzer, OFF);
        writeAll(blank);
        sleep_ms(150);
        i++;
    }
    if (timesRan > 5) {
        writeAll(line);
        return;
    }
    sleep_ms(1000);
    partyTime(timesRan + 1);

}


void startCountDown() {
    while (counter > 0) {
        displayCounter();
        int sleepTime = 1000;
        bool cancel = false;
        while (sleepTime != 0) {
            sleep_ms(1);
            sleepTime--;
            if (gpio_get(pin_toggle) == 1) {
                cancel = true;
                break;
            }
        }
        if (cancel)
        {
            cancelledTimer = true;
            writeAll(line);
            break;
        }
        counter--;
    }
    if (!cancelledTimer)
        partyTime();
}

int main() {
    stdio_init_all();
    spi_init(spi_default, 500000);

    spi_set_format(spi0, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );
    gpio_init(pin_pico_led);
    gpio_init(pin_sign_pwr);
    gpio_init(pin_buzzer);
    gpio_init(pin_decrement);
    gpio_init(pin_increment);
    gpio_init(pin_toggle);

    gpio_init(PICO_DEFAULT_SPI_RX_PIN);
    gpio_init(PICO_DEFAULT_SPI_SCK_PIN);
    gpio_init(PICO_DEFAULT_SPI_TX_PIN);
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    
    gpio_set_dir(pin_pico_led, GPIO_OUT);
    gpio_set_dir(pin_sign_pwr, GPIO_OUT);
    gpio_set_dir(pin_buzzer, GPIO_OUT);

    gpio_set_dir(pin_decrement, GPIO_IN);
    gpio_set_dir(pin_increment, GPIO_IN);
    gpio_set_dir(pin_toggle, GPIO_IN);


    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);

    gpio_put(pin_pico_led, ON);
    gpio_put(pin_sign_pwr, OFF);
    gpio_put(pin_buzzer, OFF);

    sleep_ms(100);
    gpio_put(pin_sign_pwr, ON);
    sleep_ms(100);
    setupDisplay();
    bool alreadyIncremented = false;
    bool alreadyDecremented = false;
    displayCounter();
    while (true) {
        bool handledClick = false;
        if (cancelledTimer) {
            sleep_ms(1000);
            counter = DEFAULT_COUNTER;
            cancelledTimer = false;
        }
        if (gpio_get(pin_toggle) == 1 && !cancelledTimer) {
            while (gpio_get(pin_toggle) == 1) {}
            startCountDown();
            handledClick = true;
        }

        if (gpio_get(pin_decrement) == 1) {
            if (counter - 30 < 0 || alreadyDecremented)
                continue;
            counter -= 30;
            handledClick = true;
            alreadyDecremented = true;
            displayCounter();
        }
        else {
            alreadyDecremented = false;
        }

        if (gpio_get(pin_increment) == 1) {
            if (alreadyIncremented)
                continue;

            counter += 30;
            handledClick = true;
            alreadyIncremented = true;
            displayCounter();
        }
        else {
            alreadyIncremented = false;
        }
        if (!handledClick) {

        }
        sleep_ms(50);
    }
}
