from flask import Flask, send_file, jsonify
import requests
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime
import io

app = Flask(__name__)
C_API_URL = "http://localhost:8080/api/results"

@app.route('/graph/<int:home_id>')
def get_graph(home_id):
    try:
        # Fetch data from C API
        resp = requests.get(f"{C_API_URL}/{home_id}", timeout=5)
        if resp.status_code != 200:
            return jsonify({"error": f"API returned {resp.status_code}"}), 500
        
        data = resp.json()
        
        # Extract plotting data
        solar = [slot['solar_kwh'] for slot in data['slots']]
        prices = [slot['grid_price'] for slot in data['slots']]
        
        # Create dual-axis plot
        fig, ax1 = plt.subplots(figsize=(15, 6))
        
        x = range(96)
        
        # Solar production (bars)
        ax1.bar(x, solar, alpha=0.6, color='orange', label='Solar Production')
        ax1.set_xlabel('Time Slot (15-min intervals)', fontsize=12)
        ax1.set_ylabel('Solar Production (kWh)', color='orange', fontsize=12)
        ax1.tick_params(axis='y', labelcolor='orange')
        ax1.grid(True, alpha=0.3)
        
        # Grid price (line)
        ax2 = ax1.twinx()
        ax2.plot(x, prices, color='blue', linewidth=2, label='Grid Price', marker='.')
        ax2.set_ylabel('Grid Price (SEK/kWh)', color='blue', fontsize=12)
        ax2.tick_params(axis='y', labelcolor='blue')
        
        # Title and legend
        plt.title(f'Solar Production vs Grid Price - Home {home_id}\n'
                  f'Total Solar: {data["total_solar_kwh"]:.2f} kWh | '
                  f'Avg Price: {data["avg_grid_price"]:.2f} SEK/kWh', 
                  fontsize=14)
        
        fig.legend(loc='upper left', bbox_to_anchor=(0.1, 0.95))
        plt.tight_layout()
        
        # Save to bytes buffer
        img = io.BytesIO()
        plt.savefig(img, format='png', dpi=100, bbox_inches='tight')
        img.seek(0)
        plt.close()
        
        return send_file(img, mimetype='image/png')
        
    except requests.exceptions.RequestException as e:
        return jsonify({"error": f"Failed to connect to C API: {str(e)}"}), 500
    except Exception as e:
        return jsonify({"error": f"Internal error: {str(e)}"}), 500

@app.route('/health')
def health():
    return jsonify({"status": "ok", "service": "graph-api"})

if __name__ == '__main__':
    print("Starting Graph API on http://localhost:5000")
    print("Example: http://localhost:5000/graph/1")
    app.run(host='0.0.0.0', port=5000, debug=True)