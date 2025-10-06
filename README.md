<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://gamejolt.com/games/fnafworldmod/861624">
    <img src="gear-icon-blueprinted.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">FWMod-Core</h3>

<p align="center">
  <strong>A powerful modding framework for FWMod</strong>
</p>

<p align="center">
  <a href="#-features">Features</a> •
  <a href="#%EF%B8%8F-installation">Installation</a> •
  <a href="#-usage">Usage</a> •
  <a href="#%EF%B8%8F-building">Building</a> •
  <a href="#-contributing">Contributing</a>
</p>

</div>

---

## 📖 About

FWMod-Core is comprehensive modding framework in development, designed specifically for [FWMod](https://github.com/fnafworldmodding/fwmod/tree/main). It provides modders with simple api a yet powerful enough to modify game raw form such as assets, code, configuration, inject custom functionality, in cases it's not possible at runtime!

### 🎯 Key Capabilities

- **Asset Management**: Replace and modify game images, fonts, sounds, and objects
- **CCN File Support**: sami support for the CCN format
- **Plugin System**: Extensible architecture for custom plugins
- **Binary I/O**: Advanced binary reading/writing for game data manipulation
- **Runtime Hooking**: Windows API hooking for advanced game modifications

## ✨ Features

### 🔧 Core Functionality
- [x] **Image Bank Management** - Replace and modify game textures and sprites
- [x] **Font Bank Support** - Replace and modify game font
- [x] **Object Management** - Modify game objects, properties.
- [x] **Binary Tools** - Custom BinaryReader and BinaryWriter classes

### 🚧 In Development
- [ ] **Sound Bank Support** - Audio asset management
- [ ] **Shader Management** - Custom shader injection
- [ ] **Animation Support** - Active animation modifications
- [ ] **ModStore API** - ModStore is a planned centralized system for tracking modifications

## 🛠️ Installation

### Prerequisites
- Windows 10/11 (x86)
- Visual Studio 2019 or later

### Quick Start
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/fwmod-core.git
   cd fwmod-core
   ```

2. Build the project:
   - Open `fwmod-core.sln` in Visual Studio
   - Select your target configuration (Debug/Release)
   - Build the solution

3. Deploy the modding framework:
   - Copy the generated `fwmod-core.dll` to fwmod directory and replace (NOTE depending on STL version you might need to recompile any plugins)
   - Place mods in the `mods/_core/preload/` directory

## 📚 Usage
no documentation created yet

## 🏗️ Building

### Requirements
- Visual Studio 2019+ with C++17 support
- Windows SDK 10.0

### Dependencies
- **LZ4** - Fast compression library
- **zlib** - Data compression
- **nlohmann/json** - JSON parsing

### Build Instructions
1. Open the solution in Visual Studio\
2. Build the solution (Ctrl+Shift+B)

## 📁 Project Structure
```
fwmod-core/
├── fwmod-core/ # Main source code
│ ├── GreenFreddyTools/ # Core utilities and parsers
│ │ ├── CCNParser/ # CCN format handling/reading
│ │ ├── BinaryTools/ # Binary I/O utilities
│ │ └── Utils/ # Helper utilities
│ ├── Hooks/ # Windows API hooking
│ ├── Loader/ # Plugin loading system
│ └── Logger/ # Logging system
|
```

## 🤝 Contributing

any contributions are welcome!


### Development Guidelines
remember to:
- Follow the existing code style
- Add comments for complex functionality
- Test your changes thoroughly
- Update documentation as needed

### 📋 TODO

See [TODO.md](TODO.md) for a comprehensive list of planned features and improvements.

## 📄 License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.txt](LICENSE.txt) file for details.

## 🙏 Acknowledgments

- thanks the FNaF World community for their support 
- Open source projects helped/used in this project
    - **[BinaryTools](https://github.com/Moneyl/BinaryTools)** - C++ tools for reading/writing binary data 
    - **[nlohmann/json](https://github.com/nlohmann/json)** - JSON for Modern C++, for modifications
    - **[zlib](https://github.com/madler/zlib)** - Data compression library, for chunks and objects compression
    - **[LZ4](https://github.com/lz4/lz4)** - Fast compression algorithm, lz4 for image decompression and compression
    - **[Nebula](https://github.com/AITYunivers/NebulaFD/tree/2024-nebula)** - reimagined decompiler for Clickteam Fusion, that helped with understanding the CCN format and decompiling it

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/programminglaboratorys/fwmod-core/issues)
- **Discord**: [no offical discord yet](https://gamejolt.com/games/fnafworldmod/861624)

---

<div align="center">
  <p>Made with ❤️ for the FNaF World modding community</p>
  <p><strong>Version 1.0.0a</strong></p>
</div>