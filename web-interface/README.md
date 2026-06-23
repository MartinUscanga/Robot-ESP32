# ✨ Asistente Virtual Kawaii - Interfaz Web para Gemini

Interfaz web adorable y animada que se conecta a tu API de Gemini (Robot ESP32). Perfecta para probar tu asistente mientras desarrollas el hardware, o simplemente para tener un chatbot kawaii en tu navegador. 🤖💕

![Robot Kawaii](https://img.shields.io/badge/Estado-Listo-success?style=for-the-badge)
![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=for-the-badge&logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)

## 🎨 Características

### 🤖 Personaje Animado
- **Robot kawaii** con animaciones fluidas
- **Expresiones dinámicas**: feliz, pensando, hablando, sorprendido
- **Parpadeo automático** de ojos
- **Antena brillante** con efectos de luz
- **Mejillas sonrojadas** que pulsan cuando está feliz
- **Respiración realista** (sube y baja suavemente)

### 💬 Chat Interactivo
- **Burbujas de mensaje** estilo kawaii
- **Indicador de "escribiendo"** mientras el robot piensa
- **Timestamps** en cada mensaje
- **Historial de conversación** persistente
- **Auto-scroll** al final del chat
- **Reproducción de audio** automática de las respuestas

### ✨ Efectos Visuales
- **Partículas flotantes** de fondo (estrellas, corazones, sparkles)
- **Gradientes pastel** en toda la interfaz
- **Animaciones suaves** en todos los elementos
- **Transiciones fluidas** entre estados

### ⚙️ Configuración
- **URL de API configurable** (para desarrollo local o VPS)
- **Device ID personalizable** (para mantener conversaciones separadas)
- **Configuración persistente** en localStorage
- **Indicador de conexión** en tiempo real

### 🎮 Easter Eggs
- **Triple click** en el robot → ¡Confeti! 🎉
- **Ctrl + Mover mouse** → Rastro mágico de partículas ✨

## 🚀 Instalación y Uso

### Opción 1: Servidor Simple (Recomendado)

#### Con Python 3:
```bash
cd web-interface
python3 -m http.server 8080
```

#### Con Python 2:
```bash
cd web-interface
python -m SimpleHTTPServer 8080
```

#### Con Node.js (npx):
```bash
cd web-interface
npx http-server -p 8080
```

#### Con PHP:
```bash
cd web-interface
php -S localhost:8080
```

Luego abre tu navegador en: **http://localhost:8080**

### Opción 2: Abrir Directamente

Si no necesitas hacer peticiones CORS (la API está en el mismo dominio), puedes abrir `index.html` directamente en tu navegador.

**Nota:** Algunos navegadores pueden bloquear peticiones a localhost por política CORS cuando abres archivos locales. En ese caso, usa la Opción 1.

## 🔧 Configuración de la API

### Primera Vez

1. **Inicia tu API de Gemini** (el servidor Python de FastAPI):
   ```bash
   cd ..  # Volver a la raíz del proyecto Robot-ESP32
   source venv/bin/activate  # En Windows: venv\Scripts\activate
   uvicorn main:app --reload --host 0.0.0.0 --port 8000
   ```

2. **Abre la interfaz web** en tu navegador

3. **Haz click en el botón de configuración** (⚙️) en la esquina inferior derecha

4. **Configura la URL de tu API**:
   - Para desarrollo local: `http://localhost:8000`
   - Para VPS: `http://tu-ip-o-dominio:8000`

5. **Device ID** (opcional):
   - Por defecto: `web-interface`
   - Cámbialo si quieres mantener conversaciones separadas

6. **Guarda la configuración** 💾

### Verificación

- El **indicador de conexión** en la parte superior debe mostrar **"Conectado"** con un punto verde
- Si aparece **"Desconectado"**, verifica:
  - ✅ Tu API está corriendo (`python main.py` o `uvicorn`)
  - ✅ La URL en configuración es correcta
  - ✅ No hay firewall bloqueando el puerto 8000
  - ✅ La API tiene una key válida de Gemini en el `.env`

## 📱 Uso

### Enviar un Mensaje

1. **Escribe tu pregunta** en el campo de texto
2. **Presiona Enter** o haz click en "Enviar 💌"
3. El robot:
   - 👂 **Escuchará** (animación de antena brillante)
   - 🧠 **Pensará** (ojos moviéndose, puntos de carga)
   - 🗣️ **Hablará** (boca animada, audio reproduciéndose)
4. La respuesta aparecerá en el chat y **se reproducirá el audio automáticamente**

### Reiniciar Conversación

- Haz click en el botón **"🔄 Reiniciar"** debajo del input
- Esto borra el historial de conversación en el servidor
- El robot volverá a su estado inicial

### Interactuar con el Personaje

- **Observa las animaciones** mientras el robot procesa tu mensaje
- **Haz triple click** en el robot para celebrar con confetti 🎉
- **Mantén Ctrl y mueve el mouse** sobre la pantalla para crear un rastro mágico ✨

## 🎨 Personalización

### Cambiar Colores

Edita `css/style.css` en la sección `:root`:

```css
:root {
    --primary: #FF9ECD;      /* Rosa principal */
    --secondary: #B4A7FF;    /* Morado */
    --accent: #FFD1DC;       /* Rosa claro */
    --success: #B4F8C8;      /* Verde */
    --warning: #FFF4A3;      /* Amarillo */
    
    /* Cambia estos valores a tus colores favoritos */
}
```

### Cambiar el Personaje

Puedes modificar la apariencia del robot en `index.html` y los estilos en `css/style.css`:

- **Tamaño**: Ajusta `width` y `height` en `.robot-head` y `.robot-torso`
- **Colores**: Cambia `--robot-body`, `--robot-accent`, `--robot-dark`
- **Forma**: Modifica `border-radius` para hacerlo más cuadrado o redondo

### Agregar Más Partículas

Edita `js/particles.js`:

```javascript
this.maxParticles = 30;  // Aumenta este número para más partículas

// Agrega nuevos tipos de partículas
this.particleTypes = ['star', 'heart', 'sparkle', 'circle', 'tu-nuevo-tipo'];
```

## 🐛 Solución de Problemas

### ❌ "No se pudo conectar con el servidor"

**Problema:** La interfaz no puede comunicarse con la API.

**Soluciones:**
1. Verifica que la API esté corriendo:
   ```bash
   curl http://localhost:8000/health
   ```
   Deberías ver algo como: `{"status":"ok",...}`

2. Revisa la URL en configuración (⚙️)
   - Debe ser exactamente: `http://localhost:8000` (sin `/` al final)
   
3. Si usas un VPS, verifica que el puerto 8000 esté abierto:
   ```bash
   sudo ufw allow 8000
   ```

### ❌ "El audio no se reproduce"

**Problema:** El texto de la respuesta aparece pero no se escucha nada.

**Soluciones:**
1. **Revisa el volumen** de tu sistema y navegador
2. **Algunos navegadores** bloquean auto-reproducción de audio
   - Haz click en cualquier parte de la página primero
   - Revisa si hay un ícono de "bloquear audio" en la barra de URL
3. **Verifica los logs** de la API para ver si hay errores con gTTS o ffmpeg

### ❌ Error CORS

**Problema:** `Access to fetch at 'http://...' from origin '...' has been blocked by CORS policy`

**Soluciones:**
1. **No abras el HTML directamente** → Usa un servidor web (Opción 1)
2. Si es tu VPS, **agrega CORS** a la API:
   ```python
   # En main.py
   from fastapi.middleware.cors import CORSMiddleware
   
   app.add_middleware(
       CORSMiddleware,
       allow_origins=["*"],  # En producción, especifica los dominios permitidos
       allow_credentials=True,
       allow_methods=["*"],
       allow_headers=["*"],
   )
   ```

### ❌ "El servidor está saturado" (Error 503)

**Problema:** La API de Gemini está experimentando alta demanda.

**Soluciones:**
- **Espera 5-10 segundos** e intenta de nuevo
- La API implementa reintentos automáticos (hasta 3 veces)
- Si persiste, revisa el [estado de la API de Gemini](https://status.cloud.google.com/)

### ❌ Las animaciones van lentas

**Problema:** El navegador va lento con tantas partículas.

**Soluciones:**
1. **Reduce las partículas**: Edita `js/particles.js`
   ```javascript
   this.maxParticles = 15;  // Reduce de 30 a 15
   ```
2. **Cierra otras pestañas** del navegador
3. **Usa un navegador más moderno** (Chrome, Edge, Firefox recientes)

## 📂 Estructura del Proyecto

```
web-interface/
├── index.html              # Estructura HTML principal
├── css/
│   ├── style.css          # Estilos principales y layout
│   └── animations.css     # Todas las animaciones CSS
├── js/
│   ├── main.js           # Lógica principal de la app
│   ├── api.js            # Comunicación con la API de Gemini
│   ├── assistant.js      # Manejo del personaje y estados
│   └── particles.js      # Sistema de partículas flotantes
└── README.md             # Este archivo
```

## 🔗 Integración con el ESP32

Esta interfaz web usa el mismo endpoint que usará tu ESP32:

- **Web**: `POST /chat/texto` con `mensaje` en FormData
- **ESP32**: `POST /chat` con archivo de audio en FormData

Ambos reciben:
- **Audio WAV** en el body de la respuesta
- **Texto en base64** en el header `X-Response-Text-B64`

Cuando tengas tu ESP32 listo, el flujo será el mismo, solo cambiará de texto a audio como input. 🎤🔊

## 🎯 Casos de Uso

### 1. **Desarrollo y Testing**
Prueba tu API de Gemini sin necesidad del hardware ESP32:
- Ajusta la personalidad del robot
- Prueba diferentes prompts
- Verifica que el TTS funciona correctamente

### 2. **Demo y Presentaciones**
Muestra tu proyecto de forma visual y atractiva:
- Interfaz llamativa para presentaciones
- Fácil de usar en cualquier dispositivo
- No requiere hardware adicional

### 3. **Asistente de Escritorio**
Úsalo como chatbot personal:
- Deja la pestaña abierta mientras trabajas
- Pregunta rápidamente sin abrir ChatGPT
- Mantiene contexto de la conversación

### 4. **Prototipado Rápido**
Base para otros proyectos:
- Adaptable a diferentes APIs
- Código limpio y comentado
- Fácil de personalizar

## 🚀 Próximos Pasos

- [ ] Agregar soporte para **entrada de voz** (micrófono del navegador)
- [ ] **Temas personalizables** (modo oscuro, otros colores)
- [ ] **Historial de conversaciones** guardado en localStorage
- [ ] **Exportar conversaciones** como texto o PDF
- [ ] **Modo compacto** para usar en una barra lateral
- [ ] **Atajos de teclado** para funciones comunes
- [ ] **Más expresiones** del personaje basadas en sentiment analysis

## 📝 Notas Técnicas

### Navegadores Compatibles
- ✅ Chrome 90+
- ✅ Firefox 88+
- ✅ Edge 90+
- ✅ Safari 14+
- ⚠️ Internet Explorer: No compatible

### Requisitos del Sistema
- **Navegador moderno** con soporte para CSS Grid, Flexbox, y Web Audio API
- **JavaScript habilitado**
- **Conexión a internet** (solo para la API de Gemini, la interfaz es local)

### Rendimiento
- **Ligero**: ~50KB total (sin comprimir)
- **Sin dependencias**: 0 librerías externas
- **Rápido**: Carga instantánea en cualquier red

## 🤝 Contribuciones

Si mejoras esta interfaz, considera:
1. **Optimizar las animaciones** para dispositivos móviles
2. **Agregar tests** automatizados
3. **Mejorar la accesibilidad** (ARIA labels, navegación por teclado)
4. **Documentar más casos de uso**

## 📄 Licencia

Este proyecto es parte del sistema Robot-ESP32. Úsalo libremente para aprender, modificar y compartir. 🎉

---

**Creado con 💕 para el proyecto Robot ESP32**

¿Preguntas? Revisa la documentación principal en el README.md del proyecto o abre un issue en GitHub.

**¡Disfruta de tu asistente kawaii!** ✨🤖💕
