# 🎭 Personalización del Asistente

Guía completa para personalizar las respuestas de tu asistente según tus preferencias.

---

## 🚀 **Método 1: Perfil de Usuario (Recomendado)**

### **Paso 1: Editar tu perfil**

```bash
nano ~/Robot-ESP32/user_profile.py
```

### **Paso 2: Personalizar los datos**

Edita los siguientes campos según tus preferencias:

```python
PERFIL_USUARIO = {
    # TU INFORMACIÓN
    "nombre": "Tu Nombre",
    "apellido": "Tu Apellido",
    "email": "tu-email@gmail.com",
    
    # TUS INTERESES
    "intereses": [
        "Robótica",
        "IoT",
        "Inteligencia Artificial",
        # Agrega más...
    ],
    
    # TUS PROYECTOS
    "proyectos": [
        {
            "nombre": "Proyecto 1",
            "descripcion": "Descripción breve",
            "tecnologias": ["Python", "ESP32"]
        }
    ],
    
    # TUS HABILIDADES
    "habilidades": {
        "lenguajes": ["Python", "JavaScript"],
        "nivel": "intermedio-avanzado"  # basico, intermedio, intermedio-avanzado, avanzado
    },
    
    # TIPO DE ASISTENTE
    "asistente": {
        "tipo": "jarvis"  # jarvis, cortana, friday, alfred
    }
}
```

### **Paso 3: Reiniciar la API**

```bash
pkill -f uvicorn
cd ~/Robot-ESP32
source venv/bin/activate
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
```

---

## 🎨 **Personalidades Disponibles:**

### **JARVIS** (Recomendado para ti)
- **Estilo:** Sofisticado, elegante, ingenioso
- **Tono:** Profesional pero amigable
- **Nivel técnico:** Alto
- **Personalidad:** Como el asistente de Iron Man

```python
"asistente": {
    "tipo": "jarvis"
}
```

### **CORTANA**
- **Estilo:** Militar, táctica, directa
- **Tono:** Seguro y eficiente
- **Nivel técnico:** Muy alto
- **Personalidad:** IA militar con personalidad

```python
"asistente": {
    "tipo": "cortana"
}
```

### **FRIDAY**
- **Estilo:** Directa, sin rodeos
- **Tono:** Casual pero profesional
- **Nivel técnico:** Alto
- **Personalidad:** Sucesora de JARVIS, más informal

```python
"asistente": {
    "tipo": "friday"
}
```

### **ALFRED**
- **Estilo:** Sabio, experimentado
- **Tono:** Refinado y cortés
- **Nivel técnico:** Medio-alto
- **Personalidad:** Mayordomo británico inteligente

```python
"asistente": {
    "tipo": "alfred"
}
```

---

## 📝 **Método 2: Variable de Entorno**

Si prefieres una configuración más simple:

```bash
nano ~/Robot-ESP32/.env
```

Agrega:

```bash
PERSONALIDAD="Eres JARVIS, asistente personal de Martín. Respondes de forma breve, asertiva e inteligente. Consideras que tu usuario es un desarrollador técnico trabajando en robótica e IoT. Tono: profesional, directo, ligeramente ingenioso. Siempre en español, sin markdown ni símbolos."
```

---

## 🧪 **Probar la Personalización**

### **Preguntas de prueba:**

1. **General:**
   ```
   "¿Qué tal estás?"
   ```
   
2. **Técnica:**
   ```
   "Explícame cómo funciona MQTT"
   ```
   
3. **Proyecto actual:**
   ```
   "Dame ideas para mejorar mi robot ESP32"
   ```
   
4. **Consejo:**
   ```
   "¿Qué debería aprender después?"
   ```

### **Diferencias que notarás:**

**Antes (personalidad genérica):**
> "¡Hola! MQTT es un protocolo de mensajería ligero. Es genial para IoT porque usa poco ancho de banda. ¿Quieres que te explique más?"

**Después (personalidad JARVIS personalizada):**
> "Excelente pregunta, Martín. MQTT es el protocolo ideal para tu proyecto ESP32 por su bajo overhead. Te recomiendo usar QoS 1 para balance entre confiabilidad y rendimiento."

---

## 🔧 **Configuraciones Avanzadas**

### **Ajustar nivel de detalle:**

```python
"preferencias": {
    "nivel_detalle": "avanzado"  # basico, intermedio, avanzado
}
```

- **básico:** Respuestas simples, menos jerga técnica
- **intermedio:** Balance entre claridad y precisión
- **avanzado:** Respuestas técnicas detalladas

### **Ajustar estilo de respuesta:**

```python
"preferencias": {
    "estilo_respuesta": "tecnico_asertivo"  # casual, tecnico_asertivo, formal, amigable
}
```

### **Ajustar tono:**

```python
"preferencias": {
    "tono": "profesional_informal"  # formal, profesional_informal, casual, amigable
}
```

---

## 📊 **Comparación de Personalidades**

| Personalidad | Formalidad | Nivel Técnico | Humor | Mejor para |
|--------------|------------|---------------|-------|------------|
| JARVIS | Alta | Muy Alto | Sutil | Proyectos técnicos complejos |
| CORTANA | Media-Alta | Muy Alto | Bajo | Tareas tácticas y análisis |
| FRIDAY | Media | Alto | Medio | Trabajo rápido y eficiente |
| ALFRED | Alta | Medio-Alto | Medio | Consejos y guía estratégica |

---

## 🎯 **Tips para Mejores Respuestas**

### **1. Sé específico con tus intereses:**

❌ Mal:
```python
"intereses": ["programación", "tecnología"]
```

✅ Bien:
```python
"intereses": [
    "Desarrollo de sistemas embebidos con ESP32",
    "Integración de APIs de IA (Gemini, OpenAI)",
    "Automatización con Python y IoT",
    "Machine Learning aplicado a robótica"
]
```

### **2. Documenta tus proyectos actuales:**

```python
"proyectos": [
    {
        "nombre": "Robot Asistente ESP32",
        "descripcion": "Robot de voz inteligente con ESP32, Gemini API y TTS",
        "estado": "en desarrollo - fase de integración hardware",
        "tecnologias": ["ESP32", "Python", "FastAPI", "Gemini", "gTTS"],
        "desafios_actuales": ["Optimización de latencia", "Consumo de batería"]
    }
]
```

El asistente podrá:
- Darte consejos específicos sobre tus desafíos
- Sugerir soluciones relevantes a tu stack tecnológico
- Contextualizar respuestas basándose en tu proyecto

### **3. Define tu nivel técnico con precisión:**

```python
"habilidades": {
    "lenguajes": ["Python", "JavaScript", "C++"],
    "frameworks": ["FastAPI", "Node.js", "Arduino"],
    "nivel": "intermedio-avanzado",
    "areas_expertas": ["Python backend", "Hardware IoT"],
    "areas_aprendiendo": ["Machine Learning", "Rust"]
}
```

---

## 🔄 **Actualizar Personalización**

### **Después de editar user_profile.py:**

```bash
# Reiniciar API para aplicar cambios
pkill -f uvicorn
cd ~/Robot-ESP32
source venv/bin/activate
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
```

### **Verificar que se aplicó:**

```bash
# Ver logs de inicio
tail -20 /tmp/robot-api.log
```

Deberías ver:
```
✅ Usando perfil personalizado para Martín Uscanga
```

---

## 🌐 **Integración con Google Account (Avanzado)**

Si quieres que el asistente acceda a tus datos de Google (Calendar, Gmail, Drive):

### **Ventajas:**
- Puede leer tu calendario y recordarte eventos
- Acceder a tus emails para contexto
- Crear/leer documentos en Drive
- Conocer tus ubicaciones frecuentes

### **Requisitos:**
- OAuth 2.0 configurado
- Tokens de acceso de Google
- Permisos específicos configurados

### **Implementación:**

```bash
# Instalar librerías adicionales
pip install google-auth google-auth-oauthlib google-auth-httplib2 google-api-python-client
```

**Nota:** Esta funcionalidad requiere configuración adicional y está fuera del scope de esta guía básica. Si te interesa, puedo ayudarte a implementarla.

---

## 📝 **Ejemplo Completo de Perfil**

```python
PERFIL_USUARIO = {
    "nombre": "Martín",
    "apellido": "Uscanga",
    "nombre_completo": "Martín Uscanga",
    "email": "martin.uscanga@example.com",
    
    "preferencias": {
        "estilo_respuesta": "tecnico_asertivo",
        "nivel_detalle": "intermedio-avanzado",
        "idioma": "es",
        "tono": "profesional_informal",
    },
    
    "intereses": [
        "Robótica y sistemas embebidos",
        "IoT (ESP32, Arduino, Raspberry Pi)",
        "Inteligencia Artificial (Gemini, OpenAI)",
        "Desarrollo web full-stack",
        "Automatización y DevOps",
        "Machine Learning aplicado"
    ],
    
    "proyectos": [
        {
            "nombre": "Robot Asistente ESP32",
            "descripcion": "Asistente de voz inteligente con ESP32 y Gemini",
            "estado": "en desarrollo - 70% completo",
            "tecnologias": ["ESP32", "Python", "FastAPI", "Gemini API", "gTTS"],
            "desafios_actuales": ["Optimizar latencia", "Reducir consumo"]
        },
        {
            "nombre": "Interfaz Web Kawaii",
            "descripcion": "UI animada para control del asistente",
            "estado": "completado",
            "tecnologias": ["HTML5", "CSS3", "JavaScript", "Nginx"]
        }
    ],
    
    "habilidades": {
        "lenguajes": ["Python", "JavaScript", "C++", "HTML/CSS"],
        "frameworks": ["FastAPI", "Node.js", "Arduino"],
        "hardware": ["ESP32", "Arduino Nano", "Raspberry Pi"],
        "herramientas": ["Git", "Linux", "AWS", "VS Code"],
        "nivel": "intermedio-avanzado",
        "areas_expertas": ["Python backend", "Hardware IoT", "APIs REST"],
        "areas_aprendiendo": ["Machine Learning", "Computer Vision"]
    },
    
    "objetivos": [
        "Completar robot ESP32 con IA funcional",
        "Aprender deep learning para visión por computadora",
        "Crear más proyectos IoT innovadores",
        "Contribuir a proyectos open source"
    ],
    
    "contexto": {
        "ubicacion": "México",
        "zona_horaria": "America/Mexico_City",
        "horario_trabajo": "flexible - mayormente noches",
    },
    
    "asistente": {
        "tipo": "jarvis",
        "caracteristicas": [
            "Asertivo y directo",
            "Técnicamente preciso",
            "Proactivo en sugerencias",
            "Con humor sutil británico",
            "Enfocado en soluciones prácticas"
        ]
    }
}
```

---

## ❓ **Preguntas Frecuentes**

### **¿Puedo usar mi email de Google?**

Sí, pero en la configuración básica solo es para referencia. Si quieres integración con servicios de Google (Calendar, Gmail), necesitas OAuth 2.0.

### **¿El asistente puede acceder a mi cuenta de Google automáticamente?**

No con la configuración básica. Necesitarías:
1. Crear un proyecto en Google Cloud Console
2. Habilitar las APIs que quieras usar
3. Configurar OAuth 2.0
4. Implementar el flujo de autenticación

### **¿Cuánto cambian realmente las respuestas?**

Mucho. Con perfil personalizado:
- Respuestas más relevantes a tu stack tecnológico
- Referencias a tus proyectos actuales
- Nivel técnico apropiado (no sobre-explica cosas que ya sabes)
- Sugerencias alineadas con tus objetivos

### **¿Puedo tener múltiples personalidades?**

Sí, puedes crear archivos como `user_profile_jarvis.py`, `user_profile_cortana.py` y cambiar el import en `main.py`.

---

## 🎉 **¡Listo!**

Tu asistente ahora te conoce mejor y puede:
- ✅ Responder considerando tu nivel técnico
- ✅ Hacer referencia a tus proyectos
- ✅ Sugerir soluciones relevantes a tu stack
- ✅ Comunicarse en el tono que prefieres

¿Necesitas ayuda con algo más específico? 🚀
