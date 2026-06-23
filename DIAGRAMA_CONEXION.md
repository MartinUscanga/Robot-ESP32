# 🔌 Diagrama de Conexión ESP32 + ST7789

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
                  │               │
                  │          IO36 │
                  │          IO39 │
                  │          IO34 │
                  │          IO35 │
                  │          IO32 │
                  │          IO33 │
                  │          IO25 │
                  │          IO26 │
                  │          IO27 │
                  │          IO14 │
                  │          IO12 │
                  │          IO13 │
     ST7789 RST ──┤ IO15     IO15 │
                  │          IO2  ├── ST7789 DC
                  │          IO0  │
                  │          IO4  │
                  │          IO16 │
                  │          IO17 │
     ST7789 CS  ──┤ IO5      IO5  │
                  │          IO18 ├── ST7789 SCK (Clock)
                  │          IO19 │
                  │          IO21 │
                  │          RX   │
                  │          TX   │
                  │          IO22 │
     ST7789 MOSI ─┤ IO23     IO23 │
                  │               │
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
                  │ CS            ├── ESP32 GPIO 5
                  │ RESET         ├── ESP32 GPIO 15
                  │ DC            ├── ESP32 GPIO 2
                  │ SDA/MOSI      ├── ESP32 GPIO 23
                  │ SCL/SCK       ├── ESP32 GPIO 18
                  │ BLK           ├── 3.3V (o GPIO 4 para PWM)
                  └───────────────┘
```

---

## 📋 Tabla de Conexiones

| ST7789 Pin | ESP32 GPIO | Función | Notas |
|------------|------------|---------|-------|
| **VCC** | 3.3V | Alimentación | ⚠️ **NUNCA 5V** - puede dañar el display |
| **GND** | GND | Tierra | Debe ser GND común con ESP32 |
| **CS** | GPIO 5 | Chip Select | Selecciona el dispositivo SPI |
| **DC** | GPIO 2 | Data/Command | Alterna entre datos y comandos |
| **RST** | GPIO 15 | Reset | Reinicia el display (activo bajo) |
| **SDA/MOSI** | GPIO 23 | Master Out | Envía datos del ESP32 al display |
| **SCL/SCK** | GPIO 18 | Clock | Señal de reloj SPI |
| **BLK** | 3.3V | Backlight | Retroiluminación (siempre ON) |

---

## ⚙️ Configuración en Código

```cpp
#define TFT_CS    5   // Chip Select
#define TFT_DC    2   // Data/Command
#define TFT_RST   15  // Reset
#define TFT_MOSI  23  // MOSI (VSPI)
#define TFT_SCLK  18  // Clock (VSPI)

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Inicializar SPI manualmente para control total
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  SPI.setFrequency(27000000); // 27MHz - estable
  
  // Inicializar display
  tft.init(240, 240, SPI_MODE2);
  delay(100);
}
```

---

## 🔧 Notas Importantes

### 1️⃣ **Voltaje**
- El ST7789 funciona a **3.3V lógico**
- Conectar a 5V puede **dañar permanentemente** el display
- Si usas un display con regulador de 5V, verifica su datasheet

### 2️⃣ **Pines SPI del ESP32**
El ESP32 tiene dos buses SPI:
- **HSPI:** No usado en este proyecto
- **VSPI (usado aquí):**
  - SCK  = GPIO 18
  - MISO = GPIO 19 (no usado)
  - MOSI = GPIO 23
  - CS   = Configurable (usamos GPIO 5)

### 3️⃣ **Pin DC (Data/Command)**
- `DC = LOW` → Envía **comandos** al display
- `DC = HIGH` → Envía **datos** (pixeles, colores)
- La librería Adafruit_ST7789 maneja esto automáticamente

### 4️⃣ **Pin RST (Reset)**
- Debe tener un **pull-up** (la librería lo hace por software)
- Durante inicialización: `LOW` → `HIGH` para reiniciar

### 5️⃣ **Backlight (BLK)**
Dos opciones:

**Opción A: Siempre encendido**
```
BLK ──► 3.3V
```

**Opción B: Control PWM (brillo ajustable)**
```cpp
#define BLK_PIN 4

void setup() {
  ledcSetup(0, 5000, 8); // Canal 0, 5kHz, 8 bits
  ledcAttachPin(BLK_PIN, 0);
  ledcWrite(0, 200); // Brillo 0-255 (200 = ~78%)
}
```

---

## 🛠️ Troubleshooting de Conexiones

### ✅ Checklist Pre-Conexión
- [ ] Verificar que VCC del display sea 3.3V, NO 5V
- [ ] Usar cables cortos (< 20cm) - reducen interferencia
- [ ] Soldar conexiones (mejor que jumpers/breadboard)
- [ ] Multímetro: verificar continuidad de cada cable
- [ ] No conectar BLK a 5V - solo 3.3V o GPIO

### ❌ Errores Comunes

**Error 1: Pantalla blanca/negra permanente**
- Revisar conexión de **DC** y **RST**
- Verificar que VCC sea exactamente 3.3V

**Error 2: Parpadeo/estática**
- Cables muy largos → acortar
- Frecuencia SPI muy alta → bajar a 20-27 MHz
- Alimentación insuficiente → usar fuente 5V/1A en VIN

**Error 3: No enciende**
- Verificar BLK conectado a 3.3V
- Medir voltaje en VCC del display (debe ser 3.3V)
- Probar conectar RST directo a 3.3V (bypass)

---

## 📸 Diagrama Visual

```
                    ┌─────────────────────┐
                    │    ESP32 DevKit     │
                    │                     │
                    │  [USB]          [○] │ 3.3V ──┐
                    │                     │        │
                    │  GPIO 23 (MOSI) [○] │────────┼──► SDA
                    │  GPIO 18 (SCK)  [○] │────────┼──► SCL
                    │  GPIO 5  (CS)   [○] │────────┼──► CS
                    │  GPIO 2  (DC)   [○] │────────┼──► DC
                    │  GPIO 15 (RST)  [○] │────────┼──► RST
                    │                     │        │
                    │  GND            [○] │────────┼──► GND
                    └─────────────────────┘        │
                                                   │
                           ┌───────────────────────┘
                           │
                    ┌──────▼──────────┐
                    │   ST7789 240x240│
                    ├─────────────────┤
                    │                 │
                    │   ┌─────────┐   │
                    │   │ SCREEN  │   │
                    │   └─────────┘   │
                    │                 │
                    └─────────────────┘
```

---

## 🔌 Conexión con Fuente Externa (Recomendado)

Si experimentas reboots o parpadeo, usa fuente externa:

```
    Fuente 5V/1A
    │
    ├─────► ESP32 VIN
    └─────► GND (común)

    ESP32 3.3V ──► ST7789 VCC
    ESP32 GND  ──► ST7789 GND (común con fuente)
```

**Ventajas:**
- Corriente estable (USB puede dar solo 500mA)
- WiFi + Display consumen ~400-600mA en picos
- Evita brownouts y reboots

---

## 📦 Lista de Materiales

| Componente | Especificación | Cantidad |
|------------|----------------|----------|
| ESP32 DevKit | 30 pines, WiFi | 1 |
| Display ST7789 | 240x240, SPI, 3.3V | 1 |
| Cables Dupont | M-M o M-F, 10-20cm | 8 |
| Fuente 5V/1A | Micro USB o DC Jack | 1 (opcional) |
| Capacitor | 100µF, 6.3V | 1 (opcional) |
| Breadboard | 400 puntos | 1 (opcional) |

---

**¿Listo para empezar?**
1. Conecta según el diagrama
2. Verifica conexiones con multímetro
3. Sube el código `esp32_display_kawaii.ino`
4. Abre Serial Monitor a 115200 baudios
5. ¡Disfruta tu robot kawaii! 🤖✨

---

**Versión:** 1.0  
**Última actualización:** Junio 2026
