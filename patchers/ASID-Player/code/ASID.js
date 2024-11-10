//parse ASID data
inlets = 2;
outlets = 2;
// global variables
var idtable = [0,1,2,3,5,6,7,8,9,10,12,13,14,15,16,17,19,20,21,22,23,24,4,11,18,4,11,18];

function list() {
	var myArray = arguments;
	var NumberOfDataBytes = 0;
	var dataP=10;
	var register=0;
	var data =0;
	var LSBdata =0;
	
	for (var i=0; i<=3; i++){
		NumberOfDataBytes = NumberOfDataBytes + number_of_bits(myArray[i+2]);
  	}
	if ((NumberOfDataBytes + 8 + 2) == myArray.length){
		for (var i=0; i<=3; i++){
			var count = 0;
			for (var mask=0x01; mask != 0x80 ; mask <<= 1){
				
				if (myArray[i+2] & mask){
					//make a sid write
					register=idtable[i*7+count];
			
					LSBdata = myArray[dataP];
					dataP++;
					
					if (myArray[i+6] & mask){
						data=LSBdata + 127;
					}
				    else {
						data=LSBdata;
					}
					outlet(1, register);
					outlet(0, data);
				}
				else {
				
				}
				count++;	
			}
		}
		dataP = 10;
	}
}
 
function number_of_bits(value){
	var ones = 0;
	ones += value & 0x01;
	ones += (value & 0x02)>>1;
	ones += (value & 0x04)>>2;
	ones += (value & 0x08)>>3;
	ones += (value & 0x10)>>4;
	ones += (value & 0x20)>>5;
	ones += (value & 0x40)>>6;
	ones += (value & 0x80)>>7;
	return ones;
}
