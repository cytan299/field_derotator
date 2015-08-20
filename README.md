# A Field DeRotator for Alt-Az Mounted Telescopes Used for Astrophotography

by C.Y. Tan 2015

Most telescopes used for astrophotography are equatorially
mounted. However, the more popular computer controlled telescopes are
alt-az mounted and are usually deemed unsuitable for
astrophotography. The reason is that alt-az mounted telescopes have
one more degree of freedom that is not corrected because it does not
affect visual observations of celestial objects. Unfortunately, by
leaving it uncorrected, alt-az telescopes cannot be used for long
exposure astrophotography. In order to overcome this flaw in alt-az
mounted telescopes, I have designed and built a field derotator that
removes this degree of freedom and thus allows alt-az telescopes to
become astrophotography platforms.

## Goal

I have built a fully functional field derotator that consists of both
a mechanical derotator and an Arduino based controller for the
LX200. All hardware, drawings, schematics, software and documentation
are open source with the respective licenses described below.

I hope that this device will be useful for astrophotographers who find
polar mounts a real pain in the rear to use, and prefer alt-az mounts
instead.

## IMPORTANT!

Field derotation **cannot** work without an off-axis guider. If you
don't plan on investing in an off-axis guider, don't bother building
this device because field derotation will not work without it.  See
[field_derotator_formula.pdf](field_derotator_formula/field_derotator_formula.pdf)
for the explanation.

## Support

All the software have been successfully compiled on Mac OS X 10.10.5
(Yosemite).

I don't do Windows or Linux so, I cannot answer questions for these
two platforms.

This is unsupported software and hardware. Build at your own peril! :)

You can submit questions or bug reports using the
[issues](https://github.com/cytan299/field_derotator/issues) tab on
the right and then click on **NEW**.

Have fun!

## Directory structure

* **arduino** The C++ code for the arduino MEGA2560.
* **arduino_case** The case design for the field derotator controller.
* **bom** Bill of materials for 1 field derotator. I also show the
  cost for building one.
* **eagle** Eagle schematic and board files.
* **emachineshop** eMachineshop files for the derotator aluminium
shell.
* **field_derotator_formula** derivation of the derotation formula,
  analysis of how off-axis guiding is improved with derotation and
  limitation of derotation.
* **frontend** C++ code for the GUI frontend for the user to control
the derotator.
* **howto** Guides for how to assemble the derotator and the
controller.

## Other information

More information can be found in he *README.md* files in each
directory and the [wiki](https://github.com/cytan299/field_derotator/wiki/A-Field-DeRotator-for-Alt-Az-Telescopes-Used-for-Astrophotography) of this project on github.

## Copyright
All the software, documentation, hardware that I have written is
copyright 2015 C.Y. Tan.

All software is released under GPLv3

All documentation is released under Creative Commons
Attribution-ShareAlike 3.0 Unported License or GNU Free
Documentation License, Version 1.3

All hardware is released under CERN Hardware Open License v1.2



