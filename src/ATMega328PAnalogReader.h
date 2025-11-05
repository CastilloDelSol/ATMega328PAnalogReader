#ifndef ATMEGA328P_ANALOG_READER_H
#define ATMEGA328P_ANALOG_READER_H

#include <Arduino.h>

/*
 * ============================================================================
 *  ATMega328PAnalogReader
 *  RAII-based ADC controller for fast analog reads with optional averaging
 *  and simulated higher-resolution oversampling.
 *
 *  • On begin: backs up the current ADC state and sets a new prescaler.
 *  • On destruction: restores the previous ADC configuration automatically.
 *  • If declared globally, the prescaler remains active permanently.
 *  • All member functions are inline for zero call overhead.
 *
 *  --------------------------------------------------------------------------
 *  Example Usage:
 *
 *      ATMega328PAnalogReader adc(4);  // prescaler ÷8 (2 MHz ADC clock)
 *
 *      void loop() {
 *          uint16_t v1 = adc.read(A0);                      // fast single read
 *          uint16_t v2 = adc.readAveraged<4>(A0);           // average 16 reads
 *          uint16_t v3 = adc.readHighRes<12>(A0);           // 12-bit simulated
 *          uint16_t v4 = adc.readHighResAveraged<12,2>(A0); // 12-bit + avg(4×)
 *          Serial.println(v4);
 *      }
 *
 *  --------------------------------------------------------------------------
 *  ADC Prescaler Reference (ATmega328P @16 MHz)
 *
 *      PRESCALER_BITS | Divider | ADC Clock | Notes
 *      ----------------|----------|------------|----------------------------
 *             2         | ÷2     | 8 MHz     | Ultra-fast, low accuracy
 *             3         | ÷4     | 4 MHz     | Very fast
 *             4         | ÷8     | 2 MHz     | Good balance
 *             5         | ÷16    | 1 MHz     | Accurate, moderate speed
 *             6         | ÷32    | 500 kHz   | High accuracy
 *             7         | ÷64    | 250 kHz   | Very accurate (default)
 * ============================================================================
 */
class ATMega328PAnalogReader {
public:
    ATMega328PAnalogReader() = default;
    ~ATMega328PAnalogReader() { end(); } // Automatically restore ADC on destruction

    /* Initialize and set prescaler (call inside setup()) */
    bool begin(uint8_t prescalerBits = 7)
    {
        _prescalerBits = prescalerBits & 0x07;
        _backupADCSRA = ADCSRA; // Backup current configuration
        ADCSRA = (ADCSRA & 0b11111000) | _prescalerBits;
        _initialized = true;

        return initialized();
    }

    bool initialized() const { return _initialized; }

    /* Restore previous ADC configuration */
    void end()
    {
        if (initialized()) { ADCSRA = _backupADCSRA; }
        _initialized = false;
    }

    /* Perform a single analog read */
    inline uint16_t read(uint8_t pin) const { return analogRead(pin); }

    /* Averaged read (2^AVG_POW2 samples) for noise reduction */
    template<uint8_t AVG_POW2 = 0>
    inline uint16_t readAveraged(uint8_t pin) const
    {
        constexpr uint16_t samples = 1 << AVG_POW2;

        uint32_t sum = 0;
        for (uint16_t i = 0; i < samples; ++i) { sum += analogRead(pin); }

        return sum >> AVG_POW2;
    }

    /* Oversampled read (10–16 bits simulated resolution) */
    template<uint8_t OUTPUT_BITS = 10>
    inline uint16_t readHighRes(uint8_t pin) const
    {
        static_assert(OUTPUT_BITS >= 10 && OUTPUT_BITS <= 16, "OUTPUT_BITS must be between 10 and 16");

        constexpr uint8_t extraBits = OUTPUT_BITS - 10;
        constexpr uint16_t samples = 1 << (2 * extraBits);

        uint32_t sum = 0;
        for (uint16_t i = 0; i < samples; ++i) { sum += analogRead(pin); }

        uint16_t avg = sum >> (2 * extraBits);
        return (avg << extraBits);
    }

    /* Combined oversampling + averaging */
    template<uint8_t OUTPUT_BITS = 10, uint8_t AVG_POW2 = 0>
    inline uint16_t readHighResAveraged(uint8_t pin) const
    {
        static_assert(OUTPUT_BITS >= 10 && OUTPUT_BITS <= 16, "OUTPUT_BITS must be between 10 and 16");
        constexpr uint8_t extraBits = OUTPUT_BITS - 10;
        constexpr uint8_t totalPow2 = (2 * extraBits) + AVG_POW2;
        constexpr uint16_t samples = 1 << totalPow2;

        uint32_t sum = 0;
        for (uint16_t i = 0; i < samples; ++i) { sum += analogRead(pin); }
        uint16_t avg = sum >> totalPow2;
        return (avg << extraBits);
    }

private:
    uint8_t _prescalerBits = 0;   // active prescaler
    uint8_t _backupADCSRA = 0;    // backup of ADC control reg
    bool _initialized = false;    // tracks if begin() has been called
};

#endif // ATMEGA328P_ANALOG_READER_H
