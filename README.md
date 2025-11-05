# ATMega328PAnalogReader

A lightweight RAII-style ADC controller for **ATmega328P** (e.g., Arduino Uno, Nano) that enables **fast**, **averaged**, and **high-resolution simulated analog readings**.

---

## ✨ Features

- RAII-based ADC configuration (automatically restores previous state)
- Fast analog reads with adjustable ADC prescaler
- Averaging for noise reduction
- Simulated higher resolution (up to 16-bit) via oversampling
- All methods are inline for **zero call overhead**

---

## 🚀 Example

```cpp
#include "ATMega328PAnalogReader.h"

ATMega328PAnalogReader adc;

void setup() {
    Serial.begin(115200);
    adc.begin(4); // Prescaler ÷8 (2 MHz ADC clock)
}

void loop() {
    uint16_t v1 = adc.read(A0);                      // single read
    uint16_t v2 = adc.readAveraged<4>(A0);           // average of 16 reads
    uint16_t v3 = adc.readHighRes<12>(A0);           // 12-bit simulated
    uint16_t v4 = adc.readHighResAveraged<12,2>(A0); // 12-bit + avg(4×)

    Serial.println(v4);
    delay(500);
}
```

---

## ⚙️ ADC Prescaler Reference (ATmega328P @16 MHz)

| PRESCALER_BITS | Divider | ADC Clock | Notes |
|----------------|----------|------------|--------|
| 2 | ÷2 | 8 MHz | Ultra-fast, low accuracy |
| 3 | ÷4 | 4 MHz | Very fast |
| 4 | ÷8 | 2 MHz | Good balance |
| 5 | ÷16 | 1 MHz | Accurate, moderate speed |
| 6 | ÷32 | 500 kHz | High accuracy |
| 7 | ÷64 | 250 kHz | Very accurate (default) |

---

## 🧠 Class Overview

### `class ATMega328PAnalogReader`

| Method | Description |
|---------|-------------|
| `void begin(uint8_t prescalerBits = 7)` | Initializes the ADC with a custom prescaler. |
| `void end()` | Restores previous ADC configuration. |
| `uint16_t read(uint8_t pin)` | Fast single analog read. |
| `readAveraged<AVG_POW2>(uint8_t pin)` | Performs 2^AVG_POW2 averaged readings. |
| `readHighRes<OUTPUT_BITS>(uint8_t pin)` | Simulates higher resolution (10–16 bits). |
| `readHighResAveraged<OUTPUT_BITS,AVG_POW2>(uint8_t pin)` | Combines oversampling and averaging. |

---

## 🧾 License

MIT License © 2025 Your Name
