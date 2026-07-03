EAPI=8

PYTHON_COMPAT=( python3_11 python3_12 python3_13 python3_14 )

inherit cmake git-r3 python-single-r1

DESCRIPTION="Robot Raconteur communication framework"
HOMEPAGE="https://robotraconteur.com"
EGIT_REPO_URI="https://github.com/robotraconteur/robotraconteur.git"

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
	')"
RDEPEND="${DEPEND}"
BDEPEND="dev-lang/swig"

src_configure() {
	local mycmakeargs=(
		-DBUILD_TESTING=$(usex test)
		-DBUILD_GEN=ON
		-DBUILD_PYTHON3=ON
		-DPYTHON3_EXECUTABLE="${PYTHON}"
	)
	cmake_src_configure
}
