"""
Cliente de prueba para el puente ESP32 <-> Gemini.

Te permite probar la API completa SIN tener el ESP32 conectado,
desde tu computadora.

Uso:
    # Probar con texto (más fácil, no necesitas micrófono)
    python test_client.py --texto "Hola robot, ¿cómo estás?"

    # Probar con un archivo de audio (simula lo que mandaría el ESP32)
    python test_client.py --audio grabacion.wav

    # Cambiar la URL si tu API ya está en el VPS
    python test_client.py --url http://tu-vps-ip:8000 --texto "Hola"
"""

import argparse
import base64
import sys
from pathlib import Path

import requests


def mostrar_respuesta(response: requests.Response, guardar_como: str = "respuesta.wav"):
    if response.status_code != 200:
        print(f"❌ Error {response.status_code}: {response.text}")
        sys.exit(1)

    texto_b64 = response.headers.get("X-Response-Text-B64", "")
    if texto_b64:
        texto = base64.b64decode(texto_b64).decode("utf-8")
        print(f"\n🤖 Robot dice: {texto}\n")

    Path(guardar_como).write_bytes(response.content)
    print(f"🔊 Audio de respuesta guardado en: {guardar_como}")
    print("   (ábrelo con cualquier reproductor para escucharlo)")


def probar_con_texto(url: str, mensaje: str, device_id: str):
    print(f"📤 Enviando texto: \"{mensaje}\"")
    response = requests.post(
        f"{url}/chat/texto",
        data={"mensaje": mensaje, "device_id": device_id},
        timeout=60,
    )
    mostrar_respuesta(response)


def probar_con_audio(url: str, ruta_audio: str, device_id: str):
    ruta = Path(ruta_audio)
    if not ruta.exists():
        print(f"❌ No encontré el archivo: {ruta_audio}")
        sys.exit(1)

    print(f"📤 Enviando audio: {ruta_audio}")
    with open(ruta, "rb") as f:
        response = requests.post(
            f"{url}/chat",
            files={"file": (ruta.name, f, "audio/wav")},
            data={"device_id": device_id},
            timeout=60,
        )
    mostrar_respuesta(response)


def probar_salud(url: str):
    response = requests.get(f"{url}/health", timeout=10)
    print(f"✅ Servidor activo: {response.json()}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Cliente de prueba del puente ESP32 <-> Gemini")
    parser.add_argument("--url", default="http://localhost:8000", help="URL de la API")
    parser.add_argument("--texto", help="Mensaje de texto a enviar (modo de prueba sin audio)")
    parser.add_argument("--audio", help="Ruta a un archivo .wav para enviar como si fuera el ESP32")
    parser.add_argument("--device-id", default="prueba-local", help="Identificador del dispositivo")
    parser.add_argument("--salud", action="store_true", help="Solo verificar que el servidor responde")

    args = parser.parse_args()

    if args.salud:
        probar_salud(args.url)
    elif args.texto:
        probar_con_texto(args.url, args.texto, args.device_id)
    elif args.audio:
        probar_con_audio(args.url, args.audio, args.device_id)
    else:
        print("Usa --texto \"mensaje\", --audio archivo.wav, o --salud")
        print("Ejemplo: python test_client.py --texto \"Hola robot\"")
