#!/usr/bin/env python3
"""
Simple MEV Trading Bot Example
"""

import asyncio
import sys
import os

# Add clients to path
sys.path.append(os.path.join(os.path.dirname(__file__), '../clients/python'))

from mev_shield_client import MEVShieldClient

class SimpleTradingBot:
    def __init__(self):
        self.client = MEVShieldClient()
        self.running = True
    
    async def handle_opportunity(self, opportunity):
        """Process a new MEV opportunity"""
        print(f"New opportunity: {opportunity.get('type', 'unknown')}")
        print(f"Profit: {opportunity.get('profit_eth', 0):.4f} ETH")
        # Add your trading logic here
    
    async def monitor_opportunities(self):
        """Monitor for new opportunities via WebSocket"""
        websocket = await self.client.connect_websocket()
        if not websocket:
            print("Failed to connect to WebSocket")
            return
        
        try:
            while self.running:
                message = await websocket.recv()
                opportunity = json.loads(message)
                await self.handle_opportunity(opportunity)
        except Exception as e:
            print(f"WebSocket error: {e}")
    
    async def run(self):
        """Main bot loop"""
        print("Starting MEV Trading Bot...")
        
        # Check server status
        status = self.client.get_status()
        if status:
            print(f"Server status: {status}")
        
        # Start monitoring
        await self.monitor_opportunities()

if __name__ == "__main__":
    bot = SimpleTradingBot()
    asyncio.run(bot.run())
