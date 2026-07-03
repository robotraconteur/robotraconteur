#!/usr/bin/env bash
set -eEuo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RESOURCE_DIR="${SCRIPT_DIR}/resources"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../../.." && pwd)"

echo ">>> Setting up keys via getuto..."
getuto

echo ">>> Syncing portage..."
emerge-webrsync --quiet

echo ">>> Deploying Portage QA configuration..."
cat "${RESOURCE_DIR}/qa.conf" >> /etc/portage/make.conf

echo ">>> Explicitly unmasking live package keywords..."
mkdir -p /etc/portage/package.accept_keywords
echo "dev-cpp/robotraconteur **" > /etc/portage/package.accept_keywords/robotraconteur

echo ">>> Setting up local overlay structure..."
mkdir -p /var/db/repos/local/profiles
echo "local" > /var/db/repos/local/profiles/repo_name

# Fix the missing masters warning by generating layout.conf
mkdir -p /var/db/repos/local/metadata
echo "masters = gentoo" > /var/db/repos/local/metadata/layout.conf

mkdir -p /etc/portage/repos.conf
cp "${RESOURCE_DIR}/local.conf" /etc/portage/repos.conf/local.conf

echo ">>> Deploying live 9999 ebuild..."
PKG_DIR="/var/db/repos/local/dev-cpp/robotraconteur"
mkdir -p "${PKG_DIR}"
cp "${RESOURCE_DIR}/robotraconteur-9999.ebuild" "${PKG_DIR}/robotraconteur-9999.ebuild"

cd "${PKG_DIR}"
ebuild robotraconteur-9999.ebuild manifest

echo ">>> Scoping test features strictly to robotraconteur..."
mkdir -p /etc/portage/package.use /etc/portage/env /etc/portage/package.env

echo "dev-cpp/robotraconteur test" > /etc/portage/package.use/robotraconteur
echo 'FEATURES="test"' > /etc/portage/env/qa-tests.conf
echo "dev-cpp/robotraconteur qa-tests.conf" > /etc/portage/package.env/robotraconteur

echo ">>> Pre-installing dependencies..."
emerge --onlydeps --getbinpkg dev-cpp/robotraconteur

echo ">>> Executing emerge QA build against Git HEAD..."
export EGIT_OVERRIDE_REPO_ROBOTRACONTEUR="${REPO_ROOT}"

FEATURES="test" emerge -v =dev-cpp/robotraconteur-9999