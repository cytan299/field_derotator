# Eagle files

These are the eagle files that can be sent to pc board manufacturers to
be made.

## Directories

* **driver_module** The driver module is mounted on the derotator that
  takes signals from the controller to step the stepper motor. It also
  sends back an interrupt signal when the derotator is at its *HOME*
  position.
* **field_rotator** This is the module that is installed in the
  Arduino controller that acts as the interface between the MEGA2560
  and the LX200 and the driver_module.

## Copyright

The schematic and board designs are copyright 2015 C.Y. Tan and
released under the CERN Open Hardware License v1.2



