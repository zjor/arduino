package com.github.zjor.arduino.robotics;

import processing.serial.Serial;

import java.util.Optional;

public interface PortHolder {

    Optional<Serial> getPort();

}
