# S_P_A_R_K - Nature-Inspired Smart Energy Monitor

**SPARK** is an elegant, real-time web-based platform that provides granular visibility into energy consumption. By interfacing with smart hardware (e.g., ESP8266), SPARK translates raw telemetry data into clear, actionable, and aesthetic insights to help users balance their load control and manage energy footprints effectively.

![SPARK UI Theme](https://lh3.googleusercontent.com/aida-public/AB6AXuBIuXcIniwPEF4atI-3_NGhdKHMvK-TPxqivKuBkIatoIWXr50XqUcbrLJgmoh_3KXOLgFuqqzd2106MOCwIg4HgNtHQzogEMvdsaPM0jJ5LKf3jOczw89rnB4-3yXI4PSz1NCacaH1oFJrrhgWFc9o1AwHmLNCWB5DgmrrssVcdjkL08xMYf-t_Ai9-3t8xMbmbDrE05lqMx4QWudsL-Hd0CGUZAo2XJkH98RT3vAEE31mK7kq8lq4k41iNQhaGb_RTyZsdb-Z6vk) *(Thematic Background)*

## 🌲 Features

*   **Real-Time Data Visibility:** Live rendering of Voltage, Current, Power, and calculated Energy use & Bills via the Firebase Realtime Database.
*   **Environmental Context:** Tracks and displays external influencing factors like Temperature and Humidity.
*   **Functional Load Control:** Direct integration to actuate physical relays/appliances remotely (turning loads ON and OFF seamlessly).
*   **Aesthetic UX:** Built with a profound "forest morning" motif, integrating robust 'Glassmorphism' CSS to float translucent measurement modules atop the vivid background.
*   **Single-Page Architecture:** Fuses a secured Login portal dynamically rendering into the Real-Time Dashboard without hard-reloads or routing complications (perfect for direct AP/IoT hosting).

## 🛠️ Technology Stack

*   **Frontend:** HTML5, Vanilla JavaScript, Tailwind CSS (via CDN)
*   **Typography:** Google Fonts (`Cinzel` for Serifs, `Nunito` for Sans-Serif)
*   **Backend / Database:** Firebase SDK (v10 Modular) leveraging the Realtime Database to ensure <2 second telemetry latency.
*   **Hardware Compatibility:** Built specifically to scale with Wi-Fi capable microcontrollers like ESP8266 or ESP32 pushing JSON objects to Firebase.

## 🗄️ Firebase Realtime Database Structure

The UI dynamically reacts and writes to the structure mapped below. The ESP8266/Hardware unit must write JSON payloads mapping to these paths:

```json
{
  "energy": {
    "voltage": 120.5,
    "current": 2.3,
    "power": 277.15,
    "energy": 997740,
    "bill": 14.50
  },
  "environment": {
    "temperature": 24.5,
    "humidity": 58
  },
  "control": {
    "load": true
  }
}
```

*Note: Toggling the Load buttons on the UI dashboard injects a boolean (`true`/`false`) directly to the `/control/load` path for the hardware to actuate.*

## 🚀 Setup & Execution

Since the SPARK frontend behaves seamlessly as a client-side Single Page Application (SPA) leveraging Firebase CDNs, zero heavy bundlers are required.

To run locally:
1. Clone the repository natively.
2. Spin up a local static server on the root directory.
   ```bash
   npx serve .
   ```
   *(Or just use `python -m http.server 3000`, or Live Server on VS Code)*.
3. Open a browser context at `http://localhost:3000`. 
4. Accept the interactive physical Toggle on the login screen to unlock the Dashboard parameters.

---
*Created as part of the VoltStream SPARK Smart Energy Project.*