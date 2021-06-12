#include <stdio.h>   // Provides for printf()
#include <stdlib.h>  // Provides for system()
#include <argp.h>    // Provides for argp()
#include <string.h>  // Provides for strcmp()
#include <unistd.h>  // Provides for access()
#include <stdbool.h> // Provides for bool

// Function prototype
int check_file (const char* filename);
char* get_base_addr (const char* filename, char* args_base);
int calc_reg_addr (char* base, char* offset);
char* read_reg (char* base, char* offset, bool display);
char* write_reg (char* base, char* offset, char* wr_value);
void read_regspec (char* filename, char* device_id, char* base_addr);
void read_regspec_dump (char* filename, char* device_id, char* base_addr, char* output_file);
void dump_pci_config (char* device_id);

static char args_doc[] = "";
static char doc[] = "MMIO register parsing tool with web-connected utility";

const char* base_addr_file = "base_addr.txt";

//----------------------------------------------------------
//--                  Setting up ARGP                     --
//----------------------------------------------------------

// Options. Field 1 in ARGP
// Order of fields: {NAME, KEY, ARG, FLAGS, DOC, GROUP}
static struct argp_option options[] = {
   {"device",        'd', "Device ID",       0, "PCI Device ID",                                     0},
   {"pci",           888, 0,                 0, "Dump all PCI Config Space",                         0},
   {"mmio",          777, 0,                 0, "Dump MMIO register and value",                      0},
   {"base",          'b', "Base address",    0, "MMIO base address",                                 0},
   {"spec",          's', "Register spec",   0, "Register spec with MMIO register offset",           0},
   {"file",          'f', "Output file",     0, "Output file with all the MMIO registers and value", 0},
   {"offset",        'o', "Register offset", 0, "MMIO register offset",                              0},
   {"read",          'r', 0,                 0, "Display a specific MMIO register",                  0},
   {"write",         'w', "Register write",  0, "Write a value to specific MMIO register",           0},
   {"set-mmio-base", 666, 0,                 0, "Set MMIO base address for MMIO operations",         0},
   {0, 0, 0, 0, 0, 0} // Last entry should be all zeros in all fields
};

// Used by main to communicate with parse_opt
struct arguments {
   char *device;
   int pci;
   int mmio;
   char *base;
   char *spec;
   char *file;
   char *offset;
   int read;
   char *write;
   char set_base;
};

// Parser. Field 2 in ARGP
// Order of parameters: KEY, ARG, STATE
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
   // Get the input argument from argp_parse, which we know is a pointer to our arguments structure
   struct arguments *arguments = state -> input;

   // Figure out which option we are parsing and decide how to store it
   switch (key)
   {
      case 'd':
         arguments->device = arg;
	 break;
      case 888:
	 arguments->pci = 1;
	 break;
      case 777:
	 arguments->mmio = 1;
	 break;
      case 'b':
	 arguments->base = arg;
	 break;
      case 's':
	 arguments->spec = arg;
	 break;
      case 'f':
	 arguments->file = arg;
	 break;
      case 'o':
	 arguments->offset = arg;
	 break;
      case 'r':
	 arguments->read = 1;
	 break;
      case 'w':
	 arguments->write = arg;
	 break;
      case 666:
	 arguments->set_base = 1;
	 break;
   }
   return 0;
}


// Our argp parser
static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

//----------------------------------------------------------
//--                    MAIN Function                     --
//----------------------------------------------------------
int main (int argc, char **argv) {
   struct arguments arguments;

   // Set arguments default
   arguments.device = "";
   arguments.pci = 0;
   arguments.mmio = 0;
   arguments.base = "";
   arguments.spec = "";
   arguments.file = "";
   arguments.offset = "";
   arguments.read = 0;
   arguments.write = "";
   arguments.set_base = 0;

   argp_parse(&argp, argc, argv, 0, 0, &arguments);

   if(arguments.pci) {
      dump_pci_config(arguments.device);
   }

   if(strcmp(arguments.spec,"") != 0 && strcmp(arguments.file,"") == 0) {
      char* base_addr;
      base_addr = get_base_addr(base_addr_file, arguments.base);

      read_regspec(arguments.spec, arguments.device, base_addr);
   } else if (strcmp(arguments.file,"") != 0) {
      char* base_addr;
      base_addr = get_base_addr(base_addr_file, arguments.base);

      read_regspec_dump(arguments.spec, arguments.device, base_addr, arguments.file);
   }


   if(arguments.read) {
      char* base_addr;
      char* ret;
      base_addr = get_base_addr(base_addr_file, arguments.base);
      
      ret = read_reg(arguments.offset, base_addr,1);
   }

   if(strcmp(arguments.write,"") != 0) {
      char* base_addr;
      base_addr = get_base_addr(base_addr_file, arguments.base);

      write_reg(arguments.offset, base_addr, arguments.write);
   }

   if(arguments.set_base) {
      FILE *fptr;

      fptr = fopen("base_addr.txt", "w");
      fprintf(fptr, "%s", arguments.base);
      fclose(fptr);
   }
    return 0;
}


//----------------------------------------------------------
//--        Function - Check base_addr.txt existance      --
//----------------------------------------------------------
int check_file (const char* filename) {
   if(access(filename, F_OK) == 0) {
      return true;
   } else {
      return false;
   }
}


//----------------------------------------------------------
//--            Function - Get base address               --
//----------------------------------------------------------
char* get_base_addr (const char* filename, char* args_base) {
   bool base_set;
   static char base [15];

   base_set = check_file(base_addr_file);
   if(base_set) { 
      FILE *fptr;
      
      if((fptr = fopen(filename,"r")) == NULL) {
         printf("Error opening the %s...\n", filename);
      }
      fscanf(fptr, "%[^\n]", base);
      fclose(fptr);
   } else {
      strcpy(base, args_base);
   }
   
   return base;
}


//----------------------------------------------------------
//--        Function - Calculate register address         --
//----------------------------------------------------------
int calc_reg_addr (char* base, char* offset) {
   return ((int)strtol(base, NULL, 0) + (int)strtol(offset, NULL, 0));
}

//----------------------------------------------------------
//--                 Function - Read register             --
//----------------------------------------------------------
char* read_reg (char* base, char* offset, bool display) {
   FILE *fp;
   char cmd [50];
   static char line [50];

   int regAddr = calc_reg_addr(base, offset);
   snprintf(cmd, sizeof(cmd), "busybox devmem 0x%x\n", regAddr);
   fp = popen(cmd, "r");
   
   while(fgets(line, sizeof(line), fp) != 0) {
	line[strcspn(line, "\n")] = 0; // Remove trailing newline
	if(display)
	   printf("[READ] Value at address 0x%x: %s\n", regAddr, line);
	return line;
   }
   pclose(fp);
}


//----------------------------------------------------------
//--                Function - Write register             --
//----------------------------------------------------------
char* write_reg (char* base, char* offset, char* wr_value) {
   char cmd [50];
   int reg_addr = calc_reg_addr(base, offset);
   snprintf(cmd, sizeof(cmd), "busybox devmem 0x%x w %s\n", reg_addr, wr_value);
   system(cmd);
   printf("[WRITE] %s is written into address 0x%x successfully\n", wr_value, reg_addr);
}


//----------------------------------------------------------
//--               Function - Read regspec.txt            --
//----------------------------------------------------------
void read_regspec (char* filename, char* device_id, char* base_addr) {
   FILE *fptr;
   char line [30] = "";
   char reg_name [50] = {""};
   char offset [50] = {""};

   printf("#########################################\n");
   printf("#  PCI Device ID=%s\n", device_id);
   printf("#  MMIO base[<NUM>]=%s\n", base_addr);
   printf("#########################################\n");
   
   fptr = fopen(filename, "r");

   while((fgets(line, sizeof(line), fptr))!= 0) {
      if(line[0] != '#' && line[0] != '\n') {
         sscanf(line, "%[^=]=%s", reg_name, offset);
         char* ret;
         ret = read_reg(offset, base_addr, 0);
         
         printf("%s[0x%x]=%s\n", reg_name, calc_reg_addr(base_addr, offset), ret);
         //printf("%s register --> %s\n", reg[i], addr[i]);
      } else {
         printf("%s", line);
      }
   }
   fclose(fptr);
}


//----------------------------------------------------------
//--        Function - Read regspec.txt with dump         --
//----------------------------------------------------------
void read_regspec_dump (char* filename, char* device_id, char* base_addr, char* output_file) {
   FILE *fptr;
   FILE *optr;
   char line [30] = "";
   char reg_name [50] = {""};
   char offset [50] = {""};
   
   fptr = fopen(filename, "r");
   optr = fopen(output_file, "w");

   fprintf(optr, "#########################################\n");
   fprintf(optr, "#  PCI Device ID=%s\n", device_id);
   fprintf(optr, "#  MMIO base[<NUM>]=%s\n", base_addr);
   fprintf(optr, "#########################################\n");

   while((fgets(line, sizeof(line), fptr))!= 0) {
      if(line[0] != '#' && line[0] != '\n') {
         sscanf(line, "%[^=]=%s", reg_name, offset);
         char* ret;
         ret = read_reg(offset, base_addr, 0);
         
         fprintf(optr, "%s[0x%x]=%s\n", reg_name, calc_reg_addr(base_addr, offset), ret);
         //printf("%s register --> %s\n", reg[i], addr[i]);
      } else {
         fprintf(optr, "%s", line);
      }
   }
   fclose(fptr);
   fclose(optr);
}

//----------------------------------------------------------
//--               Function - PCI Config space            --
//----------------------------------------------------------
void dump_pci_config (char* device_id) {
   FILE *fptr;
   char cmd [] = "setpci --dumpregs";
   static char line [50];

   fptr = popen(cmd, "r");
   
   int addr;
   char size;
   char reg_name [40];
   while(fgets(line, sizeof(line), fptr) != 0) {
      char exclude1 [] = "CAP";
      char exclude2 [] = "cap";
      char *sptr1 = strstr(line, exclude1);
      char *sptr2 = strstr(line, exclude2);
      if(sptr1 == NULL && sptr2 == NULL) {
         sscanf(line, "%x %c %s", &addr, &size, reg_name);

	 FILE *rptr;
	 char reg_cmd [70];
         static char reg_line [50];

         snprintf(reg_cmd, sizeof(reg_cmd), "setpci -d :%s %s", device_id, reg_name);
         rptr = popen(reg_cmd, "r");
         
         while(fgets(reg_line, sizeof(reg_line), rptr) != 0) {
	      reg_line[strcspn(reg_line, "\n")] = 0; // Remove trailing newline
              printf("%s: %s\n", reg_name, reg_line);
         }
      } 
   }
   pclose(fptr);

}
