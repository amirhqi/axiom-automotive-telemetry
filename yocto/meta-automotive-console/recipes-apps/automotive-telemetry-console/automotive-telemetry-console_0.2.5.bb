SUMMARY = "Qt automotive telemetry and diagnostics portfolio application"
DESCRIPTION = "C++17/Qt 6 telemetry console with TCP, UDP and serial transports"
HOMEPAGE = "https://github.com/YOUR_USERNAME/automotive-telemetry-console"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=REPLACE_WITH_MD5_AFTER_PUBLISHING"

# Replace URL and SRCREV with the actual public repository and a pinned commit.
SRC_URI = "git://github.com/YOUR_USERNAME/automotive-telemetry-console.git;protocol=https;branch=main"
SRCREV = "REPLACE_WITH_PINNED_COMMIT"
S = "${WORKDIR}/git"

inherit qt6-cmake

DEPENDS += "qtbase qtdeclarative qtserialport"

EXTRA_OECMAKE += "-DBUILD_TESTING=OFF -DBUILD_BENCHMARKS=OFF"
