FROM ros:humble-ros-base-jammy
ENV DEBIAN_FRONTEND=noninteractive

# 1. System packages, build tools, and vision libraries
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git curl nano python3-pip \
    python3-colcon-common-extensions v4l-utils \
    libopencv-dev protobuf-compiler \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# 2. Python robotics and cloud dependencies
RUN pip3 install --no-cache-dir \
    pyserial cobs protobuf firebase-admin numpy opencv-python-headless

# 3. Project workspace and YOLO-CPP Vision Core integration
WORKDIR /root/robot_ws
RUN git clone https://github.com/Geekgineer/YOLOs-CPP.git src/YOLOs-CPP \
    && mkdir -p src/pet_brain/protos

# 4. ROS 2 environment sourcing
RUN echo "source /opt/ros/humble/setup.bash" >> /root/.bashrc

# 5. Communication ports
EXPOSE 5000/udp 5001/udp

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["bash"]