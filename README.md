# ⚡ CLASSIC TERMINAL

**Classic Terminal** - lightweight, fast terminal emulator with tmux support, theme switching, and font customization. Styled like a classic green-on-black terminal.

---

## ✨ Features

- 🖥️ **Full terminal** - VTE-based, runs bash with tmux
- 🎨 **Color themes** - classic, dark, light, matrix, amber, white
- 🔤 **Font customization** - Monospace with adjustable size (12-24)
- ⌨️ **Keyboard shortcuts**:

  - `Ctrl+Shift++` - zoom in
  - `Ctrl+Shift+-` - zoom out
  - `Ctrl+0` - reset font size to 12

- 📁 **Separate sessions** - each window has its own tmux session
- 💾 **Config persistence** - theme and font saved automatically

---

## 📦 Installation

### Requirements

| Package | Arch Linux | Ubuntu/Debian |
|---------|------------|---------------|
| GTK3 | `sudo pacman -S gtk3` | `sudo apt install libgtk-3-dev` |
| VTE | `sudo pacman -S vte3` | `sudo apt install libvte-2.91-dev` |
| Tmux | `sudo pacman -S tmux` | `sudo apt install tmux` |
| GCC | `sudo pacman -S gcc` | `sudo apt install build-essential` |

### Quick Install (Recommended)

```bash
# Clone or download the repository
cd /path/to/your/classic-terminal

# Make installer executable
chmod +x install.sh

# Run installer
./install.sh
```

### Manual Installation
```bash
# Compile
gcc -o classic_terminal classic.c `pkg-config --cflags --libs gtk+-3.0 vte-2.91` -D_GNU_SOURCE

# Copy to system
sudo cp classic_terminal /usr/local/bin/classic

# Add alias
echo 'alias classic="/usr/local/bin/classic"' >> ~/.bashrc
source ~/.bashrc
