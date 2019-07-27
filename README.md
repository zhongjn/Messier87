# Messier87

A realtime raytracing blackhole renderer. GLSL is used to accelerate computation.

<b>NOTE</b>: Although physical-based raytracing is used, the rendering result is still close to artists' impression (Intersteller?), not that physically realistic.
For more physically realistic result, check the "Physical background 1" in references below.


# Control

- Move camera:  W/A/S/D/Z/X

- Rotate camera: I/J/K/L/U/O

- Precise camera control: hold space

- MSAA ratio: N/M

- Try different parameters in shader/blackhole_adisc.glslf


# Some Rendering Result

## Default scene

<img src="./img/default1.png" width="500"/>

<img src="./img/default2.png" width="500"/>


## A closer look

<img src="./img/closer1.png" width="500"/>

<img src="./img/closer2.png" width="500"/>

<img src="./img/closer3.png" width="500"/>


## Near the photon sphere

<img src="./img/near_photon_sphere.png" width="500"/>


## The Einstein Ring

<img src="./img/ring.png" width="500"/>


# References

- Physical background 1: https://jila.colorado.edu/~ajsh/insidebh/

- Physical background 2: https://rantonels.github.io/starless/

- The bloom effect: https://www.shadertoy.com/view/lstSRS