cmake_minimum_required(VERSION 2.8)
project( lidar )
SET(COMPILE_DEFINITIONS -Werror)

#find_package( OpenCV REQUIRED )
include_directories(/opt/vc/include)
include_directories(/opt/vc/userland-master/interface/vcos)
include_directories(/opt/vc/userland-master)
include_directories(/opt/vc/userland-master/interface/vcos/pthreads)
include_directories(/opt/vc/userland-master/interface/vmcs_host/linux)
include_directories(/opt/vc/userland-master/host_applications/linux/libs/bcm_host/include)
link_directories(/opt/vc/lib)
add_executable(lidar lidar.cpp camera.cpp cameracontrol.cpp graphics.cpp)
add_custom_command(TARGET lidar
                   POST_BUILD
                   COMMAND sudo chown root lidar
                   COMMAND sudo chmod +s lidar)

target_link_libraries(lidar libmmal_core.so libmmal_util.so libmmal_vc_client.so libvcos.so libbcm_host.so GLESv2 EGL bcm2835 ncurses pthread)
