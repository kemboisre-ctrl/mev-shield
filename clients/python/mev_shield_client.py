"""
MEV Shield Python Client
Simple client for connecting to MEV Shield API
"""

import requests
import json
import asyncio
import websockets
from typing import Dict, List, Optional

class MEVShieldClient:
    def __init__(self, host: str = "localhost", port: int = 8765):
        self.base_url = f"http://{host}:{port}"
        self.ws_url = f"ws://{host}:{port}/ws"
    
    async def connect_websocket(self):
        """Connect to WebSocket for real-time updates"""
        try:
            self.websocket = await websockets.connect(self.ws_url)
            return self.websocket
        except Exception as e:
            print(f"WebSocket connection failed: {e}")
            return None
    
    def get_status(self):
        """Get server status"""
        try:
            response = requests.get(f"{self.base_url}/status")
            return response.json()
        except Exception as e:
            print(f"Status request failed: {e}")
            return None
    
    def get_opportunities(self):
        """Get current MEV opportunities"""
        try:
            response = requests.get(f"{self.base_url}/opportunities")
            return response.json()
        except Exception as e:
            print(f"Opportunities request failed: {e}")
            return None
