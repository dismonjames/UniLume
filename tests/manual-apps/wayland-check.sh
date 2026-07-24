#!/bin/sh
# wayland-check.sh — determine display server and Fcitx5 environment
# SPDX-License-Identifier: GPL-2.0-or-later

echo "=== Session ==="
echo "Session type:       ${XDG_SESSION_TYPE:-unknown}"
echo "Desktop:            ${XDG_CURRENT_DESKTOP:-unknown}"
echo "Wayland display:    ${WAYLAND_DISPLAY:-none}"
echo "X11 display:        ${DISPLAY:-none}"
echo "Desktop session:    ${XDG_SESSION_DESKTOP:-unknown}"

echo ""
echo "=== Kernel ==="
uname -a

echo ""
echo "=== OS ==="
cat /etc/os-release 2>/dev/null || cat /usr/lib/os-release 2>/dev/null

echo ""
echo "=== Fcitx5 ==="
fcitx5 --version 2>/dev/null || echo "fcitx5 not found"
fcitx5-diagnose 2>/dev/null | head -20 || echo "fcitx5-diagnose not available"

echo ""
echo "=== Input method ==="
echo "GTK_IM_MODULE:      ${GTK_IM_MODULE:-unset}"
echo "QT_IM_MODULE:       ${QT_IM_MODULE:-unset}"
echo "XMODIFIERS:         ${XMODIFIERS:-unset}"
echo "SDL_IM_MODULE:      ${SDL_IM_MODULE:-unset}"

echo ""
echo "=== GTK version ==="
pkg-config --modversion gtk4 2>/dev/null || \
pkg-config --modversion gtk+-3.0 2>/dev/null || \
echo "GTK not found via pkg-config"

echo ""
echo "=== Qt version ==="
pkg-config --modversion Qt6Core 2>/dev/null || \
pkg-config --modversion Qt5Core 2>/dev/null || \
echo "Qt not found via pkg-config"

echo ""
echo "=== Browsers ==="
for browser in firefox google-chrome chromium chromium-browser; do
    path=$(command -v "$browser" 2>/dev/null)
    if [ -n "$path" ]; then
        version=$("$browser" --version 2>/dev/null || echo "version unknown")
        echo "$browser: $path ($version)"
    fi
done

echo ""
echo "=== VSCode ==="
path=$(command -v code 2>/dev/null)
if [ -n "$path" ]; then
    version=$("$path" --version 2>/dev/null | head -1 || echo "version unknown")
    echo "code: $path ($version)"
fi

echo ""
echo "=== Wayland detection ==="
if [ -n "$WAYLAND_DISPLAY" ]; then
    echo "Native Wayland session detected (WAYLAND_DISPLAY=$WAYLAND_DISPLAY)"
    if [ -n "$DISPLAY" ]; then
        echo "XWayland also active (DISPLAY=$DISPLAY)"
    fi
    echo ""
    echo "Running compositor info:"
    if command -v kwin_wayland >/dev/null 2>&1; then
        kwin_wayland --version 2>/dev/null | head -2
    fi
    if command -v gnome-shell >/dev/null 2>&1; then
        gnome-shell --version 2>/dev/null
    fi
else
    echo "No Wayland display detected"
    if [ -n "$DISPLAY" ]; then
        echo "Running on X11 (DISPLAY=$DISPLAY)"
    else
        echo "No display server detected"
    fi
fi

echo ""
echo "=== fcitx5 process ==="
pgrep -a fcitx5 2>/dev/null || echo "fcitx5 not running"
