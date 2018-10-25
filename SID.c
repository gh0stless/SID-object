/*
	@file
	SID.c - a Max-Extention for SIDBlaster-USB
	(c) by Andreas Schumm (gh0stless) for www.crazy-midi.de
	Version v.0.1 started at: 06.12.2016
	        v.0.2 started at: 11.09.2017
			v.0.3 started at: 23.09.2017
			v.0.4 started at: 26.09.2017
			v.0.5 started at: 22.10.2017
			v.0.6 started at: 15.03.2018
			v.0.7 started at: 24.07.2018
			v.0.8 started at: 24.10.2018
			-Instanzverwaltung zugefügt
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.
#include "ext_systhread.h"  // the threat-stuff
#include <stddef.h>
#include "SID.h"

//global variables
int Nr_Of_Instances = 0;
int Number_Of_Devices = 0;
HINSTANCE hardsiddll = 0;
bool dll_initialized = FALSE;
bool InUse[8] = { false, false, false, false, false, false, false, false };

void ext_main(void *r)
{
	t_class *c;
	c = class_new("sid", (method)sid_new, (method)sid_free, sizeof(t_sid), 0L, A_DEFLONG, 0);
	// class_new() loads our external's class into Max's memory so it can be used in a patch
	// sid_new = object creation method defined below
	class_addmethod(c, (method)sid_filter_freq, "in4", A_LONG, 0);
	class_addmethod(c, (method)sid_freq3, "in3", A_LONG, 0);
	class_addmethod(c, (method)sid_freq2, "in2", A_LONG, 0);
	class_addmethod(c, (method)sid_freq1, "in1", A_LONG, 0);
	class_addmethod(c, (method)sid_play, "play", A_GIMME, 0);
	class_addmethod(c, (method)sid_stop, "stop", A_GIMME, 0);
	class_addmethod(c, (method)sid_ADSR, "ADSR", A_GIMME, 0);
	class_addmethod(c, (method)sid_volume, "volume", A_GIMME, 0);
	class_addmethod(c, (method)sid_filter_res, "resonance", A_GIMME, 0);
	class_addmethod(c, (method)sid_filter_filt, "filt", A_GIMME, 0);
	class_addmethod(c, (method)sid_filter_mode, "fmode", A_GIMME, 0);
	class_addmethod(c, (method)sid_waveform, "waveform", A_GIMME, 0);
	class_addmethod(c, (method)sid_pulse, "pulse", A_GIMME, 0);
	class_addmethod(c, (method)sid_init, "init", A_GIMME, 0);
	class_addmethod(c, (method)sid_readraw, "readraw", A_GIMME, 0);
	class_addmethod(c, (method)sid_writeraw, "writeraw", A_GIMME, 0);
	class_addmethod(c, (method)sid_ringmod, "ringmod", A_GIMME, 0);
	class_addmethod(c, (method)sid_sync, "sync", A_GIMME, 0);
	class_addmethod(c, (method)sid_test, "test", A_GIMME, 0);
	class_addmethod(c, (method)sid_anything, "anything", A_GIMME, 0);
	class_addmethod(c, (method)sid_assist,	"assist",	A_CANT, 0);	// (optional) assistance method needs to be declared like this
	class_register(CLASS_BOX, c);
	sid_class = c;

	//--------------------------------------------------------

	post("sid object v.0.8 loaded...", 0);	// post any important info to the max window when our class is loaded

	hardsiddll = LoadLibrary("hardsid.dll");
	// Check to see if the library was loaded successfully 
	if (hardsiddll != 0) {
		post("hardsid.dll library loaded!\n");
		HardSID_Version = (lpHardSID_Version)GetProcAddress(hardsiddll, "HardSID_Version");
		HardSID_Devices = (lpHardSID_Devices)GetProcAddress(hardsiddll, "HardSID_Devices");
		WriteToHardSID = (lpWriteToHardSID)GetProcAddress(hardsiddll, "WriteToHardSID");
		ReadFromHardSID = (lpReadFromHardSID)GetProcAddress(hardsiddll, "ReadFromHardSID");
		InitHardSID_Mapper = (lpInitHardSID_Mapper)GetProcAddress(hardsiddll, "InitHardSID_Mapper");
		MuteHardSID_Line = (lpMuteHardSID_Line)GetProcAddress(hardsiddll, "MuteHardSID_Line");
		HardSID_Delay = (lpHardSID_Delay)GetProcAddress(hardsiddll, "HardSID_Delay");
		HardSID_Write = (lpHardSID_Write)GetProcAddress(hardsiddll, "HardSID_Write");
		HardSID_Flush = (lpHardSID_Flush)GetProcAddress(hardsiddll, "HardSID_Flush");
		HardSID_SoftFlush = (lpHardSID_SoftFlush)GetProcAddress(hardsiddll, "HardSID_SoftFlush");
		HardSID_Lock = (lpHardSID_Lock)GetProcAddress(hardsiddll, "HardSID_Lock");
		HardSID_Try_Write = (lpHardSID_Try_Write)GetProcAddress(hardsiddll, "HardSID_Try_Write");
		HardSID_Reset = (lpHardSID_Reset)GetProcAddress(hardsiddll, "HardSID_Reset");
		
		//check version & device count
		int DLL_Version = (int)HardSID_Version();
		Number_Of_Devices = (int)HardSID_Devices();
		post("dll-version: %ld", (long)DLL_Version);
		if ((DLL_Version < 512) || Number_Of_Devices>8) {
			dll_initialized = FALSE;
			post("wrong hardsid.dll Version");
		}
		else {
			post("number of devices: %ld", (long)Number_Of_Devices);
			dll_initialized = TRUE;
		}
	}
	else {
		dll_initialized = FALSE;
		post("hardsid.dll library failed to load!\n");
	}

	
}

void *sid_new(long n)		// n = int argument typed into object box (A_DEFLONG) -- defaults to 0 if no args are typed
{
	t_sid *x;				// local variable (pointer to a t_sid data structure)
	x = (t_sid *)object_alloc(sid_class); // create a new instance of this object
	intin(x, 4);	//filter-frquency inlet
	intin(x, 3);	//freq. 3 inlet
	intin(x, 2);    //freq. 2 inlet
	intin(x, 1);    //freq. 1 inlet
	x->x_outlet1 = bangout(x);	// create an bang outlet
	x->x_outlet2 = intout(x);
	x->x_outlet3 = intout(x);
	x->x_outlet4 = intout(x);
	x->x_outlet5 = intout(x);

	x->x_systhread = NULL;
	systhread_mutex_new(&x->x_mutex, 0);

	//x->x_qelem = qelem_new((t_object *)x, (method)sid_qtask);

	
	post("a new sid object instance added to patch...",0); // post important info to the max window when new instance is created
	Nr_Of_Instances++;
	post("number of instances: %dl", Nr_Of_Instances);

	if (!dll_initialized) {
		post("error! cant load hardsid.dll");
		Nr_Of_Instances--;
		post("number of instances: %dl", Nr_Of_Instances);
		return(NULL);
	}

	if (Number_Of_Devices > 8) {
		post("error! more than 8 devices");
		Nr_Of_Instances--;
		post("number of instances: %dl", Nr_Of_Instances);
		return(NULL);
	}
	
	if (Nr_Of_Instances > Number_Of_Devices) {
		post("error! not enough SIDBlasters!");
		Nr_Of_Instances--;
		post("number of instances: %dl", Nr_Of_Instances);
		return(NULL);
	}
	
	for (int i =0; i < Number_Of_Devices; i++) 
	{
		if (!InUse[i]) {
			x->My_Device = i;
			InUse[i] = true;
			break;
		}
	}

	post("using device No.: %ld", x->My_Device);
	
	if (cb_init(x, &x->my_cb, MY_BUFFER_SIZE, sizeof(write_event))) {
		post("Fatal! can't alloc mem");
		return(NULL);

	}
	
	if (x->x_systhread == NULL) {
		post("starting a new thread");
		systhread_create((method)sid_threadproc, x, 0, 0, 0, &x->x_systhread);
	}

	sid_init(x);
	return(x);					// return a reference to the object instance
}

void sid_free(t_sid *x)
{
	// stop our thread if it is still running
	sid_thread_stop(x);
	// free out mutex
	if (x->x_mutex)
		systhread_mutex_free(x->x_mutex);
	// free ringbuffer
	cb_free(x, &x->my_cb);
	//post(" I die!!");
	Nr_Of_Instances--;
	post("number of instances: %dl", Nr_Of_Instances);
	InUse[x->My_Device] = false;
}

void *sid_threadproc(t_sid *x)
{
	write_event lwe;
	while (1) {
		if (x->x_systhread_cancel)
			break;
		if (!(cb_empty(x, &x->my_cb))) {
			systhread_mutex_lock(x->x_mutex);
			if (cb_pop_front(x, &x->my_cb, &lwe)) {
				post("Fatal!");
			}
			systhread_mutex_unlock(x->x_mutex);
			Uint8 reg = lwe.WE_Reg_NR;
			Uint8 val = lwe.WE_Value;
			Uint8 RS = 0;
			while (RS != HSID_USB_WSTATE_OK) {
				RS = HardSID_Try_Write(x->My_Device, 0, reg, val); 
				if (RS == HSID_USB_WSTATE_BUSY) systhread_sleep(20);
			}
			HardSID_SoftFlush(x->My_Device);
		}
	}
	x->x_systhread_cancel = false;							// reset cancel flag for next time, in case
															// the thread is created again
	systhread_exit(0);										// this can return a value to systhread_join();
	return NULL;
}

void sid_thread_stop(t_sid *x)
{
	unsigned int ret;

	if (x->x_systhread) {
		post("stopping our thread");
		x->x_systhread_cancel = true;						// tell the thread to stop
		systhread_join(x->x_systhread, &ret);					// wait for the thread to stop
		x->x_systhread = NULL;
	}
}

void sid_init(t_sid *x) {
	post("reset & init SID");
	//Init SID
	HardSID_Reset(x->My_Device);
	HardSID_Lock(x->My_Device);
	HardSID_Flush(x->My_Device);

	//Init Registers
	push_event(x, 0, 0x00);
	Sleep(300);
	BYTE r;
	for (r = 0; r <= NUMSIDREGS; r++) {
		push_event(x, r, 0x00);
	}

	x->SID_Voice1.FREQ_LO = 0;
	x->SID_Voice1.FREQ_HI = 0;
	x->SID_Voice1.PULSE = 0;
	x->SID_Voice1.WAVEFORM =0;
	x->SID_Voice1.CONTROL = 0;
	x->SID_Voice1.GATE =0;
	x->SID_Voice1.ATTACK = 0;
	x->SID_Voice1.DECAY = 0;
	x->SID_Voice1.SUSTAIN = 0;
	x->SID_Voice1.RELEASE = 0;

	x->SID_Voice1.VOLUME = 0;
	x->SID_Voice1.FILTER = 0;
	x->SID_Voice1.FIL_FREQ_LOW = 0;
	x->SID_Voice1.FIL_FREQ_HIGH = 0;
	x->SID_Voice1.FILT = 0;
	x->SID_Voice1.RES = 0;

	
	x->SID_Voice2.FREQ_LO = 0;
	x->SID_Voice2.FREQ_HI = 0;
	x->SID_Voice2.PULSE = 0;
	x->SID_Voice2.WAVEFORM = 0;
	x->SID_Voice2.CONTROL = 0;
	x->SID_Voice2.GATE = 0;
	x->SID_Voice2.ATTACK = 0;
	x->SID_Voice2.DECAY = 0;
	x->SID_Voice2.SUSTAIN = 0;
	x->SID_Voice2.RELEASE = 0;

	x->SID_Voice2.VOLUME = 0;
	x->SID_Voice2.FILTER = 0;
	x->SID_Voice2.FIL_FREQ_LOW = 0;
	x->SID_Voice2.FIL_FREQ_HIGH = 0;
	x->SID_Voice2.FILT = 0;
	x->SID_Voice2.RES = 0;

	x->SID_Voice3.FREQ_LO = 0;
	x->SID_Voice3.FREQ_HI = 0;
	x->SID_Voice3.PULSE = 0;
	x->SID_Voice3.WAVEFORM = 0;
	x->SID_Voice3.CONTROL = 0;
	x->SID_Voice3.GATE = 0;
	x->SID_Voice3.ATTACK = 0;
	x->SID_Voice3.DECAY = 0;
	x->SID_Voice3.SUSTAIN = 0;
	x->SID_Voice3.RELEASE = 0;

	x->SID_Voice3.VOLUME = 0;
	x->SID_Voice3.FILTER = 0;
	x->SID_Voice3.FIL_FREQ_LOW = 0;
	x->SID_Voice3.FIL_FREQ_HIGH = 0;
	x->SID_Voice3.FILT = 0;
	x->SID_Voice3.RES = 0;
}

void sid_readraw(t_sid *x, t_symbol *s, long argc, t_atom *argv) {
	//RAW-Mode
	t_atom *ap;
	ap = argv;
	if (argc == 1) {
		//OK there is 1 parameter
		if (atom_gettype(ap++) == A_LONG) {
			ap = argv;
			Uint8 n = atom_getlong(ap++);
			if ((n >= 25) && (n <= 28)) {
				//post("I have to read from %ld", n);
				//so dann lies mal
				int m = 0;
				m = get_event(x, n);
				//post("I read : %ld",m);
				
				switch (n)
				{
				case 25: { outlet_int(x->x_outlet4, m);
					break; }
				case 26: { outlet_int(x->x_outlet3, m);
					break; }
				case 27: { outlet_int(x->x_outlet2, m);
					break; }
				case 28: { outlet_int(x->x_outlet1, m);
					break; }

				default:
					break;
				}
			}
			else {
				post("sid(readraw): error! value out of range");
			}
		}
		else {
			post("sid(readraw): error!!! not an Integer");
		}
	}
	else {
		post("sid(readraw): error!!! not 1 parameter!");
	}
}

void sid_writeraw(t_sid *x, t_symbol *s, long argc, t_atom *argv) {
	//RAW-Mode
	t_atom *ap;
	ap = argv;
	if (argc == 2) {
		//OK there is 2 parameters
		post("rawmode is not implementet yet");
	}
	else {
		post("sid(writeraw): error!!! not 2 parameters!");
	}
}

void push_event(t_sid *x, Uint8 reg, Uint8 val) {
	write_event lwe;
	lwe.WE_Command_Type = true;
	lwe.WE_Reg_NR = reg;
	lwe.WE_Value = val;
	systhread_mutex_lock(x->x_mutex);
	if (cb_push_back(x, &x->my_cb, &lwe)) {
		post("Fatal!");
	}
	systhread_mutex_unlock(x->x_mutex);
	//post("I wrote %ld in %ld", (long)val, (long)reg);
}

int get_event(t_sid *x, Uint8 reg) {

	Uint8 val = 0;
	systhread_mutex_lock(x->x_mutex);
	//val = HardSID_Read(x->My_Device, 0, reg);
	val = ReadFromHardSID(x->My_Device, reg);
	systhread_mutex_unlock(x->x_mutex);
	//post("I get %ld from %ld", (long)val, (long)reg);
	return((int)val);
}

void sid_assist(t_sid *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
	if (m == ASSIST_OUTLET)
		switch (a) {
		case 0:
			sprintf(s, "bang, if it wants");
			break;
		case 1:
			sprintf(s, "result reading register 25");
			break;
		case 2:
			sprintf(s, "result reading register 26");
			break;
		case 3:
			sprintf(s, "result reading register 27");
			break;
		case 4:
			sprintf(s, "result reading register 28");
			break;
		}
	else {
		switch (a) {
		case 0:
			sprintf(s, "Set and Play");
			break;
		case 1:
			sprintf(s, "Freq. of Voice 1");
			break;
		case 2:
			sprintf(s, "Freq. of Voice 2");
			break;
		case 3:
			sprintf(s, "Freq. of Voice 3");
			break;
		case 4:
			sprintf(s, "filter freq.");
			break;
		}
	}
}

void sid_volume(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 1) {
		//OK there is 1 parameter
		if (atom_gettype(ap++) == A_LONG) {
			ap = argv;
			Uint8 n = atom_getlong(ap++);
			//n is volume 0-15
			if ((n >= 0) && (n <= 15)) {
				Uint8 m = x->SID_Voice1.FILTER; // Get Filter
				Uint8 o = n + (m << 4);
				push_event(x, 24, (Uint8)o);
				x->SID_Voice1.VOLUME = n; //store volume
			}
			else {
				post("sid(volume): error!!! value out of range");
			}
		}
	}
	else {
		post("sid(volume): error!!! not 1 parameter!");
	}
}

void sid_ADSR(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 5) {
		//OK there is 5 parameters
		if ((atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG)) {
			ap = argv;
			Uint8 n = atom_getlong(ap++);
			Uint8 A = atom_getlong(ap++);
			Uint8 D = atom_getlong(ap++);
			Uint8 S = atom_getlong(ap++);
			Uint8 R = atom_getlong(ap++);
			if ((A >= 0 && A <= 15) && (D >= 0 && D <= 15) && (S >= 0 && S <= 15) && (R >= 0 && R <= 15)) {
				switch (n) {
					case 0: 
						push_event(x, 5, (Uint8)((A << 4) + D)); //AD in 5
						push_event(x, 6, (Uint8)((S << 4) + R)); //SR in 6
						break;
					case 1: 
						push_event(x, 12, (Uint8)((A << 4) + D)); //AD in 12
						push_event(x, 13, (Uint8)((S << 4) + R)); //SR in 13
						break;
					case 2:
						push_event(x, 19, (Uint8)((A << 4) + D)); //AD in 19
						push_event(x, 20, (Uint8)((S << 4) + R)); //SR in 20
						break;
					
					default: 
						post("sid(ADSR): error!!! not yet a voice!");
						break;
				}
			}
			else {
				post("sid(ADSR): error!!! value out of range!");
			}
		}
	}
	else {
		post("sid(ADSR): error!!! not 5 Parameters!");
	}
}

void sid_waveform(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 2) {
		//OK there is 2 parameters
		if ((atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG)) {
			ap = argv;
			int n = atom_getlong(ap++);
			Uint8 w = atom_getlong(ap++);
			if (w >= 0 && w <= 15) {
				Uint8 m = 0;
				switch (n) {
				case 0:
					m = x->SID_Voice1.CONTROL;	// Get Control-Nible
					if (w >= 8) m = m | 0b00001000; //Rausch-Bug
					push_event(x, 4, (Uint8)((w << 4)+m));
					x->SID_Voice1.WAVEFORM = w;		//Store Waveform
					break;
				case 1:
					m = x->SID_Voice2.CONTROL;	// Get Control-Nible
					if (w >= 8) m = m | 0b00001000; //Rausch-Bug
					push_event(x, 11, (Uint8)((w << 4)+m));
					x->SID_Voice2.WAVEFORM = w;		//Store Waveform
					break;
				
				case 2:
					m = x->SID_Voice3.CONTROL;	// Get Control-Nible
					if (w >= 8) m = m | 0b00001000; //Rausch-Bug
					push_event(x, 18, (Uint8)((w << 4)+m));
					x->SID_Voice3.WAVEFORM = w;		//Store Waveform
					break;
				default:
					post("sid(waveform): error!!! not yet a voice!");
					break;
				}
			}
			else {
				post("sid(waveform): error!!! value out of range!");
			}
		}
		else {
			post("sid(waveform): error!!! not right Parameters!");
		}
	}
	else {
		post("sid(waveform): error!!! not 2 Parameters!");
	}
}

void sid_pulse(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 2) {
		//OK there is 2 parameters
		if ((atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG)) {
			ap = argv;
			int n = atom_getlong(ap++);
			Uint16 w = atom_getlong(ap++);
			Uint8 l = (Uint8)w;
			Uint16 a = (w & 0b0000111100000000) >> 8;
			Uint8 h = (Uint8)a;
			if (w >= 0 && w <= 4095) {
				switch (n) {
					case 0:
						push_event(x, 2, (Uint8)l);
						push_event(x, 3, (Uint8)h);
						x->SID_Voice1.PULSE = w;		//Store Pulse
						break;
					case 1:
						push_event(x, 9, (Uint8)l);
						push_event(x, 10, (Uint8)h);
						x->SID_Voice2.PULSE = w;		//Store Pulse
						break;
					case 2:
						push_event(x, 16, (Uint8)l);
						push_event(x, 17, (Uint8)h);
						x->SID_Voice3.PULSE = w;		//Store Pulse
						break;
					default:
						post("sid(pulse): error!!! not yet a voice!");
						break;					
				}
			}
			else {
				post("sid(pulse): error!!! value out of range!");
			}
		}
		else {
			post("sid(pulse): rrror!!! not right parameters!");
		}
	}
	else {
		post("sid(pulse): error!!! not 2 parameters!");
	}
}

void sid_ringmod(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 2) {
		//OK there is 2 parameters
		if ((atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG)) {
			ap = argv;
			int n = atom_getlong(ap++);
			int w = atom_getlong(ap++);
			if ((w >= 0) && (w <= 1)) {
				switch (n) {
				case 0:
					if (w == 1) {
						Uint8 r = x->SID_Voice1.CONTROL; //load backup
						Uint8 s = r | 0b00000100; //set Bit
						Uint8 t = x->SID_Voice1.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 4, (Uint8)u); //write to SID
						x->SID_Voice1.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice1.CONTROL; //load backup
						Uint8 s = r & 0b11111011; //clear bit
						Uint8 t = x->SID_Voice1.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 4, (Uint8)u); //write to SID
						x->SID_Voice1.CONTROL = s; //do backup

					}
					break;
				case 1:
					if (w == 1) {
						Uint8 r = x->SID_Voice2.CONTROL; //load backup
						Uint8 s = r | 0b00000100; //set Bit
						Uint8 t = x->SID_Voice2.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 11, (Uint8)u); //write to SID
						x->SID_Voice2.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice2.CONTROL; //load backup
						Uint8 s = r & 0b11111011; //clear bit
						Uint8 t = x->SID_Voice2.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 11, (Uint8)u); //write to SID
						x->SID_Voice2.CONTROL = s; //do backup
					}
					break;
				case 2:
					if (w == 1) {
						Uint8 r = x->SID_Voice3.CONTROL; //load backup
						Uint8 s = r | 0b00000100; //set Bit
						Uint8 t = x->SID_Voice3.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 18, (Uint8)u); //write to SID
						x->SID_Voice3.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice3.CONTROL; //load backup
						Uint8 s = r & 0b11111011; //clear bit
						Uint8 t = x->SID_Voice3.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 18, (Uint8)u); //write to SID
						x->SID_Voice3.CONTROL = s; //do backup
					}
					break;
				default:
					post("sid(ringmod): error!!! not yet a voice!");
					break;
				}
			}
			else {
				post("sid(ringmod): error!!! value out of range!");
			}
		}
		else {
			post("sid(ringmod): error!!! not right Parameters!");
		}
	}
	else {
		post("sid(ringmod): error!!! not 2 Parameters!");
	}
}

void sid_sync(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 2) {
		//OK there is 2 parameters
		if ((atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG)) {
			ap = argv;
			int n = atom_getlong(ap++);
			int w = atom_getlong(ap++);
			if ((w >= 0) && (w <= 1)) {
				switch (n) {
				case 0: 
					if (w == 1) {
						Uint8 r = x->SID_Voice1.CONTROL; //load backup
						Uint8 s = r | 0b00000010; //set Bit
						Uint8 t = x->SID_Voice1.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 4, (Uint8)u); //write to SID
						x->SID_Voice1.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice1.CONTROL; //load backup
						Uint8 s = r & 0b11111101; //clear bit
						Uint8 t = x->SID_Voice1.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 4, (Uint8)u); //write to SID
						x->SID_Voice1.CONTROL = s; //do backup
					}
					break;
				case 1:
					if (w == 1) {
						Uint8 r = x->SID_Voice2.CONTROL; //load backup
						Uint8 s = r | 0b00000010; //set Bit
						Uint8 t = x->SID_Voice2.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 11, (Uint8)u); //write to SID
						x->SID_Voice2.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice2.CONTROL; //load backup
						Uint8 s = r & 0b11111101; //clear bit
						Uint8 t = x->SID_Voice2.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 11, (Uint8)u); //write to SID
						x->SID_Voice2.CONTROL = s; //do backup
					}
					break;
				case 2:
					if (w == 1) {
						Uint8 r = x->SID_Voice3.CONTROL; //load backup
						Uint8 s = r | 0b00000010; //set Bit
						Uint8 t = x->SID_Voice3.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 18, (Uint8)u); //write to SID
						x->SID_Voice3.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice3.CONTROL; //load backup
						Uint8 s = r & 0b11111101; //clear bit
						Uint8 t = x->SID_Voice3.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 18, (Uint8)u); //write to SID
						x->SID_Voice3.CONTROL = s; //do backup
					}
					break;
				}
			}
			else {
				post("sid(sync): error!!! value out of range!");
			}
		}
		else {
			post("sid(sync): error!!! not right Parameters!");
		}
	}
	else {
		post("sid(sync): error!!! not 2 Parameters!");
	}
}

void sid_test(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 2) {
		//OK there is 2 parameters
		if ((atom_gettype(ap++) == A_LONG) && (atom_gettype(ap++) == A_LONG)) {
			ap = argv;
			int n = atom_getlong(ap++);
			int w = atom_getlong(ap++);
			if ((w >= 0) && (w <= 1)) {
				switch (n) {
				case 0:
					if (w == 1) {
						Uint8 r = x->SID_Voice1.CONTROL; //load backup
						Uint8 s = r | 0b00001000; //set Bit
						Uint8 t = x->SID_Voice1.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 4, (Uint8)u); //write to SID
						x->SID_Voice1.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice1.CONTROL; //load backup
						Uint8 s = r & 0b11110111; //clear bit
						Uint8 t = x->SID_Voice1.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 4, (Uint8)u); //write to SID
						x->SID_Voice1.CONTROL = s; //do backup

					}
					break;
				case 1:
					if (w == 1) {
						Uint8 r = x->SID_Voice2.CONTROL; //load backup
						Uint8 s = r | 0b00001000; //set Bit
						Uint8 t = x->SID_Voice2.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 11, (Uint8)u); //write to SID
						x->SID_Voice2.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice2.CONTROL; //load backup
						Uint8 s = r & 0b11110111; //clear bit
						Uint8 t = x->SID_Voice2.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 11, (Uint8)u); //write to SID
						x->SID_Voice2.CONTROL = s; //do backup
					}
					break;
				case 2:
					if (w == 1) {
						Uint8 r = x->SID_Voice3.CONTROL; //load backup
						Uint8 s = r | 0b00001000; //set Bit
						Uint8 t = x->SID_Voice3.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 18, (Uint8)u); //write to SID
						x->SID_Voice3.CONTROL = s; //do backup
					}
					else {
						Uint8 r = x->SID_Voice3.CONTROL; //load backup
						Uint8 s = r & 0b11110111; //clear bit
						Uint8 t = x->SID_Voice3.WAVEFORM;
						Uint8 u = (t << 4) + s; //restore register
						push_event(x, 18, (Uint8)u); //write to SID
						x->SID_Voice3.CONTROL = s; //do backup
					}
					break;
				}
			}
			else {
				post("sid(test): error!!! value out of range!");
			}
		}
		else {
			post("sid(test): error!!! not right Parameters!");
		}
	}
	else {
		post("sid(test): error!!! not 2 Parameters!");
	}
}

void sid_play(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 1) {
		//OK there is 1 parameter
		if (atom_gettype(ap++) == A_LONG) {
			ap = argv;
			int n = atom_getlong(ap++);
			Uint8 w, c = 0;
			switch (n) {
				case 0:
					w = x->SID_Voice1.WAVEFORM;	// Get Waveform-Nible
					c = x->SID_Voice1.CONTROL;
					c = c | 0b00000001;
					push_event(x, 4, (Uint8)((w << 4)+(c)));
					x->SID_Voice1.CONTROL = c;
					break;
				case 1: 
					w = x->SID_Voice2.WAVEFORM;	// Get Waveform-Nible
					c = x->SID_Voice2.CONTROL;
					c = c | 0b00000001;
					push_event(x, 11, (Uint8)((w << 4) + (c)));
					x->SID_Voice2.CONTROL = c;
					break;
				case 2:
					w = x->SID_Voice3.WAVEFORM;	// Get Waveform-Nible
					c = x->SID_Voice3.CONTROL;
					c = c | 0b00000001;
					push_event(x, 18, (Uint8)((w << 4) + (c)));
					x->SID_Voice3.CONTROL = c;
					break;
				default: {
					post("sid(play): error!!! not yet a Voice!");
					break;
				}
			}
		}
		else {
			post("sid(play): error!!! wrong Parameter(s)!");
		}
	}
	else {
		post("sid(play): error!!! not 1 Parameter!");
	}
}

void sid_stop(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 1) {
		//OK there is 1 parameter
		if (atom_gettype(ap++) == A_LONG) {
			ap = argv;
			int n = atom_getlong(ap++);
			Uint8 w, c = 0;
			switch (n) {
				case 0:
					w = x->SID_Voice1.WAVEFORM;	// Get Waveform-Nible
					c = x->SID_Voice1.CONTROL;
					c = c & 0b11111110;
					push_event(x, 4, (Uint8)((w << 4) + (c)));
					x->SID_Voice1.CONTROL = c;
					break;
				case 1: 
					w = x->SID_Voice2.WAVEFORM;	// Get Waveform-Nible
					c = x->SID_Voice2.CONTROL;
					c = c & 0b11111110;
					push_event(x, 11, (Uint8)((w << 4) + (c)));
					x->SID_Voice2.CONTROL = c;
					break;
				case 2:
					w = x->SID_Voice3.WAVEFORM;	// Get Waveform-Nible
					c = x->SID_Voice3.CONTROL;
					c = c & 0b11111110;
					push_event(x, 18, (Uint8)((w << 4) + (c)));
					x->SID_Voice3.CONTROL = c;
					break;
				default:
					post("sid(stop): error!!! not yet a Voice!");
					break;
			}
		}
		else {
			post("sid(stop): error!!! wrong Parameter(s)!");
		}
	}
	else {
		post("sid(stop): error!!! not 1 Parameter!");
	}
}

void sid_anything(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	post("sid: error! wrong message!");
}

void sid_freq1(t_sid *x, long n)
{
		//OK: n is a real frequency
		//Calculate SID-frequency
		float SF = (17.02841924 * (float)n);
		SF += 0.5;
		int SID_FREQ = SF;
		if ((SID_FREQ >= 0) && (SID_FREQ <= 65535))
		{
			int SF_HI = SID_FREQ / 256;
			int SF_LOW = SID_FREQ - (256 * SF_HI);
			//set frequency of voice #1
			push_event(x, 0, (Uint8)SF_LOW);
			push_event(x, 1, (Uint8)SF_HI);
			x->SID_Voice1.FREQ_LO = SF_LOW;
			x->SID_Voice1.FREQ_HI = SF_HI;
		}
		else {
			post("SID (freq1) error! value out of range");
		}	
}

void sid_freq2(t_sid *x, long n)
{
		//OK: n is a real frequency
		//Calculate SID-frequency
		float SF = (17.02841924 * (float)n);
		SF += 0.5;
		int SID_FREQ = SF;
		if ((SID_FREQ >= 0) && (SID_FREQ <= 65535))
		{
			int SF_HI = SID_FREQ / 256;
			int SF_LOW = SID_FREQ - (256 * SF_HI);
			//set frequency of voice #2
			push_event(x, 7, (Uint8)SF_LOW);
			push_event(x, 8, (Uint8)SF_HI);
			x->SID_Voice2.FREQ_LO = SF_LOW;
			x->SID_Voice2.FREQ_HI = SF_HI;
		}
		else {
			post("SID (freq2) error! value out of range");

		}
}

void sid_freq3(t_sid *x, long n)
{
		//OK: n is a real frequency
		//Calculate SID-frequency
		float SF = (17.02841924 * (float)n);
		SF += 0.5;
		int SID_FREQ = SF;
		if ((SID_FREQ >= 0) && (SID_FREQ <= 65535))
		{
			int SF_HI = SID_FREQ / 256;
			int SF_LOW = SID_FREQ - (256 * SF_HI);
			//set frequency of voice #3
			push_event(x, 14, (Uint8)SF_LOW);
			push_event(x, 15, (Uint8)SF_HI);
			x->SID_Voice3.FREQ_LO = SF_LOW;
			x->SID_Voice3.FREQ_HI = SF_HI;
		}
		else {
				post("SID (freq3) error! value out of range");
		}
}

void sid_filter_freq(t_sid *x, long n)
{
	if ((n >= 0) && (n <= 2047)) {
		//OK: n is a real frequency
		Uint16 m = (Uint16)n;
		Uint16 LO = m & 0b0000000000000111;
		Uint16 HI = m >> 3;
		//set filterfrequency
		push_event(x, 21, (Uint8)LO);
		push_event(x, 22, (Uint8)HI);
		x->SID_Voice1.FIL_FREQ_LOW = LO;
		x->SID_Voice1.FIL_FREQ_HIGH = HI;
	}
	else {
		post("sid(freq): error!!! value out of range");
	}
}

void sid_filter_res(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 1) {
		//OK there is 1 parameter
		if (atom_gettype(ap++) == A_LONG) {
			ap = argv;
			Uint8 n = atom_getlong(ap++);
			//n is res 0-15
			if ((n >= 0) && (n <= 15)) {
				Uint8 m = (Uint8)n;
				Uint8 o = x->SID_Voice1.FILT;
				Uint8 p = (Uint8)((m << 4) + o);
				push_event(x, 23, (Uint8)p);
				x->SID_Voice1.RES = (Uint8)n;
			}
			else {
				post("sid(res): error!!! value out of range");
			}
		}
	}
	else {
		post("sid(res): error!!! not 1 parameter!");
	}
}

void sid_filter_filt(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 1) {
		//OK there is 1 parameters
		if ((atom_gettype(ap++) == A_LONG)) {
			ap = argv;
			Uint8 n = atom_getlong(ap++);
			if (n >= 0 && n <= 15) {
					Uint8 m = x->SID_Voice1.RES;	// Get res-Nible
					push_event(x, 23, (Uint8)(n + (m << 4)));
					x->SID_Voice1.FILT = n;		//Store filt
			}
			else {
				post("sid(filt): error!!! value out of range!");
			}
		}
		else {
			post("sid(filt): error!!! not right parameters!");
		}
	}
	else {
		post("sid(filt): error!!! not 1 parameter!");
	}
}

void sid_filter_mode(t_sid *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	ap = argv;
	if (argc == 1) {
		//OK there is 1 parameters
		if ((atom_gettype(ap++) == A_LONG)) {
			ap = argv;
			Uint8 n = atom_getlong(ap++);
			if (n >= 0 && n <= 15) {
				Uint8 m = x->SID_Voice1.VOLUME;	// Get vol-Nible
				push_event(x, 24, (Uint8)((n << 4) + m));
				x->SID_Voice1.FILTER = n;		//Store filt
			}
			else {
				post("sid(mode): error!!! value out of range!");
			}
		}
		else {
			post("sid(mode): error!!! not right parameters!");
		}
	}
	else {
		post("sid(mode): error!!! not 1 parameter!");
	}
}

//////////////////////////////////////////////////////////////////////////////////

int cb_init(t_sid *x, circular_buffer *cb, size_t capacity, size_t sz)
{
	//cb->buffer = malloc(capacity * sz);
	cb->buffer = sysmem_newptr(capacity * sz);
	post("I have a pointer %lx and it is %ld bytes in size", cb->buffer, sysmem_ptrsize(cb->buffer));
	if (cb->buffer == NULL) {
		// handle error
		post("mem alloc error!");
		return (1);
	}
		
	cb->buffer_end = (char *)cb->buffer + capacity * sz;
	cb->capacity = capacity;
	cb->count = 0;
	cb->sz = sz;
	cb->head = cb->buffer;
	cb->tail = cb->buffer;
	return (0);
}

int cb_empty(t_sid *x, circular_buffer *cb) {
	return(0 == cb->count);
}

void cb_free(t_sid *x, circular_buffer *cb)
{
	//free(cb->buffer);
	sysmem_freeptr(cb->buffer);
	// clear out other fields too, just to be safe
}

int cb_push_back(t_sid *x, circular_buffer *cb, const void *item)
{
	if (cb->count == cb->capacity) {
		// handle error
		post("buffer overflow!");
		return (1);
	}
	memcpy(cb->head, item, cb->sz);
	cb->head = (char*)cb->head + cb->sz;
	if (cb->head == cb->buffer_end)
		cb->head = cb->buffer;
	cb->count++;
	return (0);
}

int cb_pop_front(t_sid *x, circular_buffer *cb, void *item)
{
	if (cb->count == 0) {
		// handle error
		post("count= 0");
		return (1);
	}
	memcpy(item, cb->tail, cb->sz);
	cb->tail = (char*)cb->tail + cb->sz;
	if (cb->tail == cb->buffer_end)
		cb->tail = cb->buffer;
	cb->count--;
	return (0);
}
