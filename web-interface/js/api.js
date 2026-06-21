/**
 * 🌐 API Manager - Maneja las comunicaciones con la API de Gemini
 */

class GeminiAPI {
    constructor() {
        // Configuración por defecto
        this.config = {
            apiUrl: 'http://localhost:8000',
            deviceId: 'web-interface',
            timeout: 60000 // 60 segundos
        };
        
        // Cargar configuración guardada
        this.loadConfig();
    }

    /**
     * Carga la configuración desde localStorage
     */
    loadConfig() {
        const savedConfig = localStorage.getItem('geminiApiConfig');
        if (savedConfig) {
            try {
                const parsed = JSON.parse(savedConfig);
                this.config = { ...this.config, ...parsed };
            } catch (e) {
                console.warn('Error al cargar configuración guardada:', e);
            }
        }
    }

    /**
     * Guarda la configuración en localStorage
     */
    saveConfig() {
        localStorage.setItem('geminiApiConfig', JSON.stringify(this.config));
    }

    /**
     * Actualiza la configuración
     * @param {object} newConfig 
     */
    updateConfig(newConfig) {
        this.config = { ...this.config, ...newConfig };
        this.saveConfig();
    }

    /**
     * Obtiene la configuración actual
     */
    getConfig() {
        return { ...this.config };
    }

    /**
     * Verifica la salud del servidor
     * @returns {Promise<object>}
     */
    async checkHealth() {
        try {
            const response = await fetch(`${this.config.apiUrl}/health`, {
                method: 'GET',
                signal: AbortSignal.timeout(5000) // 5 segundos timeout para health
            });

            if (!response.ok) {
                throw new Error(`HTTP ${response.status}: ${response.statusText}`);
            }

            const data = await response.json();
            return { success: true, data };
        } catch (error) {
            console.error('Error al verificar salud del servidor:', error);
            return { 
                success: false, 
                error: error.message || 'No se pudo conectar con el servidor'
            };
        }
    }

    /**
     * Envía un mensaje de texto a la API
     * @param {string} message - Mensaje a enviar
     * @returns {Promise<object>} - { success, audioBlob, text, error }
     */
    async sendMessage(message) {
        try {
            const formData = new FormData();
            formData.append('mensaje', message);
            formData.append('device_id', this.config.deviceId);

            const response = await fetch(`${this.config.apiUrl}/chat/texto`, {
                method: 'POST',
                body: formData,
                signal: AbortSignal.timeout(this.config.timeout)
            });

            if (!response.ok) {
                const errorText = await response.text();
                throw new Error(`HTTP ${response.status}: ${errorText}`);
            }

            // Obtener el audio como blob
            const audioBlob = await response.blob();

            // Extraer el texto de respuesta del header (viene en base64)
            const textB64 = response.headers.get('X-Response-Text-B64');
            let responseText = 'Sin respuesta de texto';
            
            if (textB64) {
                try {
                    responseText = atob(textB64);
                } catch (e) {
                    console.warn('Error al decodificar texto de respuesta:', e);
                }
            }

            return {
                success: true,
                audioBlob,
                text: responseText
            };

        } catch (error) {
            console.error('Error al enviar mensaje:', error);
            
            // Mensajes de error más amigables
            let errorMessage = 'Error al comunicarse con el servidor';
            
            if (error.name === 'AbortError' || error.name === 'TimeoutError') {
                errorMessage = 'El servidor tardó mucho en responder. Intenta de nuevo.';
            } else if (error.message.includes('Failed to fetch')) {
                errorMessage = 'No se pudo conectar con el servidor. Verifica la URL en configuración.';
            } else if (error.message.includes('503')) {
                errorMessage = 'El servidor está saturado. Intenta de nuevo en unos segundos.';
            } else if (error.message.includes('500')) {
                errorMessage = 'Error interno del servidor. Verifica los logs de la API.';
            }

            return {
                success: false,
                error: errorMessage,
                details: error.message
            };
        }
    }

    /**
     * Reinicia la conversación (borra el historial)
     * @returns {Promise<object>}
     */
    async resetConversation() {
        try {
            const formData = new FormData();
            formData.append('device_id', this.config.deviceId);

            const response = await fetch(`${this.config.apiUrl}/reset`, {
                method: 'POST',
                body: formData,
                signal: AbortSignal.timeout(5000)
            });

            if (!response.ok) {
                throw new Error(`HTTP ${response.status}: ${response.statusText}`);
            }

            const data = await response.json();
            return { success: true, data };

        } catch (error) {
            console.error('Error al reiniciar conversación:', error);
            return {
                success: false,
                error: 'Error al reiniciar la conversación'
            };
        }
    }

    /**
     * Reproduce un blob de audio
     * @param {Blob} audioBlob - Blob de audio WAV
     * @returns {Promise<number>} - Duración del audio en milisegundos
     */
    playAudio(audioBlob) {
        return new Promise((resolve, reject) => {
            const audioUrl = URL.createObjectURL(audioBlob);
            const audio = new Audio(audioUrl);

            audio.onloadedmetadata = () => {
                const duration = audio.duration * 1000; // Convertir a milisegundos
                
                audio.play()
                    .then(() => {
                        // Limpiar URL cuando termine
                        audio.onended = () => {
                            URL.revokeObjectURL(audioUrl);
                            resolve(duration);
                        };
                    })
                    .catch(error => {
                        URL.revokeObjectURL(audioUrl);
                        reject(error);
                    });
            };

            audio.onerror = (error) => {
                URL.revokeObjectURL(audioUrl);
                reject(error);
            };
        });
    }
}

// Crear instancia global
const geminiAPI = new GeminiAPI();
