#!/usr/bin/env bash
set -eEuo pipefail

# Calculate paths relative to this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../../.." && pwd)"

echo ">>> Launching local Gentoo QA test via Podman..."
echo ">>> Repository Root: ${REPO_ROOT}"

# Run the container interactively
# We mount the repository root to /workspace inside the container
podman run --rm -it \
    --name gentoo-qa-local \
    -v "${REPO_ROOT}:/workspace:Z" \
    -w /workspace \
    docker.io/gentoo/stage3:latest \
    /workspace/tools/build_files/guru/run-gentoo-qa.sh
