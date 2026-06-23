# 🔧 Solución Rápida - Errores de Compilación ESP32-S3

## ❌ Error Encontrado

Si ves este error al compilar:

```
error: invalid conversion from 'int' to 'const esp_task_wdt_config_t*' [-fpermissive]
esp_task_wdt_init(30, false);
```

## ✅ Solución Aplicada

El código ya ha sido corregido y es **compatible con ambas versiones** del Arduino Core:

### Versión Antigua (< 2.0.14)
```cpp
esp_task_wdt_init(30, false);  // Dos parámetros: timeout_s, panic_on_timeout
```

### Versión Nueva (>= 2.0.14)
```cpp
esp_task_wdt_config_t wdt_config = {
  .timeout_ms = 30000,
  .idle_core_mask = 0,
  .trigger_panic = false
};
esp_task_wdt_init(&wdt_config);  // Puntero a estructura
```

### Código Implementado (Automático)
```cpp
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 14)
  // Versión nueva
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 30000,
    .idle_core_mask = 0,
    .trigger_panic = false
  };
  esp_task_wdt_init(&wdt_config);
#else
  // Versión antigua
  esp_task_wdt_init(30, false);
#endif
```

## 🔄 Cómo Actualizar

### Opción 1: Descargar Código Actualizado
```bash
git pull origin feature/esp32s3-audio-bidirectional
```

### Opción 2: Actualizar Arduino Core

1. **Abrir Arduino IDE**
2. **Herramientas > Placa > Gestor de tarjetas**
3. Buscar: **"ESP32"**
4. Desinstalar versión actual
5. Instalar: **"ESP32 by Espressif Systems"** versión **2.0.14** o superior
6. Reiniciar Arduino IDE

## 📋 Versiones Recomendadas

| Componente | Versión Recomendada |
|------------|-------------------|
| Arduino IDE | 2.3.0 o superior |
| ESP32 Arduino Core | 2.0.14 - 2.0.17 |
| Adafruit GFX | 1.11.9 |
| Adafruit ST7789 | 1.10.4 |

## 🔍 Verificar Versión del Arduino Core

En Arduino IDE:
1. **Herramientas > Placa > Gestor de tarjetas**
2. Buscar: "ESP32"
3. Ver versión instalada de "ESP32 by Espressif Systems"

## 🐛 Otros Errores Comunes

### Error: "'ps_malloc' was not declared"
**Causa**: PSRAM no habilitado

**Solución**:
```
Herramientas > PSRAM > "OPI PSRAM"
```

### Error: "Sketch too big"
**Causa**: Partition scheme incorrecta

**Solución**:
```
Herramientas > Partition Scheme > "Huge APP (3MB No OTA/1MB SPIFFS)"
```

### Error: "A fatal error occurred: Timed out waiting for packet header"
**Causa**: ESP32-S3 no entró en modo bootloader

**Solución**:
1. Mantener presionado **BOOT**
2. Presionar y soltar **RESET**
3. Soltar **BOOT**
4. Intentar cargar de nuevo

### Warning: "implicit declaration of function"
**Causa**: Falta declaración adelantada (forward declaration)

**Solución**: Ya incluidas en el código actualizado

## 📚 Referencias

- **ESP32 Arduino Core Releases**: https://github.com/espressif/arduino-esp32/releases
- **ESP32-S3 Datasheet**: https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- **Watchdog Timer API**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/system/wdts.html

## ✅ Checklist de Verificación

Antes de compilar, asegúrate de:

- [ ] Arduino Core ESP32 versión 2.0.14 o superior instalada
- [ ] Bibliotecas Adafruit GFX y ST7789 instaladas
- [ ] Placa seleccionada: "ESP32S3 Dev Module"
- [ ] PSRAM: "OPI PSRAM"
- [ ] Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
- [ ] USB CDC On Boot: "Enabled"
- [ ] Código actualizado de GitHub

## 🆘 ¿Todavía tienes errores?

1. **Limpiar proyecto**: Sketch > Limpiar carpeta de construcción
2. **Reiniciar Arduino IDE**
3. **Verificar puerto COM correcto**
4. **Revisar Serial Monitor**: 115200 baudios
5. **Consultar**: [ESP32S3_SETUP.md](./ESP32S3_SETUP.md)

---

**Última actualización**: Compatible con Arduino Core 2.0.14 - 2.0.17

**Estado**: ✅ Corregido y probado
