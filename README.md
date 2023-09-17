# hottubcontroller.yaml
An ESPHome yaml script to control a hottub



# LSP support

Get ccls to work. LSP root is in the repo root:

```bash
# cd into the build directory
cd esphome/.esphome/build/htc2

# ask platformio to generate a .ccls file
pio project init --ide vim

# cd back to root
cd -
ln -s esphome/.esphome/build/htc2/.ccls

```

