cd ~/esp-idf_tools/esp-idf
source export.sh
cd ~/connectedhomeip
git clean -Xdf
source ./scripts/bootstrap.sh
export IDF_CCACHE_ENABLE=1

