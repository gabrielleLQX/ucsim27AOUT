/*
 * Simulator of microcontrollers (mov.cc)
 *
 * Copyright (C) 1999,99 Drotos Daniel, Talker Bt.
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

/* Bugs fixed by Sandeep Dutta:
 *	source<->dest bug in "mov direct,direct"
 *	get register in "mov @ri,address"
 */
 
#include "ddconfig.h"

#include <stdio.h>

// sim
#include "memcl.h"

// local
#include "uc51cl.h"
#include "regs51.h"

/*
 * 0x74 2 12 MOV A,#data
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_a_Sdata(uchar code)
{
  acc->write(fetch());
  return(resGO);
}


/*
 * 0x75 3 24 MOV addr,#data
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_addr_Sdata(uchar code)
{
  class cl_memory_cell *cell;

  cell= get_direct(fetch());
  cell->write(fetch());
  tick(1);
  return(resGO);
}


/*
 * 0x76-0x77 2 12 MOV @Ri,#data
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_Sri_Sdata(uchar code)
{
  class cl_memory_cell *cell;
  
  cell= iram->get_cell(get_reg(code & 0x01)->read());
  t_mem d= fetch();
  cell->write(d);
  return(resGO);
}


/*
 * 0x78-0x7f 2 12 MOV Rn,#data
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_rn_Sdata(uchar code)
{
  class cl_memory_cell *reg;

  reg= get_reg(code & 0x07);
  reg->write(fetch());
  return(resGO);
}


/*
 * 0x93 1 24 MOVC A,@A+DPTR
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_movc_a_Sa_pc(uchar code)
{
  CC2530radio->MovInstruction = true;
  acc->write(rom->read(PC + acc->read()));
  CC2530radio->MovInstruction = false;
  tick(1);
  return(resGO);
}


/*
 * 0x85 3 24 MOV addr,addr
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_addr_addr(uchar code)
{
  class cl_memory_cell *d, *s;
  CC2530radio->MovInstruction = true;
  /* SD reversed s & d here */
  s= get_direct(fetch());
  d= get_direct(fetch());
  d->write(s->read());
  CC2530radio->MovInstruction = false;
  tick(1);
  return(resGO);
}


/*
 * 0x86-0x87 2 24 MOV addr,@Ri
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_addr_Sri(uchar code)
{
  class cl_memory_cell *d, *s;

  d= get_direct(fetch());
  s= iram->get_cell(get_reg(code & 0x01)->read());
  d->write(s->read());
  tick(1);
  return(resGO);
}


/*
 * 0x88-0x8f 2 24 MOV addr,Rn
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_addr_rn(uchar code)
{
  class cl_memory_cell *cell;

  cell= get_direct(fetch());
  cell->write(get_reg(code & 0x07)->read());
  tick(1);
  return(resGO);
}


/*
 * 0x90 3 24 MOV DPTR,#data
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_dptr_Sdata(uchar code)
{
  sfr->write(DPH, fetch());
  sfr->write(DPL, fetch());
  tick(1);
  return(resGO);
}


/*
 * 0x93 1 24 MOVC A,@A+DPTR
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_movc_a_Sa_dptr(uchar code)
{
  //CC2530radio->MovInstruction = true;
  acc->write(rom->read(sfr->read(DPH)*256+sfr->read(DPL) +  acc->read()));
  //CC2530radio->MovInstruction = false;
  tick(1);
  return(resGO);
}


/*
 * 0xa6-0xa7 2 24 MOV @Ri,addr
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_Sri_addr(uchar code)
{
  class cl_memory_cell *d, *s;
  CC2530radio->MovInstruction = true;
  d= iram->get_cell(get_reg(code & 0x01)->read());
  s= get_direct(fetch());
  d->write(s->read());
  CC2530radio->MovInstruction = false;
  tick(1);

  return(resGO);
}


/*
 * 0xa8-0xaf 2 24 MOV Rn,addr
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_rn_addr(uchar code)
{
  class cl_memory_cell *reg, *cell;
  CC2530radio->MovInstruction = true;
  reg = get_reg(code & 0x07);
  cell= get_direct(fetch());
  reg->write(cell->read());
  CC2530radio->MovInstruction = false;
  tick(1);
  return(resGO);
}


/*
 * 0xc0 2 24 PUSH addr
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_push(uchar code)
{
  t_addr sp, sp_before/*, sp_after*/;
  t_mem data;
  class cl_memory_cell *stck, *cell;

  cell= get_direct(fetch());
  sp_before= sfr->get(SP);
  sp= /*sp_after= */sfr->wadd(SP, 1);
  stck= iram->get_cell(sp);
  stck->write(data= cell->read());
  class cl_stack_op *so=
    new cl_stack_push(instPC, data, sp_before, sp/*_after*/);
  so->init();
  stack_write(so);
  tick(1);
  return(resGO);
}


/*
 * 0xc5 2 12 XCH A,addr
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_xch_a_addr(uchar code)
{
  t_mem temp;
  class cl_memory_cell *cell;

  cell= get_direct(fetch());
  temp= acc->read();
  acc->write(cell->read());
  cell->write(temp);
  return(resGO);
}


/*
 * 0xc6-0xc7 1 12 XCH A,@Ri
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_xch_a_Sri(uchar code)
{
  t_mem temp;
  class cl_memory_cell *cell;

  cell= iram->get_cell(get_reg(code & 0x01)->read());
  temp= acc->read();
  acc->write(cell->read());
  cell->write(temp);
  return(resGO);
}


/*
 * 0xc8-0xcf 1 12 XCH A,Rn
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_xch_a_rn(uchar code)
{
  t_mem temp;
  class cl_memory_cell *reg;

  reg = get_reg(code & 0x07);
  temp= acc->read();
  acc->write(reg->read());
  reg->write(temp);
  return(resGO);
}


/*
 * 0xd0 2 24 POP addr
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_pop(uchar code)
{
  t_addr sp, sp_before/*, sp_after*/;
  t_mem data;
  class cl_memory_cell *cell, *stck;

  sp_before= sfr->get(SP);
  cell= get_direct(fetch());
  stck= iram->get_cell(sfr->get(SP));
  cell->write(data= stck->read());
  sp= /*sp_after= */sfr->wadd(SP, -1);
  tick(1);
  class cl_stack_op *so=
    new cl_stack_pop(instPC, data, sp_before, sp/*_after*/);
  so->init();
  stack_read(so);
  return(resGO);
}


/*
 * 0xd6-0xd7 1 12 XCHD A,@Ri
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_xchd_a_Sri(uchar code)
{
  t_mem temp, d;
  class cl_memory_cell *cell;

  cell= iram->get_cell(get_reg(code & 0x01)->read());
  temp= (d= cell->read()) & 0x0f;
  cell->write((d & 0xf0) | (acc->read() & 0x0f));
  acc->write((acc->get() & 0xf0) | temp);
  return(resGO);
}


/*
 * 0xe0 1 24 MOVX A,@DPTR
 *____________________________________________________________________________
 *Mofified by Calypso for correct access to xdata (xram)  when CC2530 defined
 */

int
cl_51core::inst_movx_a_Sdptr(uchar code)
{
  t_mem d;
  CC2530radio->MovInstruction = true;
  d=xram->read(sfr->read(DPH)*256 + sfr->read(DPL));
  acc->write(d);
  CC2530radio->MovInstruction = false;
  tick(1);
  return(resGO);
}


/*
 * 0xe2-0xe3 1 24 MOVX A,@Ri
 *____________________________________________________________________________
 *Mofified by Calypso for correct access to xdata (xram)  when CC2530 defined
 */

int
cl_51core::inst_movx_a_Sri(uchar code)
{
  t_mem d, mpage;

  d= get_reg(code & 0x01)->read();

  #ifdef CC2530
  mpage= sfr->read(MPAGE);
  if (mpage==0x70 && d>=0x80){
    acc->write(sfr->read(mpage*256 + d));
    TRACE();
  }
  else {
    acc->write(xram->read(mpage*256 + d));
  }
#else
  acc->write(xram->read(sfr->read(P2)*256 + d));
  #endif 
  tick(1);
  return(resGO);
}


/*
 * 0xe5 2 12 MOV A,addr
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_a_addr(uchar code)
{
  class cl_memory_cell *cell;
  t_addr address= fetch();
    CC2530radio->MovInstruction = true;
  /* If this is ACC, it is an invalid instruction */
  if (address == ACC)
    {
      //sim->app->get_commander()->
      //debug("Invalid Instruction : E5 E0  MOV A,ACC  at  %06x\n", PC);
      inst_unknown();
    }
  else
    {
      cell= get_direct(address);
      acc->write(cell->read());
    }
  CC2530radio->MovInstruction = false;
  return(resGO);
}


/*
 * 0xe6-0xe7 1 12 MOV A,@Ri
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_a_Sri(uchar code)
{
  class cl_memory_cell *cell;

  cell= iram->get_cell(get_reg(code & 0x01)->read());
  acc->write(cell->read());
  return(resGO);
}


/*
 * 0xe8-0xef 1 12 MOV A,Rn
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_a_rn(uchar code)
{
  acc->write(get_reg(code & 0x07)->read());
  return(resGO);
}


/*
 * 0xf0 1 24 MOVX @DPTR,A
 *____________________________________________________________________________
 *Mofified by Calypso for correct access to xdata (xram) when CC2530 defined
 */

int
cl_51core::inst_movx_Sdptr_a(uchar code)
{
#ifdef CC2530
  TRACE();
  t_addr dp = sfr->read(DPH)*256 + sfr->read(DPL);
  if ((sfr->read(DPH)==0x70) && (sfr->read(DPL)>= 0x80)){
    sfr->write(dp, acc->read());
    }
  else
    {
      xram->write(dp, acc->read());
      fprintf(stderr, "MOVX: Writing %d in xram at @ %d\n", acc->read(), dp);
    }
#else
  xram->write(dp, acc->read());
#endif
  tick(1);
  return(resGO);
}


/*
 * 0xf2-0xf3 1 24 MOVX @Ri,A
 *____________________________________________________________________________
 *Mofified by Calypso for correct access to xdata (xram) when CC2530 defined
 */

int
cl_51core::inst_movx_Sri_a(uchar code)
{
  t_mem d, mpage;

  d= get_reg(code & 0x01)->read();
  #ifndef CC2530
  xram->write(sfr->read(P2)*256 + d, acc->read());
#endif
  #ifdef CC2530
  TRACE();
  mpage=sfr->read(MPAGE);
  if ((mpage == 0x70) && (d >= 0x80)) {
    sfr->write(mpage*256 + d, acc->read());
  }
  else
    xram->write(mpage*256 + d, acc->read());
#endif
  tick(1);
  return(resGO);
}


/*
 * 0xf5 2 12 MOV addr,A
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_addr_a(uchar code)
{
  class cl_memory_cell *cell;
  
  cell= get_direct(fetch());
  cell->write(acc->read());
  return(resGO);
}


/*
 * 0xf6-0xf7 1 12 MOV @Ri,A
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_Sri_a(uchar code)
{
  class cl_memory_cell *cell;

  cell= iram->get_cell(get_reg(code & 0x01)->read());
  cell->write(acc->read());
  return(resGO);
}


/*
 * 0xf8-0xff 1 12 MOV Rn,A
 *____________________________________________________________________________
 *
 */

int
cl_51core::inst_mov_rn_a(uchar code)
{
  class cl_memory_cell *reg;

  reg= get_reg(code &0x07);
  reg->write(acc->read());
  return(resGO);
}


/* End of s51.src/mov.cc */
