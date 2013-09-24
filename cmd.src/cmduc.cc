/*
 * Simulator of microcontrollers (cmd.src/cmduc.cc)
 *
 * Copyright (C) 2001,01 Drotos Daniel, Talker Bt.
 * 
 * To contact author send email to drdani@mazsola.iit.uni-miskolc.hu
 *
 */

/* This file is part of microcontroller simulator: ucsim.

UCSIM is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

UCSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with UCSIM; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA. */
/*@1@*/

#include <string>

#include "../s51.src/regs51.h"

// prj
#include "globals.h"
#include "utils.h"

// sim.src
#include "uccl.h"

// local, cmd.src
#include "cmduccl.h"
using namespace std;

#define DEBUG
#ifdef DEBUG
#define TRACE() \
fprintf(stderr, "%s:%d in %s()\n", __FILE__, __LINE__, __FUNCTION__)
#else
#define TRACE()
#endif

/*
 * Command: state
 *----------------------------------------------------------------------------
 */

//int
//cl_state_cmd::do_work(class cl_sim *sim,
//		      class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_state_cmd)
{
  con->dd_printf("CPU state= %s PC= 0x%06x XTAL= %g\n",
		 get_id_string(cpu_states, uc->state),
		 uc->PC, 
		 uc->xtal);
  con->dd_printf("Total time since last reset= %g sec (%lu clks)\n",
		 uc->get_rtime(), uc->ticks->ticks);
  /*con->dd_printf("Time in isr = %g sec (%lu clks) %3.2g%%\n",Calypso
		 uc->isr_ticks->get_rtime(uc->xtal),
		 uc->isr_ticks->ticks,
		 (uc->ticks->ticks == 0)?0.0:
		 (100.0*((double)(uc->isr_ticks->ticks)/
			 (double)(uc->ticks->ticks))));
  con->dd_printf("Time in idle= %g sec (%lu clks) %3.2g%%\n",
		 uc->idle_ticks->get_rtime(uc->xtal),
		 uc->idle_ticks->ticks,
		 (uc->ticks->ticks == 0)?0.0:
		 (100.0*((double)(uc->idle_ticks->ticks)/
		 (double)(uc->ticks->ticks))));*/
  con->dd_printf("Max value of stack pointer= 0x%06x, avg= 0x%06x\n",
		 uc->sp_max, uc->sp_avg);
  return(0);
}


/*
 * Command: file
 *----------------------------------------------------------------------------
 */

//int
//cl_file_cmd::do_work(class cl_sim *sim,
//		     class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_file_cmd)
{
  char *fname= 0;
  long l;
  
  if ((cmdline->param(0) == 0) ||
      ((fname= cmdline->param(0)->get_svalue()) == NULL))
    {
      con->dd_printf("File name is missing.\n");
      return(0);
    }
  if ((l= uc->read_hex_file(fname)) >= 0)
    con->dd_printf("%ld words read from %s\n", l, fname);

  return(0);
}


/*
 * Command: download
 *----------------------------------------------------------------------------
 */

//int
//cl_dl_cmd::do_work(class cl_sim *sim,
//		   class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_dl_cmd)
{
  long l;
  
  if ((l= uc->read_hex_file(NULL)) >= 0)
    con->dd_printf("%ld words loaded\n", l);

  return(0);
}


/*
 * Command: pc
 *----------------------------------------------------------------------------
 */

//int
//cl_pc_cmd::do_work(class cl_sim *sim,
//		   class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_pc_cmd)
{
  t_addr addr;
  class cl_cmd_arg *params[1]= { cmdline->param(0) };

  if (params[0])
    {
      if (!(params[0]->get_address(uc, &addr)))
	{
	  con->dd_printf("Error: wrong parameter\n");
	  return(DD_FALSE);
	}
      class cl_address_space *rom= uc->address_space(MEM_ROM_ID);
      if (rom)
	{
	  if (addr > rom->highest_valid_address())
	    addr= rom->highest_valid_address();
	}
      if (!uc->inst_at(addr))
	con->dd_printf("Warning: maybe not instruction at 0x%06x\n", addr);
      uc->PC= addr;
    }
  uc->print_disass(uc->PC, con);
  return(DD_FALSE);
}


/*
 * Command: reset
 *----------------------------------------------------------------------------
 */

//int
//cl_reset_cmd::do_work(class cl_sim *sim,
//		      class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_reset_cmd)
{
  uc->reset();
  return(0);
}


/*
 * Command: dump
 *----------------------------------------------------------------------------
 */

//int
//cl_dump_cmd::do_work(class cl_sim *sim,
//		     class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_dump_cmd)
{
  class cl_memory *mem = NULL;
  class cl_cmd_arg *params[4]= { cmdline->param(0),
				 cmdline->param(1),
				 cmdline->param(2),
				 cmdline->param(3) };

  // 1. Dumping registers that should be broken down into identified bits:
  if (params[0] &&
      params[0]->as_bit(uc))
    {
      int i= 0;
      while (params[0] &&
	     params[0]->as_bit(uc))
	{
	  t_mem m;
	  mem= params[0]->value.bit.mem;
	  m= mem->read(params[0]->value.bit.mem_address);
	  char *sn=
	    uc->symbolic_bit_name((t_addr)-1,
				  mem,
				  params[0]->value.bit.mem_address,
				  params[0]->value.bit.mask);
	  con->dd_printf("%10s ", sn?sn:"");
	  con->dd_printf(mem->addr_format, params[0]->value.bit.mem_address);
	  con->dd_printf(" ");
	  con->dd_printf(mem->data_format, m);
	  con->dd_printf(" %c\n", (m&(params[0]->value.bit.mask))?'1':'0');
	  i++;
	  params[0]= cmdline->param(i);
	}
      if (params[0])
	con->dd_printf("%s\n", short_help?short_help:"Error: wrong syntax\n");

      return(DD_FALSE);
    }


  // 2. Error message if no parameter or if parameter is not of type 'memory':
  if (!params[0] ||
      !params[0]->as_memory(uc))
    {
      con->dd_printf("No memory specified. Use \"info memory\" for available memories\n");
      return(DD_FALSE);
    }


  // 3. Dump a memory:

  // 3.0 Debug message
  /*  {
    string fname;
    fname = cmdline->param(0)->get_svalue();
    fprintf(stderr, "memory: %s\n", fname.c_str());
    }*/

  // 3.1 Define the interval to print, depending on arguments of dump command:
  t_addr d_start = 0;
  t_addr d_end   = 0;
  long   bpl   = 8;
  mem  = params[0]->value.memory.memory;

  if (cmdline->syntax_match(uc, MEMORY)) {
    d_start= mem->dump_finished;
    //fprintf(stderr, "mem->dump_finished is 0x%02x\n",mem->dump_finished);
    d_end  = d_start+10*8-1;
  } else if (cmdline->syntax_match(uc, MEMORY ADDRESS)) {
    d_start= params[1]->value.address;
    d_end  = d_start+10*8-1;
  } else if (cmdline->syntax_match(uc, MEMORY ADDRESS ADDRESS)) {
    d_start= params[1]->value.address;
    d_end  = params[2]->value.address;
  } else if (cmdline->syntax_match(uc, MEMORY ADDRESS ADDRESS NUMBER)) {
    d_start= params[1]->value.address;
    d_end  = params[2]->value.address;
    bpl  = params[3]->value.number;	
  }
  else {
    con->dd_printf("%s\n", short_help?short_help:"Error: wrong syntax\n");
    return(DD_FALSE);
  }

  class cl_memory *xram = uc->memory("xram");
  class cl_memory *rom = uc->memory("rom");
  class cl_memory *sfr = uc->memory("sfr");

  if (mem == xram) {
    int i;
    int tab_size;
    int count_no_intersect = 0;

    class cl_memory * tab[4];
    tab[0] = uc->memory("sfr");
    tab[1] = uc->memory("sram");
    tab[2] = uc->memory("iram");

    tab_size=(sizeof tab)/(sizeof(cl_memory *));

    int memctr = sfr->read(MCON) & 0x07;
    switch (memctr)
      {
      case 0:
	tab[3]= uc->memory("flashbank0");
	break;
      case 1:
	tab[3]= uc->memory("flashbank1");
	break;
      case 2:
	tab[3]= uc->memory("flashbank2");
	break;
      case 3:
	tab[3]= uc->memory("flashbank3");
	break;
      default:
	tab[3]= uc->memory("flashbank0");
	break;
      }
    fprintf(stderr,"The flashbank mapped to xram is %s.\n", tab[3]->name);
 
    for (i=0; i<tab_size; i++){

      // 1. Check whether there is an intersection with flash, sfr, iram (=data):
      t_addr r_start = tab[i]->xram_offset + tab[i]->start_address;
      t_addr r_end   = tab[i]->xram_offset + tab[i]->start_address + tab[i]->size;
 

      //fprintf(stderr,"Case of %s, start 0x%02x, end 0x%02x, offset 0x%02x\n",
      //	      tab[i]->name, r_start, r_end, tab[i]->xram_offset);
      if (i==1) 
	r_end = r_end - 0x100;//sram case (sram_end-100 is beginning of iram)

      if (has_intersection(d_start, d_end, r_start, r_end)) {
	if ((d_start <= r_start) && (d_end >= r_start) && (d_end <= r_end)) {
	  fprintf(stderr,"Dump of xram (including beginning of %s).\n", tab[i]->name);
	  xram->dump(d_start, r_start-1, bpl, con);
	  fprintf(stderr,"\n*******Beginning of %s section of xram.*******\n\n",
		  tab[i]->name);
	  xram->dump(r_start, d_end, bpl, con);
	}
	else if ((d_end >= r_start) && (d_end <= r_end) && 
		 (d_start >= r_start) && (d_start <= r_end)) {
	  fprintf(stderr,"Dumping part of %s section of xram.\n", tab[i]->name); 
	  xram->dump(d_start, d_end, bpl, con);
	}
	else if ((d_end >= r_end) && (d_start <= r_start)) {
	  fprintf(stderr,"Dump of xram (including the %s section).\n", tab[i]->name);
	  xram->dump(d_start, r_start-1, bpl, con);
	  fprintf(stderr,"\n*******Beginning of %s section of xram.*******\n\n",
		  tab[i]->name);
	  xram->dump(r_start, r_end, bpl, con);
	  fprintf(stderr,"\n*******End of %s section of xram.*******\n\n",
		  tab[i]->name);

	  d_start = r_end;
	  xram->dump(r_end, d_end, bpl, con);
	}
	else if ((d_end >= r_end) && (d_start >= r_start) && (d_start <= r_end)) {
	  fprintf(stderr,"Dump of xram (including end of %s).\n", tab[i]->name);
	  xram->dump(d_start, r_end - 1, bpl, con);
	  fprintf(stderr,"\n*******End of %s section of xram.*******\n\n",
		  tab[i]->name); 
 	  //xram->dump(r_end, d_end, bpl, con);
	  d_start = r_end;
	  xram->dump(r_end, d_end, bpl, con);
	}
      }
      else {
	//used for case where there has been no intersection 
	//of dump section with any specific parts of the xram
	count_no_intersect++;
      }
    }
    if (count_no_intersect == tab_size){
      mem->dump(d_start, d_end, bpl, con);
    }
  }
  else if(mem == rom){
    int fmap = sfr->read(FMAP) & 0x07;
    fprintf(stderr,"The flashbank mapped to the upper part of rom is flashbank%d\n",
	    fmap); 
    mem->dump(d_start, d_end, bpl, con);    
  }
  else {
    //fprintf(stderr,"About to dump %s, from @ 0x%04x to 0x%04x\n",
    // mem->name, d_start, d_end); 
    mem->dump(d_start, d_end, bpl, con);

  }
  return(DD_FALSE);
}


/*
 * Command: debug_dump
 *----------------------------------------------------------------------------
 */

//int
//cl_dump_cmd::do_work(class cl_sim *sim,
//		     class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_debug_dump_cmd)
{
  class cl_memory *mem = NULL;
  class cl_cmd_arg *params[4]= { cmdline->param(0),
				 cmdline->param(1),
				 cmdline->param(2),
				 cmdline->param(3) };

  // 1. Dumping registers that should be broken down into identified bits:
  if (params[0] &&
      params[0]->as_bit(uc))
    {
      int i= 0;
      while (params[0] &&
	     params[0]->as_bit(uc))
	{
	  t_mem m;
	  mem= params[0]->value.bit.mem;
	  m= mem->read(params[0]->value.bit.mem_address);
	  char *sn=
	    uc->symbolic_bit_name((t_addr)-1,
				  mem,
				  params[0]->value.bit.mem_address,
				  params[0]->value.bit.mask);
	  con->dd_printf("%10s ", sn?sn:"");
	  con->dd_printf(mem->addr_format, params[0]->value.bit.mem_address);
	  con->dd_printf(" ");
	  con->dd_printf(mem->data_format, m);
	  con->dd_printf(" %c\n", (m&(params[0]->value.bit.mask))?'1':'0');
	  i++;
	  params[0]= cmdline->param(i);
	}
      if (params[0])
	con->dd_printf("%s\n", short_help?short_help:"Error: wrong syntax\n");

      return(DD_FALSE);
    }


  // 2. Error message if no parameter or if parameter is not of type 'memory':
  if (!params[0] ||
      !params[0]->as_memory(uc))
    {
      con->dd_printf("No memory specified. Use \"info memory\" for available memories\n");
      return(DD_FALSE);
    }



  // 3. Dump a memory:

  // 3.0 Debug message
  /*  {
    string fname;
    fname = cmdline->param(0)->get_svalue();
    fprintf(stderr, "memory: %s\n", fname.c_str());
    }*/

  // 3.1 Define the interval to print, depending on arguments of dump command:
  t_addr d_start = 0;
  t_addr d_end   = 0;
  long   bpl   = 8;
  mem  = params[0]->value.memory.memory;

  if (cmdline->syntax_match(uc, MEMORY)) {
    d_start= mem->debug_dump_finished;
    //fprintf(stderr, "mem->debug_dump_finished is 0x%02x\n",mem->debug_dump_finished);
    d_end  = d_start+10*8-1;
  } else if (cmdline->syntax_match(uc, MEMORY ADDRESS)) {
    d_start= params[1]->value.address;
    d_end  = d_start+10*8-1;
  } else if (cmdline->syntax_match(uc, MEMORY ADDRESS ADDRESS)) {
    d_start= params[1]->value.address;
    d_end  = params[2]->value.address;
  } else if (cmdline->syntax_match(uc, MEMORY ADDRESS ADDRESS NUMBER)) {
    d_start= params[1]->value.address;
    d_end  = params[2]->value.address;
    bpl  = params[3]->value.number;	
  }
  else {
    con->dd_printf("%s\n", short_help?short_help:"Error: wrong syntax\n");
    return(DD_FALSE);
  }

  class cl_memory *xram = uc->memory("xram");
  class cl_memory *rom = uc->memory("rom");
  class cl_memory *sfr = uc->memory("sfr");

  if (mem == xram) {
    int i;
    int tab_size;
    int count_no_intersect = 0;

    class cl_memory * tab[4];
    tab[0] = uc->memory("sfr");
    tab[1] = uc->memory("sram");
    tab[2] = uc->memory("iram");

    tab_size=(sizeof tab)/(sizeof(cl_memory *));

    int memctr = sfr->read(MCON) & 0x07;
    switch (memctr)
      {
      case 0:
	tab[3]= uc->memory("flashbank0");
	break;
      case 1:
	tab[3]= uc->memory("flashbank1");
	break;
      case 2:
	tab[3]= uc->memory("flashbank2");
	break;
      case 3:
	tab[3]= uc->memory("flashbank3");
	break;
      default:
	tab[3]= uc->memory("flashbank0");
	break;
      }
    fprintf(stderr,"The flashbank mapped to xram is %s.\n", tab[3]->name);
 
    for (i=0; i<tab_size; i++){

      // 1. Check whether there is an intersection with flash, sfr, iram (=data):
      t_addr r_start = tab[i]->xram_offset + tab[i]->start_address;
      t_addr r_end   = tab[i]->xram_offset + tab[i]->start_address + tab[i]->size;
 

      //fprintf(stderr,"Case of %s, start 0x%02x, end 0x%02x, offset 0x%02x\n",
      //	      tab[i]->name, r_start, r_end, tab[i]->xram_offset);
      if (i==1) 
	r_end = r_end - 0x100;//sram case (sram_end-100 is beginning of iram)

      if (has_intersection(d_start, d_end, r_start, r_end)) {
	if ((d_start <= r_start) && (d_end >= r_start) && (d_end <= r_end)) {
	  fprintf(stderr,"Dump of xram (including beginning of %s).\n", tab[i]->name);
	  xram->debug_dump(d_start, r_start-1, bpl, con);
	  fprintf(stderr,"\n*******Beginning of %s section of xram.*******\n\n",
		  tab[i]->name);
	  xram->debug_dump(r_start, d_end, bpl, con);
	}
	else if ((d_end >= r_start) && (d_end <= r_end) && 
		 (d_start >= r_start) && (d_start <= r_end)) {
	  fprintf(stderr,"Dumping part of %s section of xram.\n", tab[i]->name); 
	  xram->debug_dump(d_start, d_end, bpl, con);
	}
	else if ((d_end >= r_end) && (d_start <= r_start)) {
	  fprintf(stderr,"Dump of xram (including the %s section).\n", tab[i]->name);
	  xram->debug_dump(d_start, r_start-1, bpl, con);
	  fprintf(stderr,"\n*******Beginning of %s section of xram.*******\n\n",
		  tab[i]->name);
	  xram->debug_dump(r_start, r_end, bpl, con);
	  fprintf(stderr,"\n*******End of %s section of xram.*******\n\n",
		  tab[i]->name);
	  //xram->debug_dump(r_end, d_end, bpl, con);
	  d_start = r_end;
	}
	else if ((d_end >= r_end) && (d_start >= r_start) && (d_start <= r_end)) {
	  fprintf(stderr,"Dump of xram (including end of %s).\n", tab[i]->name);
	  xram->debug_dump(d_start, r_end - 1, bpl, con);
	  fprintf(stderr,"\n*******End of %s section of xram.*******\n\n",
		  tab[i]->name); 
 	  //xram->debug_dump(r_end, d_end, bpl, con);
	  d_start = r_end;
	}
      }
      else {
	//used for case where there has been no intersection 
	//of dump section with any specific parts of the xram
	count_no_intersect++;
      }
    }
    if (count_no_intersect == tab_size){
      mem->debug_dump(d_start, d_end, bpl, con);
    }
  }
  else if(mem == rom){
    int fmap = sfr->read(FMAP) & 0x07;
    fprintf(stderr,"The flashbank mapped to the upper part of rom is flashbank%d\n",
	    fmap); 
    mem->debug_dump(d_start, d_end, bpl, con);    
  }
  else {
    //fprintf(stderr,"About to dump %s, from @ 0x%04x to 0x%04x\n",
    // mem->name, d_start, d_end); 
    mem->debug_dump(d_start, d_end, bpl, con);

  }
  return(DD_FALSE);
}



/*
 * Command: di
 *----------------------------------------------------------------------------
 */

//int
//cl_di_cmd::do_work(class cl_sim *sim,
//		   class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_di_cmd)
{
  cmdline->insert_param(0, new cl_cmd_sym_arg("iram"));
  cl_dump_cmd::do_work(uc, cmdline, con);
  return(0);
}


/*
 * Command: dx
 *----------------------------------------------------------------------------
 */

//int
//cl_dx_cmd::do_work(class cl_sim *sim,
//		   class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_dx_cmd)
{
  cmdline->insert_param(0, new cl_cmd_sym_arg("xram"));
  cl_dump_cmd::do_work(uc, cmdline, con);
  return(0);
}


/*
 * Command: dch
 *----------------------------------------------------------------------------
 */

//int
//cl_dch_cmd::do_work(class cl_sim *sim,
//		    class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_dch_cmd)
{
  cmdline->insert_param(0, new cl_cmd_sym_arg("rom"));
  cl_dump_cmd::do_work(uc, cmdline, con);
  return(0);
}


/*
 * Command: ds
 *----------------------------------------------------------------------------
 */

//int
//cl_ds_cmd::do_work(class cl_sim *sim,
//		   class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_ds_cmd)
{
  cmdline->insert_param(0, new cl_cmd_sym_arg("sfr"));
  cl_dump_cmd::do_work(uc, cmdline, con);
  return(0);
}


/*
 * Command: dc
 *----------------------------------------------------------------------------
 */

//int
//cl_dc_cmd::do_work(class cl_sim *sim,
//		   class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_dc_cmd)
{
  t_addr start= last, end= last+20;
  class cl_cmd_arg *params[2]= { cmdline->param(0),
				 cmdline->param(1) };
  class cl_address_space *rom= uc->address_space(MEM_ROM_ID);

  if (!rom)
    return(DD_FALSE);
  if (params[0] == 0)
    ;
  else if (cmdline->syntax_match(uc, ADDRESS)) {
    start= params[0]->value.address;
    end= start+20;
  }
  else if (cmdline->syntax_match(uc, ADDRESS ADDRESS)) {
    start= params[0]->value.address;
    end= params[1]->value.address;
  }
  if (start > rom->highest_valid_address())
    {
      con->dd_printf("Error: start address is too high\n");
      return(DD_FALSE);
    }
  if (end > rom->highest_valid_address())
    {
      con->dd_printf("Error: end address is too high\n");
      return(DD_FALSE);
    }

  for (;
       start <= end;
       start+= uc->inst_length(start))
    uc->print_disass(start, con);
  last= start;
  return(DD_FALSE);
}


/*
 * Command: disassemble
 *----------------------------------------------------------------------------
 */

static int disass_last_stop= 0;

//int
//cl_disassemble_cmd::do_work(class cl_sim *sim,
//			    class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_disassemble_cmd)
{
  t_addr start, realstart;
  int offset= -1, dir, lines= 20;
  class cl_cmd_arg *params[4]= { cmdline->param(0),
				 cmdline->param(1),
				 cmdline->param(2),
				 cmdline->param(3) };

  start= disass_last_stop;
  if (params[0] == 0) ;
  else if (cmdline->syntax_match(uc, ADDRESS)) {
    start= params[0]->value.address;
  }
  else if (cmdline->syntax_match(uc, ADDRESS NUMBER)) {
    start= params[0]->value.address;
    offset= params[1]->value.number;
  }
  else if (cmdline->syntax_match(uc, ADDRESS NUMBER NUMBER)) {
    start= params[0]->value.address;
    offset= params[1]->value.number;
    lines= params[2]->value.number;
  }
  else
    {
      con->dd_printf("%s\n", short_help?short_help:"Error: wrong syntax\n");
      return(DD_FALSE);
    }

  if (lines < 1)
    {
      con->dd_printf("Error: wrong `lines' parameter\n");
      return(DD_FALSE);
    }
  if (!uc->there_is_inst())
    return(DD_FALSE);
  realstart= start;
  class cl_address_space *rom= uc->address_space(MEM_ROM_ID);
  if (!rom)
    return(DD_FALSE);
  while (realstart <= rom->highest_valid_address() &&
	 !uc->inst_at(realstart))
    realstart= realstart+1;
  if (offset)
    {
      dir= (offset < 0)?-1:+1;
      while (offset)
	{
	  realstart= rom->inc_address(realstart, dir);
	  while (!uc->inst_at(realstart))
	    realstart= rom->inc_address(realstart, dir);
	  offset+= -dir;
	}
    }
  
  while (lines)
    {
      uc->print_disass(realstart, con);
      realstart= rom->inc_address(realstart, +1) + rom->start_address;
      while (!uc->inst_at(realstart))
	realstart= rom->inc_address(realstart, +1) + rom->start_address;
      lines--;
    }

  disass_last_stop= realstart;

  return(DD_FALSE);;
}


/*
 * Command: fill
 *----------------------------------------------------------------------------
 */

//int
//cl_fill_cmd::do_work(class cl_sim *sim,
//		     class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_fill_cmd)
{
  class cl_memory *mem= 0;
  t_mem what= 0;
  t_addr start= 0, end;
  class cl_cmd_arg *params[4]= { cmdline->param(0),
				 cmdline->param(1),
				 cmdline->param(2),
				 cmdline->param(3) };

  if (cmdline->syntax_match(uc, MEMORY ADDRESS ADDRESS NUMBER)) {
    mem  = params[0]->value.memory.memory;
    start= params[1]->value.address;
    end  = params[2]->value.address;
    what = params[3]->value.number;
    t_addr i;
    for (i= start; i <= end; i++)
      {
	t_mem d;
	d= what;
	mem->write(i, d);
      }
  }
  else
    con->dd_printf("%s\n", short_help?short_help:"Error: wrong syntax\n");

  return(DD_FALSE);;
}


/*
 * Command: where
 *----------------------------------------------------------------------------
 */

int
cl_where_cmd::do_real_work(class cl_uc *uc,
			   class cl_cmdline *cmdline, class cl_console *con,
			   bool case_sensitive)
{
  class cl_memory *mem= 0;
  class cl_cmd_arg *params[2]= { cmdline->param(0),
				 cmdline->param(1) };

  if (cmdline->syntax_match(uc, MEMORY DATALIST)) {
    mem= params[0]->value.memory.memory;
    t_mem *array= params[1]->value.data_list.array;
    int len= params[1]->value.data_list.len;
    if (!len)
      {
	con->dd_printf("Error: nothing to search for\n");
	return(DD_FALSE);
      }
    t_addr addr= 0;
    bool found= mem->search_next(case_sensitive, array, len, &addr);
    while (found)
      {
	mem->dump(addr, addr+len-1, 8, con);
	addr++;
	found= mem->search_next(case_sensitive, array, len, &addr);
      }
  }
  else
    con->dd_printf("%s\n", short_help?short_help:"Error: wrong syntax\n");

  return(DD_FALSE);
}

//int
//cl_where_cmd::do_work(class cl_sim *sim,
//		      class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_where_cmd)
{
  return(do_real_work(uc, cmdline, con, DD_FALSE));
}

//int
//cl_Where_cmd::do_work(class cl_sim *sim,
//		      class cl_cmdline *cmdline, class cl_console *con)
COMMAND_DO_WORK_UC(cl_Where_cmd)
{
  return(do_real_work(uc, cmdline, con, DD_TRUE));
}


/* End of cmd.src/cmduc.cc */
