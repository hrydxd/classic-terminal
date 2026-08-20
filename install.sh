#!/bin/bash

# Classic Terminal - Installer
# INSTALLS CLASSIC TERMINAL!

echo "╔═══════════════════════════════════════════════════════════╗"
echo "║                                                          ║"
echo "║     ██████╗ ██╗      █████╗ ███████╗███████╗██╗ ██████╗ ║"
echo "║     ██╔══██╗██║     ██╔══██╗██╔════╝██╔════╝██║██╔════╝ ║"
echo "║     ██████╔╝██║     ███████║███████╗███████╗██║██║      ║"
echo "║     ██╔══██╗██║     ██╔══██║╚════██║╚════██║██║██║      ║"
echo "║     ██████╔╝███████╗██║  ██║███████║███████║██║╚██████╗ ║"
echo "║     ╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚═╝ ╚═════╝ ║"
echo "║                                                          ║"
echo "║              CLASSIC TERMINAL INSTALLER                 ║"
echo "╚═══════════════════════════════════════════════════════════╝"
echo ""

# Check if we're in the right directory
if [ ! -f "classic.c" ]; then
    echo "✗ ERROR: classic.c not found!"
    echo "  Run install.sh in the directory with source code."
    exit 1
fi

echo "✓ Found classic.c"

# Check requirements
echo ""
echo "📦 Checking requirements..."

# GTK3
if ! pkg-config --exists gtk+-3.0; then
    echo "✗ GTK3 is not installed!"
    echo "  Install: sudo pacman -S gtk3 (Arch) or sudo apt install libgtk-3-dev (Ubuntu)"
    exit 1
fi
echo "✓ GTK3 OK"

# VTE
if ! pkg-config --exists vte-2.91; then
    echo "✗ VTE is not installed!"
    echo "  Install: sudo pacman -S vte3 (Arch) or sudo apt install libvte-2.91-dev (Ubuntu)"
    exit 1
fi
echo "✓ VTE OK"

# Tmux
if ! command -v tmux &> /dev/null; then
    echo "⚠ Tmux is not installed (optional)"
    echo "  Install: sudo pacman -S tmux (Arch) or sudo apt install tmux (Ubuntu)"
else
    echo "✓ Tmux OK"
fi

# GCC
if ! command -v gcc &> /dev/null; then
    echo "✗ GCC is not installed!"
    echo "  Install: sudo pacman -S gcc (Arch) or sudo apt install build-essential (Ubuntu)"
    exit 1
fi
echo "✓ GCC OK"

echo ""
echo "🔨 Compiling..."

# Compile
gcc -o classic_terminal classic.c `pkg-config --cflags --libs gtk+-3.0 vte-2.91` -D_GNU_SOURCE

if [ $? -ne 0 ]; then
    echo "✗ Compilation failed!"
    exit 1
fi
echo "✓ Compilation successful!"

echo ""
echo "📦 Installing..."

# Copy to /usr/local/bin
sudo cp classic_terminal /usr/local/bin/classic

if [ $? -ne 0 ]; then
    echo "✗ Copy failed!"
    echo "  Try: sudo cp classic_terminal /usr/local/bin/classic"
    exit 1
fi
echo "✓ File copied to /usr/local/bin/classic"

# Add alias to .bashrc if not exists
if ! grep -q "alias classic=" ~/.bashrc; then
    echo 'alias classic="/usr/local/bin/classic"' >> ~/.bashrc
    echo "✓ Alias added to ~/.bashrc"
else
    echo "✓ Alias already exists in ~/.bashrc"
fi

# Add classicconf to .bashrc if not exists
if ! grep -q "classicconf()" ~/.bashrc; then
    cat >> ~/.bashrc << 'EOF'

# Classic Terminal Configuration
classicconf() {
    case $1 in
        theme)
            if [ -z "$2" ]; then
                echo "Available themes: classic dark light matrix amber white"
                echo "Usage: classicconf theme <name>"
            else
                echo "$2" > ~/.classic_theme
                echo "✓ Theme changed to: $2"
                echo "Restart classic terminal to apply"
            fi
            ;;
        font)
            if [ -z "$2" ]; then
                echo "Available fonts:"
                echo "  Monospace 12, 14, 16"
                echo "  JetBrains Mono 12, 14, 16"
                echo "  Fira Code 12, 14, 16"
                echo "  Cascadia Code 12, 14, 16"
                echo "  MesloLGS NF 12, 14, 16"
                echo "  Hack 12, 14, 16"
                echo "  DejaVu Sans Mono 12, 14, 16"
                echo "  Ubuntu Mono 12, 14, 16"
                echo "Usage: classicconf font \"<name> <size>\""
            else
                echo "$2" > ~/.classic_font
                echo "✓ Font changed to: $2"
                echo "Restart classic terminal to apply"
            fi
            ;;
        *)
            echo "Classic Terminal Configuration"
            echo "Usage: classicconf {theme|font} [value]"
            echo "  classicconf theme        - Show themes"
            echo "  classicconf theme <name> - Change theme"
            echo "  classicconf font         - Show fonts"
            echo "  classicconf font <name>  - Change font"
            ;;
    esac
}
EOF
    echo "✓ classicconf added to ~/.bashrc"
else
    echo "✓ classicconf already exists in ~/.bashrc"
fi

echo ""
echo "✅ INSTALLATION COMPLETE!"
echo ""
echo "═══════════════════════════════════════════════════════════"
echo ""
echo "  🚀 Launch terminal:"
echo "     classic"
echo ""
echo "  🎨 Change theme:"
echo "     classicconf theme amber"
echo ""
echo "  🔤 Change font:"
echo "     classicconf font \"Monospace 14\""
echo ""
echo "  ⌨️  Shortcuts:"
echo "     Ctrl+Shift+C  - copy"
echo "     Ctrl+Shift+V  - paste"
echo "     Ctrl+Shift++  - zoom in"
echo "     Ctrl+Shift+-  - zoom out"
echo "     Ctrl+0        - reset size"
echo ""
echo "═══════════════════════════════════════════════════════════"
echo ""

# Ask if want to reload .bashrc
read -p "Reload .bashrc now? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    source ~/.bashrc
    echo "✓ .bashrc reloaded!"
    echo ""
    echo "🔥 Launching Classic Terminal..."
    classic
fi