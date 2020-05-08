# sid-object
![](./Documentation/The-Sid-Object.png)
#### a Max/MSP C-external for SIDBlaster-USB hardware


This object allows full access to a real SID-Chip.

Installation:

*If you use Ableton Live 9 32Bit Version:
-Copy SID.mxe to C:\Program Files (x86)\Cycling '74\Max 7\resources\extensions\max
-Copy 32Bit Version of the hardsid.dll to C:\Program Files (x86)\Cycling '74\Max 7

*If you use Ableton Live 9 64Bit Version:
-Copy SID.mxe64 to C:\Program Files\Cycling '74\Max 7\resources\extensions\max
-Copy 64Bit Version of the hardsid.dll to C:\Program Files\Cycling '74\Max 7

*If you use Ableton Live 10
-Copy SID.mxe64 to C:\Program Files\Ableton\Live 10 Trial\Resources\Max\resources\extensions\max
-Copy 64Bit Version of the hardsid.dll toC:\Program Files\Ableton\Live 10 Trial\Resources\Max

(Paths depend on your installations of these programs)

*or*

copy to ...\documents\Max 8\Library

Copy the appropriate DLL into the program directory of Max

Study the attached max patch for the feature learning.

versions history
----------------
v.0.8.1 2020-05-07
- Improvement of error messages: "Real" errors are reported to the MAX console. The messages follow a     system so that they can be parsed in MAX.

v.0.8
- better threading and multitasking
better multidevice, up to 8 devices tested

Thanks to:

*Stein Pedersen and Wilfred Bos for their support.

Andreas Schumm (gh0stless)
contact: info@crazy-mide.de