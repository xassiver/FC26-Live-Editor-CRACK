# FC26 Live Editor Bypass Injector

A lightweight, automated DLL injector specifically designed to bypass the compatibility issues of FC26 Live Editor on non-original (cracked) versions of EA Sports FC 26. 

Since recent versions of the FC26 Live Editor do not natively support cracked game executables, this tool automates the injection process with a customizable delay, ensuring the Live Editor loads successfully every time you start your game.

---

## Features

* **Set & Forget Configuration:** Saves your paths automatically to a `config.cfg` file after the first setup.
* **Smart Detection:** Automatically detects when the FC26 executable is launched and prepares for injection.
* **Customizable Delay:** Features a built-in waiting period (Default: 100ms) before injection to prevent crashes and ensure the game has fully initialized.
* **Lightweight & Fast:** Minimalistic console/GUI footprint with no performance impact on your gameplay.

---

## How It Works

1. **Initial Setup:** On the first launch, the app will ask you to provide the path for your **FC26 Launch Executable** (`.exe`) and the **FC26 Live Editor DLL** (`FCLiveEditor.DLL`).
2. **Configuration Saving:** These paths are saved to a local `config.cfg` file so you don't have to re-enter them.
3. **Active Listening:** Every time you run the injector, it will wait patiently in the background for you to start FC26.
4. **Delayed Injection:** Once the game process is detected, the tool waits for your specified delay (e.g., 100ms) and safely injects the DLL into the game.

---

## Download

> 📌 **Looking for the latest version?**  
> You can download the compiled executable from the **[Releases](https://github.com/xassiver/FC26-Live-Editor-CRACK/releases)** section of this GitHub repository.

---

## How to Use

1. Download the latest release and extract it to any folder.
2. Run the injector.
3. Provide the requested paths:
   * Select your FC26 game executable.
   * Select the `FCLiveEditor.DLL`.
4. Leave the injector open. It will say it's waiting for the game.
5. Launch your FC26 game.
6. The tool will automatically inject the Live Editor and close/minimize itself depending on your setup.

*Note: If you experience crashes during the loading screen, try increasing the delay time in the `config.cfg` file (e.g., change `100` to `200` or `500` ms).*

---

## Disclaimer

This project is intended strictly for educational and personal compatibility purposes. It is not affiliated with, endorsed by, or associated with Electronic Arts (EA) or the official developers of the FC26 Live Editor. Use it at your own risk.