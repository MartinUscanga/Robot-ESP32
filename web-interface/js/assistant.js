/**
 * 🤖 Assistant Manager - Maneja el estado y animaciones del personaje
 */

class AssistantManager {
    constructor() {
        this.character = document.getElementById('character');
        this.statusText = document.getElementById('statusText');
        this.currentState = 'idle';
        this.stateMessages = {
            idle: '¡Hola! Soy tu asistente',
            listening: 'Escuchando...',
            thinking: 'Pensando...',
            talking: 'Hablando...',
            happy: '¡Me alegro de ayudarte!',
            error: 'Ups, algo salió mal...',
            waving: '¡Hola! 👋'
        };
    }

    /**
     * Cambia el estado del personaje
     * @param {string} state - Estado: idle, listening, thinking, talking, happy, error, waving
     */
    setState(state) {
        // Remover estado anterior
        this.character.classList.remove(
            'idle', 'listening', 'thinking', 'talking', 'happy', 'error', 'waving', 'surprised'
        );
        
        // Agregar nuevo estado
        this.character.classList.add(state);
        this.currentState = state;
        
        // Actualizar texto de estado
        if (this.stateMessages[state]) {
            this.statusText.textContent = this.stateMessages[state];
        }
    }

    /**
     * Establece un mensaje personalizado de estado
     * @param {string} message 
     */
    setStatusMessage(message) {
        this.statusText.textContent = message;
    }

    /**
     * Animación de saludo
     */
    greet() {
        this.setState('waving');
        setTimeout(() => {
            this.setState('idle');
        }, 2000);
    }

    /**
     * Animación de celebración
     */
    celebrate() {
        this.setState('happy');
        setTimeout(() => {
            this.setState('idle');
        }, 1500);
    }

    /**
     * Muestra error
     */
    showError() {
        this.setState('error');
        setTimeout(() => {
            this.setState('idle');
        }, 2000);
    }

    /**
     * Simula que está escuchando
     */
    listen() {
        this.setState('listening');
    }

    /**
     * Simula que está pensando
     */
    think() {
        this.setState('thinking');
    }

    /**
     * Simula que está hablando
     */
    talk() {
        this.setState('talking');
    }

    /**
     * Vuelve al estado idle
     */
    idle() {
        this.setState('idle');
    }

    /**
     * Anima al personaje mientras habla (durante reproducción de audio)
     * @param {number} duration - Duración en milisegundos
     */
    animateWhileTalking(duration) {
        this.talk();
        setTimeout(() => {
            this.idle();
        }, duration);
    }
}

// Crear instancia global
const assistant = new AssistantManager();
