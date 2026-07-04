EAPI=8

PYTHON_COMPAT=( python3_11 python3_12 python3_13 python3_14 )

inherit cmake python-single-r1

DESCRIPTION="Robot Raconteur communication framework"
HOMEPAGE="https://robotraconteur.com"

LICENSE="Apache-2.0"
SLOT="0"
KEYWORDS=""
IUSE="test"
RESTRICT="!test? ( test )"

REQUIRED_USE="${PYTHON_REQUIRED_USE}"

DEPEND="${PYTHON_DEPS}
	dev-libs/boost:=
	dev-libs/openssl:=
	sys-apps/dbus
	dev-libs/libusb:1
	net-wireless/bluez
	dev-cpp/gtest
	$(python_gen_cond_dep '
		dev-python/numpy[${PYTHON_USEDEP}]
		dev-python/pytest[${PYTHON_USEDEP}]
	')"

RDEPEND="${DEPEND}"
BDEPEND="dev-lang/swig"

S="/workspace"

pkg_setup() {
    python-single-r1_pkg_setup
}

src_unpack() {
    # Fallback to the current directory if CI_WORKSPACE_PATH isn't explicitly provided
    local src_path="${CI_WORKSPACE_PATH:-${PWD}}"

    if [[ ! -d ${src_path} ]]; then
        die "CRITICAL CI FAILURE: Local source path '${src_path}' does not exist!"
    fi

    einfo "Copying local CI repository from ${src_path} into Portage sandbox..."

    mkdir -p "${WORKDIR}/${P}" || die
    cp -R "${src_path}/." "${WORKDIR}/${P}/" || die

    S="${WORKDIR}/${P}"
}

src_configure() {
	local mycmakeargs=(
		-DBUILD_TESTING=$(usex test)
		-DBUILD_GEN=ON
		-DBUILD_PYTHON3=ON
		-DPYTHON3_EXECUTABLE="${PYTHON}"
	)
	cmake_src_configure
}

src_test() {

	cd "${BUILD_DIR}" || die "Failed to enter build directory"

	ctest -C RelWithDebInfo \
        -E "robotraconteur_test_discovery_loopback|RobotRaconteurService.DiscoveryLoopback" \
        --output-on-failure || die "Tests failure"
}
