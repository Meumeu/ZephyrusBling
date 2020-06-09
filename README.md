# ZephyrusBling

ZephyrusBling is a utility that can display text and images on the back of the
ASUS ROG Zephyrus G14 laptop using [rog-core](https://github.com/flukejones/rog-core).

![](example.gif)

## Features

- [X] Displaying images
- [X] Text rendering (with ImageMagick)
- [X] Animations with linear transforms (rotations, traslations and scaling only) and adjusting image brightness
- [ ] Aligning the text with grid for sharp letters
- [ ] Displaying desktop notifications
- [ ] Displaying battery status

## Compiling

The following libraries are used:
- Boost
- CLI11
- fmt
- glm
- GSL
- sdbus-c++

[rog-core](https://github.com/flukejones/rog-core) should also be running to actually
send the images to the LEDs, I have patched it to get smoother animations but the basic
functionalities work with the animatrix branch.
