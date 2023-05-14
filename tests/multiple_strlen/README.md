This test is made to work with a special memory dump file. This memory dump file is constructed such that the memory contains a list of strings, and a list of pointers to these strings. In other words, a char*[]. Also on the stack is a pointer to this array. The idea is we have to traverse the array in order to read elements off of it.

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