/**
 * MEV Shield Node.js Client
 * Simple client for connecting to MEV Shield API
 */

const WebSocket = require('ws');
const axios = require('axios');

class MEVShieldClient {
    constructor(host = 'localhost', port = 8765) {
        this.baseUrl = `http://${host}:${port}`;
        this.wsUrl = `ws://${host}:${port}/ws`;
    }

    async getStatus() {
        try {
            const response = await axios.get(`${this.baseUrl}/status`);
            return response.data;
        } catch (error) {
            console.error('Status request failed:', error);
            return null;
        }
    }

    async getOpportunities() {
        try {
            const response = await axios.get(`${this.baseUrl}/opportunities`);
            return response.data;
        } catch (error) {
            console.error('Opportunities request failed:', error);
            return null;
        }
    }

    connectWebSocket() {
        return new Promise((resolve, reject) => {
            this.websocket = new WebSocket(this.wsUrl);
            
            this.websocket.on('open', () => {
                resolve(this.websocket);
            });
            
            this.websocket.on('error', (error) => {
                reject(error);
            });
        });
    }
}

module.exports = MEVShieldClient;
