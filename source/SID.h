#pragma once

typedef unsigned char	Uint8;
typedef unsigned short	Uint16;
//typedef unsigned char	boolean;

int Number_Of_Instances = 0;
int Number_Of_Devices = 0;
int DLL_Version = 0;
bool dll_load = false;
bool dll_initialized = false;
bool InUse[8] = { false, false, false, false, false, false, false, false };

#if defined(_WIN32) || defined(_WIN64)
HINSTANCE hardsiddll = 0;
#endif

#define NUMSIDREGS 0x19 // numbers of (writable) SID-registers
#define SIDWRITEDELAY 14 // lda $xxxx,x 4 cycles, sta $d400,x 5 cycles, dex 2 cycles, bpl 3 cycles
#define HARDSID_FLUSH_CYCLES 1000
#define HARDSID_FLUSH_MS 50
#define PAL_FRAMERATE 50
#define PAL_CLOCKRATE 985248
#define NTSC_FRAMERATE 60
#define NTSC_CLOCKRATE 1022727 //This is for machines with 6567R8 VIC. 6567R56A is slightly different.
#define FRAME_IN_CYCLES (17734475 / 18 / 50 )	// 50Hz in cycles for PAL clock
#define MY_BUFFER_SIZE 4096

#if defined(_WIN32) || defined(_WIN64)
typedef Uint16	(CALLBACK* lpHardSID_Version)(void);
typedef Uint8	(CALLBACK* lpHardSID_Devices)(void);
typedef void    (CALLBACK* lpHardSID_Delay)(Uint8 DeviceID, Uint16 Cycles);
typedef void    (CALLBACK* lpHardSID_Write)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg, Uint8 Data);
typedef Uint8	(CALLBACK* lpHardSID_Read)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg);
typedef void    (CALLBACK* lpHardSID_Flush)(Uint8 DeviceID);
typedef void    (CALLBACK* lpHardSID_SoftFlush)(Uint8 DeviceID);
typedef boolean	(CALLBACK* lpHardSID_Lock)(Uint8 DeviceID);
typedef void    (CALLBACK* lpHardSID_Filter)(Uint8 DeviceID, boolean Filter);
typedef void    (CALLBACK* lpHardSID_Reset)(Uint8 DeviceID);
typedef void    (CALLBACK* lpHardSID_Sync)(Uint8 DeviceID);
typedef void    (CALLBACK* lpHardSID_Mute)(Uint8 DeviceID, Uint8 Channel, boolean Mute);
typedef void    (CALLBACK* lpHardSID_MuteAll)(Uint8 DeviceID, boolean Mute);
typedef void    (CALLBACK* lpInitHardSID_Mapper)(void);
typedef Uint8	(CALLBACK* lpGetHardSIDCount)(void);
typedef void    (CALLBACK* lpWriteToHardSID)(Uint8 DeviceID, Uint8 SID_reg, Uint8 Data);
typedef Uint8	(CALLBACK* lpReadFromHardSID)(Uint8 DeviceID, Uint8 SID_reg);
typedef void    (CALLBACK* lpMuteHardSID_Line)(int Mute);
typedef void    (CALLBACK* lpHardSID_Reset2)(Uint8 DeviceID, Uint8 Volume);
typedef void    (CALLBACK* lpHardSID_Unlock)(Uint8 DeviceID);
typedef Uint8	(CALLBACK* lpHardSID_Try_Write)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg, Uint8 Data);
typedef BOOL	(CALLBACK* lpHardSID_ExternalTiming)(Uint8 DeviceID);
typedef void	(CALLBACK* lpHardSID_SetWriteBufferSize)(Uint8 bufferSize);
typedef void	(CALLBACK* lpHardSID_GetSerial)(char* output, int buffersize, Uint8 DeviceID);
typedef int		(CALLBACK* lpHardSID_SetSIDType)(Uint8 DeviceID, int sidtype);
typedef int		(CALLBACK* lpHardSID_GetSIDType)(Uint8 DeviceID);

lpHardSID_Version HardSID_Version = NULL;
lpHardSID_Devices HardSID_Devices = NULL;
lpHardSID_Delay HardSID_Delay = NULL;
lpHardSID_Write HardSID_Write = NULL;
lpHardSID_Read HardSID_Read = NULL;
lpHardSID_Flush HardSID_Flush = NULL;
lpHardSID_SoftFlush HardSID_SoftFlush = NULL;
lpHardSID_Lock HardSID_Lock = NULL;
lpHardSID_Filter HardSID_Filter = NULL;
lpHardSID_Reset HardSID_Reset = NULL;
lpHardSID_Sync HardSID_Sync = NULL;
lpHardSID_Mute HardSID_Mute = NULL;
lpHardSID_MuteAll HardSID_MuteAll = NULL;
lpInitHardSID_Mapper InitHardSID_Mapper = NULL;
lpGetHardSIDCount GetHardSIDCount = NULL;
lpWriteToHardSID WriteToHardSID = NULL;
lpReadFromHardSID ReadFromHardSID = NULL;
lpMuteHardSID_Line MuteHardSID_Line = NULL;
lpHardSID_Reset2 HardSID_Reset2 = NULL;
lpHardSID_Unlock HardSID_Unlock = NULL;
lpHardSID_Try_Write HardSID_Try_Write = NULL;
lpHardSID_ExternalTiming HardSID_ExternalTiming = NULL;
lpHardSID_SetWriteBufferSize HardSID_SetWriteBufferSize = NULL;
lpHardSID_GetSerial HardSID_GetSerial = NULL;
lpHardSID_SetSIDType HardSID_SetSIDType = NULL;
lpHardSID_GetSIDType HardSID_GetSIDType = NULL;
#endif

#if defined(__APPLE__)
typedef Uint16  (*lpHardSID_Version)(void);
typedef Uint8   (*lpHardSID_Devices)(void);
typedef void    (*lpHardSID_Delay)(Uint8 DeviceID, Uint16 Cycles);
typedef void    (*lpHardSID_Write)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg, Uint8 Data);
typedef Uint8   (*lpHardSID_Read)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg);
typedef void    (*lpHardSID_Flush)(Uint8 DeviceID);
typedef void    (*lpHardSID_SoftFlush)(Uint8 DeviceID);
typedef bool    (*lpHardSID_Lock)(Uint8 DeviceID);
typedef void    (*lpHardSID_Filter)(Uint8 DeviceID, bool Filter);
typedef void    (*lpHardSID_Reset)(Uint8 DeviceID);
typedef void    (*lpHardSID_Sync)(Uint8 DeviceID);
typedef void    (*lpHardSID_Mute)(Uint8 DeviceID, Uint8 Channel, bool Mute);
typedef void    (*lpHardSID_MuteAll)(Uint8 DeviceID, bool Mute);
typedef void    (*lpInitHardSID_Mapper)(void);
typedef Uint8   (*lpGetHardSIDCount)(void);
typedef void    (*lpWriteToHardSID)(Uint8 DeviceID, Uint8 SID_reg, Uint8 Data);
typedef Uint8   (*lpReadFromHardSID)(Uint8 DeviceID, Uint8 SID_reg);
typedef void    (*lpMuteHardSID_Line)(int Mute);
typedef void    (*lpHardSID_Reset2)(Uint8 DeviceID, Uint8 Volume);
typedef void    (*lpHardSID_Unlock)(Uint8 DeviceID);
typedef Uint8   (*lpHardSID_Try_Write)(Uint8 DeviceID, Uint16 Cycles, Uint8 SID_reg, Uint8 Data);
typedef bool    (*lpHardSID_ExternalTiming)(Uint8 DeviceID);
typedef void    (*lpHardSID_SetWriteBufferSize)(Uint8 buffersize);
typedef void    (*lpHardSID_GetSerial)(char* output, int buffersize, Uint8 DeviceID);
typedef int     (*lpHardSID_SetSIDType)(Uint8 DeviceID, int sidtype);
typedef int     (*lpHardSID_GetSIDType)(Uint8 DeviceID);
typedef void    (*lpHardSID_Uninitialize)(void);

lpHardSID_Version HardSID_Version = NULL;
lpHardSID_Devices HardSID_Devices = NULL;
lpHardSID_Delay HardSID_Delay = NULL;
lpHardSID_Write HardSID_Write = NULL;
lpHardSID_Read HardSID_Read = NULL;
lpHardSID_Flush HardSID_Flush = NULL;
lpHardSID_SoftFlush HardSID_SoftFlush = NULL;
lpHardSID_Lock HardSID_Lock = NULL;
lpHardSID_Filter HardSID_Filter = NULL;
lpHardSID_Reset HardSID_Reset = NULL;
lpHardSID_Sync HardSID_Sync = NULL;
lpHardSID_Mute HardSID_Mute = NULL;
lpHardSID_MuteAll HardSID_MuteAll = NULL;
lpInitHardSID_Mapper InitHardSID_Mapper = NULL;
lpGetHardSIDCount GetHardSIDCount = NULL;
lpWriteToHardSID WriteToHardSID = NULL;
lpReadFromHardSID ReadFromHardSID = NULL;
lpMuteHardSID_Line MuteHardSID_Line = NULL;
lpHardSID_Reset2 HardSID_Reset2 = NULL;
lpHardSID_Unlock HardSID_Unlock = NULL;
lpHardSID_Try_Write HardSID_Try_Write = NULL;
lpHardSID_ExternalTiming HardSID_ExternalTiming = NULL;
lpHardSID_SetWriteBufferSize SetWriteBufferSize = NULL;
lpHardSID_GetSerial HardSID_GetSerial = NULL;
lpHardSID_SetSIDType HardSID_SetSIDType = NULL;
lpHardSID_GetSIDType HardSID_GetSIDType = NULL;
lpHardSID_Uninitialize HardSID_Uninitialize = NULL;
#endif

typedef enum {
	HSID_USB_WSTATE_OK = 1, HSID_USB_WSTATE_BUSY,
	HSID_USB_WSTATE_ERROR, HSID_USB_WSTATE_END
} HSID_STATE;

enum SID_TYPE {
		SID_TYPE_NONE = 0, SID_TYPE_6581, SID_TYPE_8580
	};

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
	void *x_outlet6;
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
	bool raw_mode;
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
void sid_getinfo(t_sid *x);
void sid_readraw(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_writeraw(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_read(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_ringmod(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_sync(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_test(t_sid *x, t_symbol *s, long argc, t_atom *argv);
void sid_thread_stop(t_sid *x);
void *sid_threadproc(t_sid *x);

// circular buffer
int cb_init(t_sid *x, circular_buffer *cb, size_t capacity, size_t sz);
int cb_empty(t_sid *x, circular_buffer *cb);
void cb_free(t_sid *x, circular_buffer *cb);
int cb_push_back(t_sid *x, circular_buffer *cb, const void *item);
int cb_pop_front(t_sid *x, circular_buffer *cb, void *item);

t_class *sid_class;		// global pointer to the object class - so max can reference the object
