BUILD_DIR=cmake-build-docker


cmake -S . -B ${BUILD_DIR}

cd ${BUILD_DIR}

cmake --build .


