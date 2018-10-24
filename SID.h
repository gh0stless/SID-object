#pragma once

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned char boolean;

#define NUMSIDREGS 0x18 // numbers of (writable) SID-registers
#define SIDWRITEDELAY 14 // lda $xxxx,x 4 cycles, sta $d400,x 5 cycles, dex 2 cycles, bpl 3 cycles
#define HARDSID_FLUSH_CYCLES 1000
#define HARDSID_FLUSH_MS 50
#define PAL_FRAMERATE 50
#define PAL_CLOCKRATE 985248
#define NTSC_FRAMERATE 60
#define NTSC_CLOCKRATE 1022727 //This is for machines with 6567R8 VIC. 6567R56A is slightly different.
#define FRAME_IN_CYCLES 19705 //( 17734475 / 18 / 50 )   // 50Hz in cycles for PAL clock
#define MY_BUFFER_SIZE 10000

typedef Uint16(CALLBACK* lpHardSID_Version)(void);
typedef Uint8(CALLBACK* lpHardSID_Devices)(void);
typedef void (CALLBACK* lpWriteToHardSID)(Uint8 DeviceID, Uint8 SID_reg, Uint8 Data);
typedef Uint8(CALLBACK* lpReadFromHardSID)(Uint8 DeviceID, Uint8 SID_reg);
typedef void (CALLBACK* lpInitHardSID_Mapper)(void);
typedef void (CALLBACK* lpMuteHardSID_Line)(int Mute);
typedef void (CALLBACK* lpHardSID_Delay)(Uint8 DeviceID, Uint16 Cycles);
typedef void (CALLBACK* lpHardSID_Write)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg, Uint8 Data);
typedef Uint8(CALLBACK* lpHardSID_Read)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg);
typedef void (CALLBACK* lpHardSID_Flush)(Uint8 DeviceID);
typedef void (CALLBACK* lpHardSID_SoftFlush)(Uint8 DeviceID);
typedef boolean(CALLBACK* lpHardSID_Lock)(Uint8 DeviceID);
typedef Uint8(CALLBACK* lpHardSID_Try_Write)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg, Uint8 Data);
typedef void (CALLBACK* lpHardSID_Reset)(Uint8 DeviceID);

lpHardSID_Version HardSID_Version = NULL;
lpHardSID_Devices HardSID_Devices = NULL;
lpWriteToHardSID WriteToHardSID = NULL;
lpReadFromHardSID ReadFromHardSID = NULL;
lpInitHardSID_Mapper InitHardSID_Mapper = NULL;
lpMuteHardSID_Line MuteHardSID_Line = NULL;
lpHardSID_Delay HardSID_Delay = NULL;
lpHardSID_Write HardSID_Write = NULL;
lpHardSID_Read HardSID_Read = NULL;
lpHardSID_Flush HardSID_Flush = NULL;
lpHardSID_SoftFlush HardSID_SoftFlush = NULL;
lpHardSID_Lock HardSID_Lock = NULL;
lpHardSID_Try_Write HardSID_Try_Write = NULL;
lpHardSID_Reset HardSID_Reset = NULL;


typedef enum {
	HSID_USB_WSTATE_OK = 1, HSID_USB_WSTATE_BUSY,
	HSID_USB_WSTATE_ERROR, HSID_USB_WSTATE_END
} HSID_STATE;

typedef struct circular_buffer
{
	void *buffer;     // data buffer
	void *buffer_end; // end of data buffer
	size_t capacity;  // maximum number of items in the buffer
	size_t count;     // number of items in the buffer
	size_t sz;        // size of each item in the buffer
	void *head;       // pointer to head
	void *tail;       // pointer to tail
} circular_buffer;

typedef struct write_event
{
	boolean WE_Command_Type; //false=Read True=Write
	Uint8 WE_Reg_NR; //Register Number
	Uint8 WE_Value; //Wert
} write_event;

typedef struct _sid {		// defines our object's internal variables for each instance in a patch
	t_object x_ob;			// object header - ALL objects MUST begin with this...
	t_systhread			x_systhread;		// thread reference
	t_systhread_mutex	x_mutex;			// mutual exclusion lock for threadsafety
	int					x_systhread_cancel; // thread cancel flag
	void *x_qelem;
	void *x_outlet1;		// outlet creation - inlets are automatic, but objects must "own" their own outlets
	void *x_outlet2;
	void *x_outlet3;
	void *x_outlet4;
	void *x_outlet5;
	struct SID_Voice {
		Uint8 FREQ_LO;
		Uint8 FREQ_HI;
		Uint16 PULSE;
		Uint8 WAVEFORM;
		Uint8 CONTROL;
		Uint8 GATE;
		Uint8 ATTACK;
		Uint8 DECAY;
		Uint8 SUSTAIN;
		Uint8 RELEASE;
		Uint8 VOLUME;
		Uint8 FILTER;
		Uint8 FIL_FREQ_LOW;
		Uint8 FIL_FREQ_HIGH;
		Uint8 RES;
		Uint8 FILT;
	} SID_Voice1, SID_Voice2, SID_Voice3;
	int My_Device;
	write_event we[MY_BUFFER_SIZE];
	circular_buffer my_cb;
} t_sid;

void sid_play(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_stop(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_ADSR(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_volume(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_waveform(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_pulse(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_assist(t_sid *x, void *b, long m, long a, char *s);
void *sid_new(long n);
void sid_free(t_sid *x);
void sid_anything(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_freq1(t_sid *x, long n);
void sid_freq2(t_sid *x, long n);
void sid_freq3(t_sid *x, long n);
void sid_filter_freq(t_sid *x, long n);
void sid_filter_res(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_filter_filt(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_filter_mode(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void push_event(t_sid *x, Uint8 reg, Uint8 val);
int get_event(t_sid *x, Uint8 reg);
void sid_init(t_sid *x);
void sid_readraw(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_writeraw(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_ringmod(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_sync(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_test(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_thread_stop(t_sid *x);
void *sid_threadproc(t_sid *x);

//prototypes

int cb_init(t_sid *x, circular_buffer *cb, size_t capacity, size_t sz);
int cb_empty(t_sid *x, circular_buffer *cb);
void cb_free(t_sid *x, circular_buffer *cb);
int cb_push_back(t_sid *x, circular_buffer *cb, const void *item);
int cb_pop_front(t_sid *x, circular_buffer *cb, void *item);

t_class *sid_class;		// global pointer to the object class - so max can reference the object

