# 🔧 Troubleshooting ESP32 + ST7789

## 🚨 Problema: Pantalla Parpadeando / Estática Negra

Si tu pantalla ST7789 está parpadeando, mostrando estática o pantalla negra, sigue estos pasos:

---

## ✅ Checklist de Verificación

### 1. **Verificar Conexiones Físicas**

| Pin ST7789 | Pin ESP32 | Función |
|------------|-----------|---------|
| VCC        | 3.3V      | Alimentación (⚠️ NO usar 5V) |
| GND        | GND       | Tierra |
| CS         | GPIO 5    | Chip Select |
| DC         | GPIO 2    | Data/Command |
| RST        | GPIO 15   | Reset |
| SDA/MOSI   | GPIO 23   | Datos SPI |
| SCL/SCK    | GPIO 18   | Clock SPI |
| BLK        | 3.3V      | Backlight (puede ir a GPIO para PWM) |

**Importante:**
- ✅ **Usar cables cortos** (< 20cm) - los cables largos causan interferencia
- ✅ **Conexiones firmes** - revisar que no haya falsos contactos
- ✅ **3.3V**, nunca 5V - el ST7789 es sensible a sobrevoltaje
- ✅ **GND común** - ESP32 y display deben compartir tierra

---

### 2. **Problemas Comunes de Software**

#### **A) Frecuencia SPI muy alta**
```cpp
// ❌ MAL - puede causar parpadeo
SPI.setFrequency(40000000); // 40MHz puede ser inestable

// ✅ BIEN - más estable
SPI.setFrequency(27000000); // 27MHz
```

#### **B) Falta de delays en inicialización**
```cpp
// ❌ MAL
tft.init(240, 240);
tft.fillScreen(COLOR_FONDO);

// ✅ BIEN
tft.init(240, 240, SPI_MODE2);
delay(100); // ⬅️ Crucial!
tft.setRotation(0);
delay(100);
tft.fillScreen(COLOR_FONDO);
delay(100);
```

#### **C) Modo SPI incorrecto**
```cpp
// Probar diferentes modos si sigue parpadeando
tft.init(240, 240, SPI_MODE0); // Probar MODE0, MODE2, MODE3
```

#### **D) Uso excesivo de RAM (causa reboots)**
```cpp
// ❌ MAL - 115KB de RAM (ESP32 solo tiene ~320KB)
GFXcanvas16 canvas(240, 240);

// ✅ BIEN - dibujar directo al display
tft.fillRect(x, y, w, h, color);
```

---

### 3. **Diagnóstico por Serial Monitor**

Abre el **Monitor Serial a 115200 baudios** y verifica:

```
=== ESP32 Robot Kawaii ===
Inicializando...
Inicializando ST7789...
✓ Display inicializado         ⬅️ Debe aparecer!

Conectando a WiFi...
.........
✓ WiFi conectado!
IP: 192.168.x.x

=== Sistema listo ===
RAM libre: 285000              ⬅️ Debe ser > 200000
```

#### **Errores comunes:**

**Error 1: Guru Meditation Error**
```
Guru Meditation Error: Core 1 panic'ed (LoadProhibited)
```
**Solución:** RAM insuficiente. Eliminar buffers grandes (canvas, arrays).

**Error 2: Brownout detector**
```
Brownout detector was triggered
```
**Solución:** Alimentación insuficiente. Usar fuente de al menos 500mA.

**Error 3: WiFi causa reboots**
```
E (12345) wifi: wifi_init: WiFi driver is not running
```
**Solución:** Agregar `WiFi.mode(WIFI_STA);` antes de `WiFi.begin()`.

---

### 4. **Código de Prueba Mínimo**

Si el código completo no funciona, prueba este código mínimo:

```cpp
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   15

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Inicializando...");
  
  // Inicializar SPI manualmente
  SPI.begin(18, -1, 23, 5); // SCK, MISO(-1), MOSI, CS
  SPI.setFrequency(27000000);
  
  // Inicializar display
  tft.init(240, 240, SPI_MODE2);
  delay(200);
  
  Serial.println("Llenando pantalla...");
  tft.fillScreen(0x001F); // Azul
  delay(1000);
  
  tft.fillScreen(0x07E0); // Verde
  delay(1000);
  
  tft.fillScreen(0xF800); // Rojo
  delay(1000);
  
  Serial.println("✓ Test completo!");
}

void loop() {
  // Vacío - solo test de inicialización
}
```

**Resultado esperado:**
- Pantalla cambia de **azul → verde → rojo**
- Si no funciona, problema es **hardware** (conexiones/alimentación)

---

### 5. **Problemas de Alimentación**

#### **Síntomas de alimentación insuficiente:**
- Pantalla parpadea al conectar WiFi
- Reboots aleatorios
- Display se apaga parcialmente
- Mensaje "Brownout detector"

#### **Soluciones:**

1. **Usar fuente externa de 5V/1A:**
   ```
   Fuente 5V ──► Pin VIN del ESP32
   GND ──────► GND común
   ```

2. **Agregar capacitor de 100µF entre VCC y GND del display**
   - Estabiliza voltaje durante picos de corriente

3. **Bajar brillo del backlight:**
   ```cpp
   // Si BLK está en GPIO (ej: GPIO 4)
   ledcSetup(0, 5000, 8); // Canal 0, 5kHz, 8 bits
   ledcAttachPin(4, 0);
   ledcWrite(0, 128); // 50% brillo (0-255)
   ```

---

### 6. **Interferencia WiFi ↔ SPI**

El WiFi puede interferir con el SPI. Soluciones:

#### **A) Separar operaciones:**
```cpp
// ❌ MAL - hacer HTTP mientras dibuja
http.POST(body);
tft.fillScreen(color);

// ✅ BIEN - separar operaciones
http.POST(body);
http.end();
delay(10); // ⬅️ Pequeño delay
tft.fillScreen(color);
```

#### **B) Desactivar WiFi temporalmente:**
```cpp
WiFi.mode(WIFI_OFF);
delay(10);
tft.fillScreen(COLOR);
delay(10);
WiFi.mode(WIFI_STA);
```

---

## 📋 Checklist Final

Antes de subir el código, verifica:

- [ ] Conexiones soldadas o firmemente conectadas
- [ ] VCC del display a **3.3V** (no 5V)
- [ ] GND común entre ESP32 y display
- [ ] Cables SPI < 20cm de largo
- [ ] SPI.setFrequency entre 20-30 MHz
- [ ] Delays de 100ms después de `init()` y `fillScreen()`
- [ ] RAM libre > 200KB (sin canvas grandes)
- [ ] Monitor Serial a 115200 para ver errores
- [ ] Fuente de al menos 500mA
- [ ] Test mínimo (código arriba) funciona primero

---

## 🆘 Si Aún No Funciona

1. **Probar con display diferente** - puede estar dañado
2. **Probar con ESP32 diferente** - puede tener pin dañado
3. **Medir voltajes con multímetro:**
   - VCC del display: debe ser 3.3V estable
   - Durante WiFi activo: voltaje NO debe caer < 3.1V
4. **Revisar datasheet del ST7789:**
   - Algunos displays necesitan `SPI_MODE0`, otros `SPI_MODE2`
   - Verificar si tu display es ST7789 o ST7789V (variantes)

---

## 📞 Soporte

Si después de seguir todos estos pasos sigue sin funcionar:

1. Copia el output completo del Serial Monitor
2. Toma foto de las conexiones físicas
3. Anota tu modelo exacto de ESP32 (DevKit, WROOM, etc.)
4. Especifica modelo del display (ST7789, ST7789V, etc.)

---

**Última actualización:** Junio 2026
**Versión código:** 1.0
