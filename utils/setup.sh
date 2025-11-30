# This script is to set up the GO project environment for uploader and downloader modules.

#!/bin/bash

set -euo pipefail

trap 'echo "[ERROR] Script failed at line $LINENO"; exit 1' ERR

log() {
    echo -e "\033[1;32m[INFO]\033[0m $1"
}

run() {
    log "Running: $*"
    "$@"
}

log "Setting up the GO project environment..."

# ---------- uploader ----------
if [ -d "./uploader" ]; then
    log "Setting up uploader module..."
    cd ./uploader

    run go mod init uploader || true
    run go get golang.org/x/oauth2
    run go get golang.org/x/oauth2/google
    run go get google.golang.org/api/drive/v3
    run go get google.golang.org/api/option

    cd ..
else
    echo "[WARN] uploader directory not found, skipping."
fi

# ---------- downloader ----------
if [ -d "./downloader" ]; then
    log "Setting up downloader module..."
    cd ./downloader

    run go mod init downloader || true
    run go get golang.org/x/oauth2
    run go get golang.org/x/oauth2/google
    run go get google.golang.org/api/drive/v3
    run go get google.golang.org/api/option

    cd ..
else
    echo "[WARN] downloader directory not found, skipping."
fi

log "Setup complete successfully."
