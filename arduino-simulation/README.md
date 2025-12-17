# Smart Blind Shoes - Real-Time Monitoring System

A comprehensive IoT solution for smart navigation shoes with real-time web dashboard, data visualization, and API integrations.

## Features

### Hardware Features
- **3x Ultrasonic Sensors** - Left, Center, Right obstacle detection
- **Advanced Filtering** - Moving average and outlier detection for reliable readings
- **Haptic Feedback** - Vibration patterns based on obstacle proximity
- **RGB LED Status** - Visual feedback with color-coded zones
- **LCD Display** - Multi-mode information display
- **Emergency Button** - Quick emergency alert system
- **Step Counting** - Pedometer functionality
- **Battery Monitoring** - Real-time battery level tracking

### Software Features
- **Real-Time Web Dashboard** - Live monitoring with graphs and charts
- **Data Visualization** - Chart.js powered graphs for sensor data
- **Google Maps Integration** - Real-time location tracking
- **Data Export** - CSV export for analysis
- **Web Serial API** - Direct browser connection to Arduino
- **JSON Data Stream** - Structured data output for web integration

## Setup Instructions

### 🎯 For Wokwi Simulator (Easiest!)

1. **Open your project in Wokwi** at https://wokwi.com
2. **Start the simulation** (click Play button)
3. **Open Serial Monitor** (bottom panel)
4. **Open `wokwi-dashboard.html`** in a new browser tab
5. **Copy JSON lines** from Serial Monitor (lines starting with `JSON:`)
6. **Paste into dashboard** and click "Process Data"
7. **See real-time graphs and visualizations!**

**Demo Mode:** Open `wokwi-dashboard.html?demo=true` to see simulated data

### 1. Arduino Setup (Real Hardware)

1. Open `sketch.ino` in Arduino IDE
2. Install required libraries:
   - LiquidCrystal_I2C (via Library Manager)
3. Upload to your Arduino Uno
4. Connect via USB to your computer

### 2. Web Dashboard Setup

#### Option A: Direct Web Serial (Recommended)

1. Open `dashboard.html` in **Chrome, Edge, or Opera** (Web Serial API support required)
2. Click "Connect to Device"
3. Select your Arduino COM port
4. Dashboard will start receiving real-time data

#### Option B: Node.js Server (For WebSocket)

1. Install Node.js dependencies:
   ```bash
   npm install express ws serialport
   ```

2. Run the server:
   ```bash
   node server.js
   ```

3. Open `http://localhost:3000` in your browser

### 3. Demo Mode

To test the dashboard without hardware:
- Open `dashboard.html?demo=true` in your browser
- The dashboard will simulate data automatically

## Dashboard Features

### Real-Time Graphs
- **Distance Over Time** - Line chart showing all three sensors
- **Zone Distribution** - Doughnut chart of zone occurrences
- **Interactive Zoom** - Pan and zoom on distance chart

### Live Statistics
- Sensor readings (Left, Center, Right)
- Step count and walking speed
- Distance traveled
- Battery level
- System uptime
- Sensitivity setting

### Location Tracking
- Real-time GPS coordinates
- Embedded Google Maps view
- Location history

### System Health
- Sensor status indicators
- Error detection
- Uptime monitoring

### Data Export
- Export all collected data as CSV
- Includes timestamps and all sensor readings
- Perfect for data analysis

## API Integration

The system supports integration with external APIs:

### Weather API (Optional)
Add weather context to emergency alerts:
```javascript
// Example: Get weather at current location
fetch(`https://api.openweathermap.org/data/2.5/weather?lat=${lat}&lon=${lng}&appid=YOUR_API_KEY`)
```

### Location Services
- Google Maps API for location visualization
- Reverse geocoding for address lookup
- Route planning integration

## Data Format

The Arduino outputs JSON data in this format:
```json
{
  "t": 12345,
  "sensors": {
    "left": 150,
    "center": 120,
    "right": 180,
    "closest": 120,
    "closestSensor": 1
  },
  "zones": {
    "warn": 150,
    "alert": 80,
    "critical": 30,
    "current": 1
  },
  "stats": {
    "steps": 1234,
    "speed": 3.5,
    "distance": 567.8
  },
  "system": {
    "battery": 85,
    "uptime": 3600,
    "sensitivity": 1.2
  },
  "gps": {
    "lat": 37.5514,
    "lng": 127.0748
  },
  "health": {
    "sensorLeft": 1,
    "sensorCenter": 1,
    "sensorRight": 1
  },
  "emergency": 0
}
```

## Hardware Connections

### Ultrasonic Sensors
- Left: TRIG=4, ECHO=5
- Center: TRIG=6, ECHO=7
- Right: TRIG=8, ECHO=3

### Output Devices
- Vibration Motor: Pin 9
- Buzzer: Pin 10
- RGB LED: Pins 11 (R), 12 (G), 13 (B)

### Input Devices
- Emergency Button: Pin 2
- Sensitivity Potentiometer: A1

### I2C Devices
- LCD Display: SDA=A4, SCL=A5

## Troubleshooting

### Web Serial Not Working
- Ensure you're using Chrome, Edge, or Opera
- Check that Arduino is connected and port is available
- Try disconnecting and reconnecting

### No Data Received
- Verify baud rate is 115200
- Check serial monitor for JSON output
- Ensure JSON_OUTPUT_ENABLED is true in code

### Charts Not Updating
- Check browser console for errors
- Verify JSON data format is correct
- Try refreshing the page

## Future Enhancements

- [ ] Machine learning for obstacle pattern recognition
- [ ] Cloud storage integration (Firebase, AWS)
- [ ] Mobile app companion
- [ ] Voice commands via speech recognition
- [ ] Social sharing of routes
- [ ] Integration with fitness trackers
- [ ] AI-powered route optimization

## License

This project is open source and available for educational purposes.

## Support

For issues or questions, please check the code comments or create an issue in the repository.

---

**Note**: This is a simulation project for Wokwi. For real hardware deployment, ensure proper power management and consider using a battery pack with appropriate voltage regulation.

