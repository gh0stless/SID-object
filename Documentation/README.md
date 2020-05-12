# sid
### controls a SIDBlaster-USB

#### inlets
- inlet1: messages
- inlets2-4: oscilator frequencys of voices 0-2 *0-3848(hz)*
- inlet5: filter frequency *0-2047*

#### outlets
- outlet1: Bang bangs if successful init and reset
- outlets2-4: (int) outputs read results of the 4 read registers of the SID (25-28)

### Messages

- ###### play *voice(0-2)*
  * trigger the ADSR
- ###### stop *voice(0-2)*
  * stops the ADSR
- ###### ADSR *voice(0-2) A(0-15) D(0-15) S(0-15) R(0-15)*
  * sets the ADSR parameters
- ###### volume *volume(0-15)*
  * sets main volume
- ###### resonance *resonance(0-15)*
  * sets resonance
- ###### filt *filter(0-15)*
  * sets filter as bits in a nibble
- ###### fmode *filtermode(0-15)*
  * sets filtermode as bits in a nibble
- ###### waveform *voice(0-2) waveform(0-15)*
  * sets waveform as bist in a nibble
- ###### pulse *pulswidth (0-4094)*
  * sets pulswith
- ###### ringmod *voice(0-2) off/on(0-1)*
  * switch ringmod on/off
- ###### sync *voice(0-2) off/on(0-1)*
  * switch sync on/off
- ###### test *voice(0-2) off/on(0-1)*
  * sets/clear test bit
- ###### init
  * resets and inits the SIDBlaster and the SID
- ###### read *reg (25-28)*
  * read from a register (experimental)
----------------------------------------------------
#####Raw Mode
- ###### readraw *reg(25-28)*
  * like read, just in raw mode
- ###### writeraw *reg(0-24) value(0-255)*
  * write a byte direct in a register

Raw mode can only leaved by calling "init".