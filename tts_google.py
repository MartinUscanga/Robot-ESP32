"""
Google Cloud Text-to-Speech Integration
Reemplazo de gTTS con voces neurales de alta calidad
"""

from google.cloud import texttospeech
from pathlib import Path
import os

# Configuración de voz
VOICE_CONFIG = {
    "language_code": os.getenv("TTS_LANGUAGE", "es-MX"),  # es-MX, es-ES, es-US
    "voice_name": os.getenv("TTS_VOICE_NAME", "es-MX-Neural2-B"),  # Voz masculina neural
    "gender": texttospeech.SsmlGender.MALE,  # MALE, FEMALE
    "speaking_rate": float(os.getenv("TTS_SPEAKING_RATE", "1.0")),  # 0.25 a 4.0
    "pitch": float(os.getenv("TTS_PITCH", "0.0")),  # -20 a 20
}

# Cliente global (reutilizable)
_tts_client = None

def get_tts_client():
    """Obtener cliente TTS (singleton)"""
    global _tts_client
    if _tts_client is None:
        _tts_client = texttospeech.TextToSpeechClient()
    return _tts_client


def texto_a_voz_google(texto: str, output_path: str = None) -> bytes:
    """
    Convierte texto a audio WAV usando Google Cloud TTS
    
    Args:
        texto: Texto a convertir
        output_path: Ruta opcional para guardar el archivo
        
    Returns:
        bytes: Audio en formato WAV PCM 16kHz mono
    """
    client = get_tts_client()
    
    # Input de síntesis
    synthesis_input = texttospeech.SynthesisInput(text=texto)
    
    # Configuración de voz
    voice = texttospeech.VoiceSelectionParams(
        language_code=VOICE_CONFIG["language_code"],
        name=VOICE_CONFIG["voice_name"],
        ssml_gender=VOICE_CONFIG["gender"]
    )
    
    # Configuración de audio
    audio_config = texttospeech.AudioConfig(
        audio_encoding=texttospeech.AudioEncoding.LINEAR16,  # WAV PCM
        sample_rate_hertz=16000,  # Compatible con ESP32
        speaking_rate=VOICE_CONFIG["speaking_rate"],
        pitch=VOICE_CONFIG["pitch"]
    )
    
    # Sintetizar
    response = client.synthesize_speech(
        input=synthesis_input,
        voice=voice,
        audio_config=audio_config
    )
    
    # Guardar si se especifica ruta
    if output_path:
        Path(output_path).write_bytes(response.audio_content)
    
    return response.audio_content


# Voces disponibles
VOCES_DISPONIBLES = {
    # Español México
    "es-MX-Neural2-A": {"genero": "FEMENINA", "tipo": "Neural", "calidad": "★★★★★"},
    "es-MX-Neural2-B": {"genero": "MASCULINA", "tipo": "Neural", "calidad": "★★★★★"},
    "es-MX-Wavenet-A": {"genero": "FEMENINA", "tipo": "WaveNet", "calidad": "★★★★"},
    "es-MX-Wavenet-B": {"genero": "MASCULINA", "tipo": "WaveNet", "calidad": "★★★★"},
    
    # Español España
    "es-ES-Neural2-A": {"genero": "FEMENINA", "tipo": "Neural", "calidad": "★★★★★"},
    "es-ES-Neural2-B": {"genero": "MASCULINA", "tipo": "Neural", "calidad": "★★★★★"},
    "es-ES-Neural2-C": {"genero": "FEMENINA", "tipo": "Neural", "calidad": "★★★★★"},
    "es-ES-Neural2-D": {"genero": "FEMENINA", "tipo": "Neural", "calidad": "★★★★★"},
    "es-ES-Neural2-E": {"genero": "MASCULINA", "tipo": "Neural", "calidad": "★★★★★"},
    
    # Español Estados Unidos
    "es-US-Neural2-A": {"genero": "FEMENINA", "tipo": "Neural", "calidad": "★★★★★"},
    "es-US-Neural2-B": {"genero": "MASCULINA", "tipo": "Neural", "calidad": "★★★★★"},
    "es-US-Neural2-C": {"genero": "MASCULINA", "tipo": "Neural", "calidad": "★★★★★"},
}


def listar_voces():
    """Muestra las voces disponibles"""
    print("\n🎤 Voces Disponibles para Google Cloud TTS:\n")
    for nombre, info in VOCES_DISPONIBLES.items():
        print(f"  {nombre}")
        print(f"    Género: {info['genero']}")
        print(f"    Tipo: {info['tipo']}")
        print(f"    Calidad: {info['calidad']}\n")


# Testing
if __name__ == "__main__":
    print("🎤 Testing Google Cloud TTS...")
    
    # Listar voces
    listar_voces()
    
    # Probar síntesis
    texto_prueba = "Hola, soy JARVIS, tu asistente personal inteligente."
    
    print(f"Sintetizando: '{texto_prueba}'")
    audio = texto_a_voz_google(texto_prueba, "test_output.wav")
    
    print(f"✅ Audio generado: {len(audio)} bytes")
    print("   Guardado en: test_output.wav")
    print("\n💡 Para cambiar la voz, edita TTS_VOICE_NAME en .env")
