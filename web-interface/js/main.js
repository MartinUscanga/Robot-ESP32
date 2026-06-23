/**
 * 💬 Main App Logic - Orquesta toda la aplicación
 */

class ChatApp {
    constructor() {
        this.messagesArea = document.getElementById('messagesArea');
        this.messageInput = document.getElementById('messageInput');
        this.chatForm = document.getElementById('chatForm');
        this.sendButton = document.getElementById('sendButton');
        this.resetButton = document.getElementById('resetButton');
        this.connectionStatus = document.getElementById('connectionStatus');
        
        // Configuración
        this.configButton = document.getElementById('configButton');
        this.configModal = document.getElementById('configModal');
        this.apiUrlInput = document.getElementById('apiUrl');
        this.deviceIdInput = document.getElementById('deviceId');
        this.saveConfigBtn = document.getElementById('saveConfig');
        this.closeConfigBtn = document.getElementById('closeConfig');
        
        this.isProcessing = false;
        
        this.init();
    }

    /**
     * Inicializa la aplicación
     */
    init() {
        // Cargar configuración en los inputs
        const config = geminiAPI.getConfig();
        this.apiUrlInput.value = config.apiUrl;
        this.deviceIdInput.value = config.deviceId;
        
        // Event listeners
        this.chatForm.addEventListener('submit', (e) => this.handleSendMessage(e));
        this.resetButton.addEventListener('click', () => this.handleReset());
        
        // Configuración
        this.configButton.addEventListener('click', () => this.openConfigModal());
        this.closeConfigBtn.addEventListener('click', () => this.closeConfigModal());
        this.saveConfigBtn.addEventListener('click', () => this.saveConfiguration());
        
        // Cerrar modal al hacer click fuera
        this.configModal.addEventListener('click', (e) => {
            if (e.target === this.configModal) {
                this.closeConfigModal();
            }
        });
        
        // Verificar conexión al cargar
        this.checkConnection();
        
        // Saludo inicial
        setTimeout(() => {
            assistant.greet();
        }, 500);
        
        // Auto-focus en el input
        this.messageInput.focus();
    }

    /**
     * Verifica la conexión con el servidor
     */
    async checkConnection() {
        const result = await geminiAPI.checkHealth();
        
        if (result.success) {
            this.setConnectionStatus(true, 'Conectado');
            console.log('✅ Servidor conectado:', result.data);
        } else {
            this.setConnectionStatus(false, 'Desconectado');
            console.warn('❌ Servidor no disponible:', result.error);
        }
    }

    /**
     * Actualiza el estado de conexión en la UI
     * @param {boolean} connected 
     * @param {string} label 
     */
    setConnectionStatus(connected, label) {
        const statusDot = this.connectionStatus.querySelector('.status-dot');
        const statusLabel = this.connectionStatus.querySelector('.status-label');
        
        statusLabel.textContent = label;
        
        if (connected) {
            statusDot.style.background = 'var(--success)';
        } else {
            statusDot.style.background = '#FF6B6B';
        }
    }

    /**
     * Maneja el envío de mensajes
     * @param {Event} e 
     */
    async handleSendMessage(e) {
        e.preventDefault();
        
        const message = this.messageInput.value.trim();
        
        if (!message || this.isProcessing) {
            return;
        }
        
        // Limpiar input y deshabilitar
        this.messageInput.value = '';
        this.setProcessing(true);
        
        // Mostrar mensaje del usuario
        this.addMessage(message, 'user');
        
        // Animar personaje - escuchando
        assistant.listen();
        
        // Pequeña pausa para animación
        await this.sleep(500);
        
        // Mostrar indicador de carga
        const loadingId = this.addLoadingMessage();
        
        // Cambiar a pensando
        assistant.think();
        
        try {
            // Enviar mensaje a la API
            const result = await geminiAPI.sendMessage(message);
            
            // Remover indicador de carga
            this.removeLoadingMessage(loadingId);
            
            if (result.success) {
                // Mostrar respuesta de texto
                this.addMessage(result.text, 'assistant');
                
                // Reproducir audio y animar
                assistant.talk();
                
                try {
                    const duration = await geminiAPI.playAudio(result.audioBlob);
                    
                    // Mantener animación durante el audio
                    setTimeout(() => {
                        assistant.celebrate();
                    }, duration);
                    
                } catch (audioError) {
                    console.error('Error al reproducir audio:', audioError);
                    assistant.idle();
                    this.showNotification('⚠️ No se pudo reproducir el audio', 'warning');
                }
                
            } else {
                // Error en la API
                assistant.showError();
                this.addMessage(`❌ ${result.error}`, 'error');
                this.showNotification(result.error, 'error');
            }
            
        } catch (error) {
            console.error('Error inesperado:', error);
            this.removeLoadingMessage(loadingId);
            assistant.showError();
            this.addMessage('❌ Ocurrió un error inesperado', 'error');
            this.showNotification('Error inesperado al procesar tu mensaje', 'error');
        }
        
        // Re-habilitar input
        this.setProcessing(false);
        this.messageInput.focus();
    }

    /**
     * Agrega un mensaje al chat
     * @param {string} text 
     * @param {string} type - 'user', 'assistant', 'error'
     */
    addMessage(text, type) {
        const messageDiv = document.createElement('div');
        messageDiv.className = `message ${type}`;
        
        const now = new Date();
        const timeStr = now.toLocaleTimeString('es-MX', { 
            hour: '2-digit', 
            minute: '2-digit' 
        });
        
        messageDiv.innerHTML = `
            <div class="message-bubble">
                <div class="message-text">${this.escapeHtml(text)}</div>
                <div class="message-time">${timeStr}</div>
            </div>
        `;
        
        this.messagesArea.appendChild(messageDiv);
        this.scrollToBottom();
        
        return messageDiv;
    }

    /**
     * Agrega un indicador de carga
     * @returns {string} ID del indicador
     */
    addLoadingMessage() {
        const loadingId = 'loading-' + Date.now();
        const loadingDiv = document.createElement('div');
        loadingDiv.className = 'message assistant';
        loadingDiv.id = loadingId;
        
        loadingDiv.innerHTML = `
            <div class="loading-message">
                <div class="loading-dots">
                    <div class="loading-dot"></div>
                    <div class="loading-dot"></div>
                    <div class="loading-dot"></div>
                </div>
            </div>
        `;
        
        this.messagesArea.appendChild(loadingDiv);
        this.scrollToBottom();
        
        return loadingId;
    }

    /**
     * Remueve el indicador de carga
     * @param {string} loadingId 
     */
    removeLoadingMessage(loadingId) {
        const loadingDiv = document.getElementById(loadingId);
        if (loadingDiv) {
            loadingDiv.remove();
        }
    }

    /**
     * Maneja el reset de la conversación
     */
    async handleReset() {
        if (!confirm('¿Estás seguro de que quieres reiniciar la conversación? Se borrará todo el historial.')) {
            return;
        }
        
        assistant.think();
        
        const result = await geminiAPI.resetConversation();
        
        if (result.success) {
            // Limpiar mensajes
            this.messagesArea.innerHTML = `
                <div class="welcome-message">
                    <div class="sparkle">✨</div>
                    <p>¡Conversación reiniciada!</p>
                    <p>¿En qué puedo ayudarte?</p>
                </div>
            `;
            
            assistant.celebrate();
            this.showNotification('✅ Conversación reiniciada', 'success');
        } else {
            assistant.showError();
            this.showNotification('❌ Error al reiniciar la conversación', 'error');
        }
    }

    /**
     * Habilita/deshabilita el modo de procesamiento
     * @param {boolean} processing 
     */
    setProcessing(processing) {
        this.isProcessing = processing;
        this.sendButton.disabled = processing;
        this.messageInput.disabled = processing;
        this.resetButton.disabled = processing;
        
        if (processing) {
            this.sendButton.querySelector('.button-text').textContent = 'Enviando...';
        } else {
            this.sendButton.querySelector('.button-text').textContent = 'Enviar';
        }
    }

    /**
     * Abre el modal de configuración
     */
    openConfigModal() {
        this.configModal.classList.add('active');
    }

    /**
     * Cierra el modal de configuración
     */
    closeConfigModal() {
        this.configModal.classList.remove('active');
    }

    /**
     * Guarda la configuración
     */
    async saveConfiguration() {
        const apiUrl = this.apiUrlInput.value.trim();
        const deviceId = this.deviceIdInput.value.trim();
        
        if (!apiUrl) {
            alert('❌ La URL de la API es requerida');
            return;
        }
        
        if (!deviceId) {
            alert('❌ El ID del dispositivo es requerido');
            return;
        }
        
        // Actualizar configuración
        geminiAPI.updateConfig({
            apiUrl,
            deviceId
        });
        
        this.showNotification('✅ Configuración guardada', 'success');
        this.closeConfigModal();
        
        // Verificar nueva conexión
        await this.checkConnection();
    }

    /**
     * Muestra una notificación temporal
     * @param {string} message 
     * @param {string} type - 'success', 'error', 'warning'
     */
    showNotification(message, type = 'info') {
        // Crear notificación
        const notification = document.createElement('div');
        notification.className = `notification ${type}`;
        notification.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            background: ${type === 'success' ? 'var(--success)' : type === 'error' ? '#FF6B6B' : 'var(--warning)'};
            color: var(--text-dark);
            padding: 1rem 1.5rem;
            border-radius: var(--radius-md);
            box-shadow: 0 4px 20px var(--shadow-strong);
            z-index: 10000;
            animation: slideIn 0.3s ease-out;
            font-weight: 600;
            max-width: 300px;
        `;
        notification.textContent = message;
        
        document.body.appendChild(notification);
        
        // Remover después de 3 segundos
        setTimeout(() => {
            notification.style.animation = 'slideOut 0.3s ease-out';
            setTimeout(() => {
                notification.remove();
            }, 300);
        }, 3000);
    }

    /**
     * Scroll al final del chat
     */
    scrollToBottom() {
        setTimeout(() => {
            this.messagesArea.scrollTop = this.messagesArea.scrollHeight;
        }, 100);
    }

    /**
     * Escapa HTML para prevenir XSS
     * @param {string} text 
     */
    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }

    /**
     * Utilidad para pausas
     * @param {number} ms 
     */
    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
}

// Inicializar cuando el DOM esté listo
document.addEventListener('DOMContentLoaded', () => {
    const app = new ChatApp();
    console.log('✨ Asistente Kawaii Gemini iniciado!');
});

// Animación de notificaciones
const style = document.createElement('style');
style.textContent = `
    @keyframes slideOut {
        from {
            opacity: 1;
            transform: translateX(0);
        }
        to {
            opacity: 0;
            transform: translateX(100%);
        }
    }
`;
document.head.appendChild(style);
