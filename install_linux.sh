#!/bin/sh
# Copy icon
cp -f ./assets/icon.ico /tmp/squire_desktop.ico
xdg-desktop-icon install --novendor /tmp/squire_desktop.ico

# Copy desktop file
xdg-desktop-icon install --novendor ./squiredesktop.desktop

