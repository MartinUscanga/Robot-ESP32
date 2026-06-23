# 🔌 Diagrama de Conexión ESP32 + ST7789 (Configuración Personalizada)

## 📐 Esquema de Conexión

```
                     ESP32 DevKit
                  ┌───────────────┐
                  │               │
           3.3V ──┤ 3.3V      VIN ├── (Opcional: 5V externa)
                  │               │
            GND ──┤ GND       GND ├── GND
                  │               │
                  │           EN  │
                  │          IO36 │
                  │          IO39 │
                  │          IO34 │
                  │          IO35 │
                  │          IO32 │
                  │          IO33 │
                  │          IO25 │
                  │          IO26 │
                  │          IO27 │
     ST7789 SCK ──┤ IO14     IO14 │ ⬅️ SCK/SCL
     ST7789 MOSI ─┤ IO13     IO13 │ ⬅️ MOSI/SDA
     ST7789 RST ──┤ IO12     IO12 │ ⬅️ RESET
     ST7789 DC  ──┤ IO11     IO11 │ ⬅️ DC (Data/Command)
     ST7789 CS  ──┤ IO10     IO10 │ ⬅️ CS (Chip Select)
                  │          IO9  │
                  │          IO8  │
                  │          IO7  │
                  │          IO6  │
                  │          IO5  │
                  │          IO4  │
                  │          IO3  │
                  │          IO2  │
                  │          IO1  │
                  │          IO0  │
                  │          RX   │
                  │          TX   │
                  └───────────────┘


                   ST7789 Display
                  ┌───────────────┐
                  │   240x240     │
                  │               │
                  │   [SCREEN]    │
                  │               │
                  └───────┬───────┘
                          │
                  ┌───────┴───────┐
                  │ VCC  (3.3V)   ├── ESP32 3.3V
                  │ GND           ├── ESP32 GND
                  │ CS            ├── ESP32 GPIO 10
                  │ DC            ├── ESP32 GPIO 11
                  │ RESET         ├── ESP32 GPIO 12
                  │ SDA/MOSI      ├── ESP32 GPIO 13
                  │ SCL/SCK       ├── ESP32 GPIO 14
                  │ BLK           ├── 3.3V (o GPIO 4 para PWM)
                  └───────────────┘
```

---

## 📋 Tabla de Conexiones

| ST7789 Pin | ESP32 GPIO | Función | Notas |
|------------|------------|---------|-------|
| **VCC** | 3.3V | Alimentación | ⚠️ **NUNCA 5V** - puede dañar el display |
| **GND** | GND | Tierra | Debe ser GND común con ESP32 |
| **CS** | **GPIO 10** | Chip Select | Selecciona el dispositivo SPI |
| **DC** | **GPIO 11** | Data/Command | Alterna entre datos y comandos |
| **RST** | **GPIO 12** | Reset | Reinicia el display (activo bajo) |
| **SDA/MOSI** | **GPIO 13** | Master Out | Envía datos del ESP32 al display |
| **SCL/SCK** | **GPIO 14** | Clock | Señal de reloj SPI |
| **BLK** | 3.3V | Backlight | Retroiluminación (siempre ON) |

---

## ⚙️ Configuración en Código

```cpp
// Pines personalizados
#define TFT_CS    10  // Chip Select
#define TFT_DC    11  // Data/Command
#define TFT_RST   12  // Reset
#define TFT_MOSI  13  // MOSI (SDA)
#define TFT_SCLK  14  // Clock (SCK/SCL)

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Inicializar SPI con tus pines personalizados
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  //         SCK=14   MISO MOSI=13  CS=10
  
  SPI.setFrequency(27000000); // 27MHz - estable
  
  // Inicializar display
  tft.init(240, 240, SPI_MODE2);
  delay(100);
}
```

---

## ⚠️ IMPORTANTE - Diferencias con Configuración Estándar

### ✅ Ventajas de Estos Pines

**Pines GPIO 10-14:**
- ✅ Disponibles en todos los modelos de ESP32
- ✅ Agrupados físicamente (fácil de cablear)
- ✅ No interfieren con GPIOs de boot (GPIO 0, 2, 15)
- ✅ Configuración limpia y organizada

### ⚠️ Consideraciones

**Pines VSPI estándar del ESP32:**
- VSPI MISO = GPIO 19 (no usado)
- VSPI MOSI = GPIO 23 (estándar, tú usas GPIO 13)
- VSPI SCK = GPIO 18 (estándar, tú usas GPIO 14)
- VSPI CS = GPIO 5 (estándar, tú usas GPIO 10)

**Tu configuración es válida porque:**
- La librería Adafruit_ST7789 permite usar **cualquier GPIO** para SPI
- El ESP32 tiene hardware SPI flexible (no limitado a pines fijos)
- Al llamar `SPI.begin(SCK, MISO, MOSI, CS)` reconfiguras los pines

---

## 🛠️ Verificación de Pines

Antes de conectar, verifica que tu modelo de ESP32 tiene estos GPIOs:

| GPIO | Disponible en | Notas |
|------|---------------|-------|
| GPIO 10 | ✅ ESP32 DevKit | OK |
| GPIO 11 | ✅ ESP32 DevKit | OK |
| GPIO 12 | ✅ ESP32 DevKit | OK, evitar si usas JTAG |
| GPIO 13 | ✅ ESP32 DevKit | OK |
| GPIO 14 | ✅ ESP32 DevKit | OK |

**⚠️ Modelos especiales:**
- ESP32-C3: Solo tiene GPIOs 0-21 (estos pines están OK)
- ESP32-S2: Verifica pinout específico
- ESP32-WROOM: ✅ Todos estos pines disponibles

---

## 📸 Diagrama Visual Simplificado

```
    ESP32                    ST7789
    ┌────┐                  ┌──────┐
3.3V│  ○ │─────────────────│○ VCC │
    │  ○ │─────────────────│○ GND │
 10 │  ○ │─────────────────│○ CS  │
 11 │  ○ │─────────────────│○ DC  │
 12 │  ○ │─────────────────│○ RST │
 13 │  ○ │─────────────────│○ MOSI│
 14 │  ○ │─────────────────│○ SCK │
3.3V│  ○ │─────────────────│○ BLK │
 GND│  ○ │─────────────────│○ GND │
    └────┘                  └──────┘
```

---

## 🔧 Código de Prueba Rápido

Usa este código para verificar que tus pines funcionan correctamente:

```cpp
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS    10
#define TFT_DC    11
#define TFT_RST   12
#define TFT_MOSI  13
#define TFT_SCLK  14

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Inicializando con pines personalizados...");
  Serial.printf("CS=%d, DC=%d, RST=%d, MOSI=%d, SCK=%d\n", 
                TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);
  
  // Inicializar SPI con tus pines
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  SPI.setFrequency(27000000);
  
  // Inicializar display
  tft.init(240, 240, SPI_MODE2);
  delay(200);
  
  Serial.println("Display inicializado!");
  Serial.println("Probando colores...");
  
  tft.fillScreen(0x001F); // Azul
  delay(1000);
  
  tft.fillScreen(0x07E0); // Verde
  delay(1000);
  
  tft.fillScreen(0xF800); // Rojo
  delay(1000);
  
  tft.fillScreen(0xFFFF); // Blanco
  delay(1000);
  
  tft.fillScreen(0x0000); // Negro
  
  Serial.println("✓ Test completo! Pines funcionando correctamente.");
  
  // Dibujar círculo de prueba
  tft.fillScreen(0x0000);
  tft.fillCircle(120, 120, 50, 0xF800); // Círculo rojo en centro
  
  Serial.println("Si ves un círculo rojo en el centro, todo está perfecto!");
}

void loop() {
  // Vacío
}
```

**Resultado esperado:**
```
Inicializando con pines personalizados...
CS=10, DC=11, RST=12, MOSI=13, SCK=14
Display inicializado!
Probando colores...
✓ Test completo! Pines funcionando correctamente.
Si ves un círculo rojo en el centro, todo está perfecto!
```

---

## 🎯 Checklist de Verificación

Antes de conectar:

- [ ] **GPIO 10** libre y disponible en tu ESP32
- [ ] **GPIO 11** libre y disponible en tu ESP32
- [ ] **GPIO 12** libre (⚠️ si usas JTAG, evitar este pin)
- [ ] **GPIO 13** libre y disponible en tu ESP32
- [ ] **GPIO 14** libre y disponible en tu ESP32
- [ ] VCC del display a **3.3V** (no 5V)
- [ ] GND común entre ESP32 y display
- [ ] Cables cortos (< 20cm)
- [ ] Código actualizado con estos pines
- [ ] Serial Monitor a 115200 baudios

---

## 📦 Lista de Cables Necesarios

| Cable | Desde | Hasta | Color sugerido |
|-------|-------|-------|----------------|
| 1 | ESP32 3.3V | ST7789 VCC | Rojo |
| 2 | ESP32 GND | ST7789 GND | Negro |
| 3 | ESP32 GPIO 10 | ST7789 CS | Amarillo |
| 4 | ESP32 GPIO 11 | ST7789 DC | Verde |
| 5 | ESP32 GPIO 12 | ST7789 RST | Azul |
| 6 | ESP32 GPIO 13 | ST7789 MOSI | Naranja |
| 7 | ESP32 GPIO 14 | ST7789 SCK | Morado |
| 8 | 3.3V | ST7789 BLK | Rojo (opcional) |

---

## 🔍 Troubleshooting Específico

### Problema: Pantalla no enciende
**Verificar:**
1. GPIO 10, 11, 12 no estén en conflicto con otras funciones
2. Conexión de RST (GPIO 12) esté firme
3. VCC sea exactamente 3.3V

### Problema: Pantalla blanca o colores extraños
**Solución:**
```cpp
// Probar diferentes modos SPI
tft.init(240, 240, SPI_MODE0); // Probar MODE0
// o
tft.init(240, 240, SPI_MODE2); // Probar MODE2
// o
tft.init(240, 240, SPI_MODE3); // Probar MODE3
```

### Problema: Parpadeo
**Solución:**
1. Bajar frecuencia SPI:
   ```cpp
   SPI.setFrequency(20000000); // 20MHz en vez de 27MHz
   ```
2. Agregar capacitor 100µF entre VCC y GND del display
3. Usar cables más cortos

---

## 🆘 Si No Funciona con Estos Pines

Si tienes problemas con GPIO 10-14, puedes probar pines alternativos:

**Opción A: Pines VSPI estándar (más probados)**
```cpp
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   15
#define TFT_MOSI  23
#define TFT_SCLK  18
```

**Opción B: Pines alternativos seguros**
```cpp
#define TFT_CS    16
#define TFT_DC    17
#define TFT_RST   21
#define TFT_MOSI  13  // Mantener
#define TFT_SCLK  14  // Mantener
```

---

## ✅ Ventajas de Tu Configuración

1. **Pines agrupados:** GPIO 10-14 están físicamente cerca en la mayoría de placas
2. **No interfiere con WiFi:** Estos pines no causan problemas con WiFi
3. **Fácil de recordar:** Secuencia consecutiva 10, 11, 12, 13, 14
4. **Documentación clara:** Esta guía cubre tu configuración específica

---

**¡Listo!** Tu código `esp32_display_kawaii.ino` ya está actualizado con estos pines.

**Próximo paso:** Conectar el hardware según este diagrama y subir el código al ESP32.

---

**Versión:** 1.1 (Pines personalizados)  
**Última actualización:** Junio 2026  
**Configuración:** GPIO 10 (CS), 11 (DC), 12 (RST), 13 (MOSI), 14 (SCK)
