This test actually makes use of the multiple segment functionality of the WHEEL file format. There is a segment (a "wedge") that contains an array in memory as described below, as well as some code in a different wedge.

Here's what the array looks like:  
HIGH MEMORY  
"Hello World!" '\0'  
"Prithvi J" '\0'  
"pARMesan" '\0'  
"here's a string" '\0'   
4 16-bit pointers to the aforementioned strings  
LOW MEMORY  

This is laid out starting at address 0x1000.

As for the code, the first thing to do is to load the address 0x1000 into some register. This is the pointer to the array of strings. Then we also need to load the special MMIO print address. We will need to use the stack to save some of these variables.

strlen(char *str) {
  short length = 0; // %cx
  char ax = *str;
  str++;
  goto test;
  length++;
  char ax = *str;
  str++;
test:
  if ax != 0
    goto loop;
  return length;
}