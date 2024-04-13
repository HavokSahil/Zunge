#!/bin/bash

INSTALL_DIR="/usr/local/bin"

if [ -f "$INSTALL_DIR/zunge" ]; then
    echo "Removing zunge executable..."
    sudo rm "$INSTALL_DIR/zunge"
fi

if [ -f "$INSTALL_DIR/pdf_to_text.py" ]; then
    echo "Removing pdf_to_text.py script..."
    sudo rm "$INSTALL_DIR/pdf_to_text.py"
fi

echo "Uninstallation complete."
