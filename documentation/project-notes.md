# Final Project Notes

## LED Matrix
- The LED Matrix Dimensions will be (21 x 7) (W x H)
    - Total pixels = 147
- Time to display 1 frame = T(Frame) = T(Pixel) * 147 = 4.41ms
- Maximum Frame Rate = 226.75 FPS 

### LED IC (WS2812B)
- [Documentation](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)
- Bit Timing:
    - Total Bit Timing = T(Bit) = 1.25us
    - Wave form (0) = 0.4us +/- 150ns high, 0.85us +/- 150ns low
    - Wave form (1) = 0.8us +/- 150ns high, 0.45us +/- 150ns low 
    - Wave form (RESET) = >50us low 

- Singular pixel timing:
    - 1 Pixel = 3 Byte = 24 Bits
    - T(Pixel) = 24 * T(Bit) = 30us

- Data is in order GRB

## Flash Memory (W25Q64JV)
- [Documentation Download](https://www.alldatasheet.com/datasheet-pdf/pdf/1243804/WINBOND/25Q64JVSIQ.html)
- Max capacity: 4MB

## ESP-32 (30-pin)
- [Pin Out](https://arduinokitproject.com/esp32-pinout-reference/)

### Clock Rate for WS2812B
- Max Wave Form (0) = 550ns high, 1000ns low
- Min Wave Form (0) = 250ns high, 700ns low
- Max Wave Form (1) = 950ns high, 600ns low
- Min Wave Form (1) = 650ns high, 300ns low
- Wave Form Error = 300ns  

Clock Frequency Freq(CLK) = 40MHz  
Period(CLK) = 25ns  

- T(Trans0) = Transmit Bit (0) = 400ns high, 800ns low = 1200ns Total
- T(Trans1) = Transmit Bit (1) = 800ns high, 400ns low = 1200ns Total  

- Clock count max value = 400ns/25ns = 16  

Psudo-Code:
```text
waveform_section = 0
trans_bit_index = 0

trans_bit_array = [0, 1, 1, 0, 0, 1, 0, 1]

def interupt_handler():
    if(waveform_section == 0):
        OUT_PIN = 1

    elif(waveform_section == 1):
        OUT_PIN = trans_bit_array[trans_bit_index]

    elif(waveform_section == 1):
        OUT_PIN = trans_bit_array[trans_bit_index]

    elif(waveform_section == 2):
        OUT_PIN = 0
        trans_bit_index += 1
    
    waveform_section = (waveform_section + 1) % 3
```
