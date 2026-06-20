"""
Puente ESP32 <-> Gemini
------------------------
Esta API recibe audio (grabado por el ESP32) o texto (para pruebas),
lo envía a Gemini para generar una respuesta, y devuelve la respuesta
ya convertida a audio (WAV PCM, fácil de reproducir desde el ESP32).

Pensada para correr en un VPS pequeño (1GB RAM funciona bien).
"""

import os
import base64
import logging
from pathlib import Path

from fastapi import FastAPI, UploadFile, File, Form, HTTPException
from fastapi.responses import Response
from dotenv import load_dotenv
from google import genai
from gtts import gTTS
from pydub import AudioSegment

# ---------------------------------------------------------------------------
# Configuración
# ---------------------------------------------------------------------------

load_dotenv()

GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
GEMINI_MODEL = os.getenv("GEMINI_MODEL", "gemini-1.5-flash")
TTS_LANG = os.getenv("TTS_LANG", "es")
API_PORT = int(os.getenv("API_PORT", "8000"))
API_HOST = os.getenv("API_HOST", "0.0.0.0")
LOG_LEVEL = os.getenv("LOG_LEVEL", "INFO")
MAX_HISTORIAL = int(os.getenv("MAX_HISTORIAL", "12"))
GEMINI_TIMEOUT = int(os.getenv("GEMINI_TIMEOUT", "30"))
TTS_TIMEOUT = int(os.getenv("TTS_TIMEOUT", "15"))
HTTP_PROXY = os.getenv("HTTP_PROXY")  # Opcional: proxy para Gemini API
HTTPS_PROXY = os.getenv("HTTPS_PROXY")  # Opcional: proxy para Gemini API

if not GEMINI_API_KEY:
    raise RuntimeError(
        "❌ Falta GEMINI_API_KEY. Crea un archivo .env basado en .env.example "
        "con tu API key de Gemini (https://aistudio.google.com/apikey)."
    )

# Configurar proxy si está definido
if HTTP_PROXY or HTTPS_PROXY:
    os.environ["HTTP_PROXY"] = HTTP_PROXY or ""
    os.environ["HTTPS_PROXY"] = HTTPS_PROXY or ""
    logger.info(f"🌐 Usando proxy: HTTP={HTTP_PROXY}, HTTPS={HTTPS_PROXY}")

client = genai.Client(api_key=GEMINI_API_KEY)

logging.basicConfig(
    level=getattr(logging, LOG_LEVEL.upper(), logging.INFO),
    format="%(asctime)s [%(levelname)s] %(message)s"
)
logger = logging.getLogger("robot-bridge")

app = FastAPI(title="Puente ESP32 <-> Gemini", version="1.0.0")

# Personalidad del asistente. Ajusta este texto para cambiar su "carácter".
PERSONALIDAD = os.getenv(
    "PERSONALIDAD",
    "Eres un asistente robot pequeño, gracioso y muy expresivo, hecho con un ESP32. "
    "Respondes siempre en español, de forma breve (máximo 2-3 frases cortas), "
    "natural y conversacional, como si hablaras en voz alta. "
    "No uses markdown, listas, asteriscos ni símbolos raros, porque tu respuesta "
    "se va a convertir directamente a voz. Tu tono es cálido, divertido y curioso."
)

# Memoria de conversación en RAM, por dispositivo (se borra si reinicia el servidor)
historiales: dict[str, list[str]] = {}

TEMP_DIR = Path("/tmp/robot_audio")
TEMP_DIR.mkdir(exist_ok=True)


# ---------------------------------------------------------------------------
# Funciones auxiliares
# ---------------------------------------------------------------------------

def texto_a_voz_wav(texto: str) -> bytes:
    """Convierte texto a audio WAV PCM 16kHz mono (formato fácil para el ESP32)."""
    mp3_path = TEMP_DIR / "respuesta.mp3"
    wav_path = TEMP_DIR / "respuesta.wav"

    tts = gTTS(text=texto, lang=TTS_LANG)
    tts.save(str(mp3_path))

    audio = AudioSegment.from_mp3(mp3_path)
    audio = audio.set_frame_rate(16000).set_channels(1).set_sample_width(2)
    audio.export(wav_path, format="wav")

    return wav_path.read_bytes()


def generar_respuesta(device_id: str, partes_contenido: list) -> str:
    """Llama a Gemini con el contenido (texto y/o audio) y regresa el texto de respuesta."""
    historial = historiales.get(device_id, [])

    contenido = [PERSONALIDAD] + historial[-6:] + partes_contenido

    respuesta = client.models.generate_content(
        model=GEMINI_MODEL,
        contents=contenido,
    )

    texto_respuesta = (respuesta.text or "Lo siento, no entendí eso.").strip()

    historial.append("Asistente: " + texto_respuesta)
    historiales[device_id] = historial[-MAX_HISTORIAL:]

    return texto_respuesta


def respuesta_con_audio(texto_respuesta: str) -> Response:
    """Empaqueta el texto + audio en una respuesta HTTP lista para el ESP32."""
    audio_wav = texto_a_voz_wav(texto_respuesta)
    texto_b64 = base64.b64encode(texto_respuesta.encode("utf-8")).decode("ascii")

    return Response(
        content=audio_wav,
        media_type="audio/wav",
        headers={"X-Response-Text-B64": texto_b64},
    )


# ---------------------------------------------------------------------------
# Endpoints
# ---------------------------------------------------------------------------

@app.get("/health")
def health():
    """Para verificar que el servidor está vivo (útil para monitoreo)."""
    return {
        "status": "ok",
        "modelo": GEMINI_MODEL,
        "idioma_tts": TTS_LANG,
        "dispositivos_activos": len(historiales),
        "max_historial": MAX_HISTORIAL,
    }


@app.post("/chat")
async def chat(file: UploadFile = File(...), device_id: str = Form("default")):
    """
    Endpoint principal: el ESP32 manda un archivo de audio (WAV/MP3) grabado
    del micrófono, y recibe de regreso audio WAV con la respuesta hablada.
    """
    try:
        audio_bytes = await file.read()
        mime_type = file.content_type or "audio/wav"
        logger.info(f"[{device_id}] Audio recibido: {len(audio_bytes)} bytes ({mime_type})")

        audio_path = TEMP_DIR / f"entrada_{device_id}.audio"
        audio_path.write_bytes(audio_bytes)

        archivo_subido = client.files.upload(
            file=str(audio_path),
            config={"mime_type": mime_type},
        )

        texto_respuesta = generar_respuesta(device_id, [archivo_subido])
        logger.info(f"[{device_id}] Gemini respondió: {texto_respuesta}")

        return respuesta_con_audio(texto_respuesta)

    except Exception as e:
        logger.exception("Error procesando audio")
        raise HTTPException(status_code=500, detail=str(e))


@app.post("/chat/texto")
async def chat_texto(mensaje: str = Form(...), device_id: str = Form("default")):
    """
    Endpoint de prueba: en vez de mandar audio, mandas texto directamente.
    Útil para probar la API sin tener el ESP32 a la mano.
    """
    try:
        logger.info(f"[{device_id}] Texto recibido: {mensaje}")

        texto_respuesta = generar_respuesta(device_id, [mensaje])
        logger.info(f"[{device_id}] Gemini respondió: {texto_respuesta}")

        historial = historiales.get(device_id, [])
        historial.insert(-1, "Usuario: " + mensaje)
        historiales[device_id] = historial[-MAX_HISTORIAL:]

        return respuesta_con_audio(texto_respuesta)

    except Exception as e:
        logger.exception("Error procesando texto")
        raise HTTPException(status_code=500, detail=str(e))


@app.post("/reset")
def reset(device_id: str = Form("default")):
    """Borra el historial de conversación de un dispositivo (reinicia la memoria)."""
    historiales.pop(device_id, None)
    return {"status": "historial borrado", "device_id": device_id}
