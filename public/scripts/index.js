const jsonUrl = 'http://3.84.149.254/messages'; // Cambiar por la IP de tu servidor
const wsUrl = 'wss://3.84.149.254:443'; // WebSocket en tu instancia EC2

const tcpInput = document.getElementById('tcpInput');
const udpInput = document.getElementById('udpInput');
const errorMessage = document.getElementById('errorMessage');

// Cargar mensajes históricos desde /messages
async function fetchMessages() {
    try {
        const response = await fetch(jsonUrl);
        if (!response.ok) throw new Error('Error al cargar el archivo JSON');

        const data = await response.json();
        tcpInput.value = (data.tcp && data.tcp.length)
            ? data.tcp[data.tcp.length - 1] // Último mensaje TCP
            : 'No hay mensajes TCP.';
        udpInput.value = (data.udp && data.udp.length)
            ? data.udp[data.udp.length - 1] // Último mensaje UDP
            : 'No hay mensajes UDP.';
        errorMessage.innerText = '';
    } catch (error) {
        console.error(error);
        errorMessage.innerText = 'Error al cargar los mensajes históricos: ' + error.message;
    }
}

// Conectar al WebSocket para mensajes en tiempo real
const ws = new WebSocket(wsUrl);

ws.onopen = () => {
    console.log('Conectado al servidor WebSocket.');
};

ws.onmessage = (event) => {
    try {
        const { type, message } = JSON.parse(event.data);
        if (type === 'tcp') {
            tcpInput.value = message; // Actualizar el último mensaje TCP
        } else if (type === 'udp') {
            udpInput.value = message; // Actualizar el último mensaje UDP
        }
    } catch (error) {
        console.error('Error procesando el mensaje del WebSocket:', error);
    }
};

ws.onerror = (error) => {
    console.error('Error en el WebSocket:', error);
};

ws.onclose = () => {
    console.warn('Conexión WebSocket cerrada.');
    errorMessage.innerText = 'Conexión WebSocket cerrada.';
};

// Cargar mensajes al cargar la página
fetchMessages();

// Actualizar mensajes históricos cada 2 segundos
setInterval(fetchMessages, 2000);