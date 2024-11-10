/******************************************************************************* 
* File Name:  B_I2CM_UDB_v1_20.v 
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*  
* Description: 
*  This file provides a top level model of the I2C Slave componnent
*  defining and all of the necessary interconnect.
* 
* Note: 
*
********************************************************************************
*                 Control and Status Register definitions
******************************************************************************** 
*
* TBD
******************************************************************************** 
*               Data Path register definitions   
******************************************************************************** 
*  INSTANCE NAME:  DatapathName 
*  DESCRIPTION: 
*  REGISTER USAGE: 
*   F0 => na 
*   F1 => na 
*   D0 => na  
*   D1 => na 
*   A0 => na 
*   A1 => na  
*   TBD
******************************************************************************** 
*               I*O Signals:   
******************************************************************************** 
*  IO SIGNALS: 
*  TBD
* 
********************************************************************************
* Copyright 2008-2009, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/



`include "cypress.v"
`ifdef B_I2CM_UDB_V_v1_10_ALREADY_INCLUDED
`else
`define B_I2CM_UDB_V_v1_10_ALREADY_INCLUDED

module B_I2CM_UDB_v1_20(
    input wire clk,
    input wire scl_in,
    input wire sda_in,
    output reg sda_out,
    output reg scl_out,
    output wire irq
);

    parameter multimaster = 0;

      
    localparam I2C_IDLE     = 5'h0;

    localparam I2C_PRE_START= 5'h2;
    localparam I2C_START   	= 5'h3;
    localparam I2C_RESTART  = 5'h4;
    localparam I2C_STOP  	= 5'h5;

    localparam I2C_TX_D0  	= 5'h8;
    localparam I2C_TX_D1  	= 5'h9;
    localparam I2C_TX_D2  	= 5'ha;
    localparam I2C_TX_D3  	= 5'hb;
    localparam I2C_TX_D4  	= 5'hc;
    localparam I2C_TX_D5  	= 5'hd;
    localparam I2C_TX_D6  	= 5'he;
    localparam I2C_TX_D7  	= 5'hf;

    localparam I2C_RX_D0  	= 5'h10;
    localparam I2C_RX_D1  	= 5'h11;
    localparam I2C_RX_D2  	= 5'h12;
    localparam I2C_RX_D3  	= 5'h13;
    localparam I2C_RX_D4  	= 5'h14;
    localparam I2C_RX_D5  	= 5'h15;
    localparam I2C_RX_D6  	= 5'h16;
    localparam I2C_RX_D7  	= 5'h17;

    localparam I2C_RX_STALL = 5'h06;
    localparam I2C_TX_STALL = 5'h07;

    localparam I2C_RX_ACK   = 5'h1e;
    localparam I2C_TX_ACK   = 5'h1f;

    reg[4:0] state;
    reg byte_complete;
    reg lrb;
    reg clkgen_tc1; 
    reg clkgen_tc2; 

    wire load_dummy;
    wire tx_reg_empty;
    wire shift_data_out;
    wire shift_enable;
    wire i2c_reset;
    wire clkgen_tc;
    wire clkgen_ce1; 
    wire clkgen_cl1; 
    wire txdata;
    wire rxdata;
    wire stalled;
    wire lost_arb;
    wire busy;

	/***************************************************************************
    * Control Register locations
    ***************************************************************************/
	localparam I2C_CTRL_UNUSED7			= 8'h7; 
	localparam I2C_CTRL_STOP  			= 8'h6;	// generate a stop
    localparam I2C_CTRL_RESTART       	= 8'h5;	// generate a restart 
    localparam I2C_CTRL_UNUSED4  		= 8'h4;
    localparam I2C_CTRL_TRANSMIT  		= 8'h3;  // 1=transmit 0=receive
    localparam I2C_CTRL_NACK     		= 8'h2;  // 1=nack on read, 0=another byte
    localparam I2C_CTRL_EN_MASTER   	= 8'h1;  // enable master
	localparam I2C_CTRL_UNUSED0			= 8'h0;	 // 
	wire	[7:0]	control;		    

	/* Instantiate the control register */
    cy_psoc3_control #(.cy_force_order(1))	
    ctrlreg(
        /* output	[07:00] */ .control(control)
    );

	wire ctrl_en_master 	= control[I2C_CTRL_EN_MASTER];
	wire ctrl_nack 			= control[I2C_CTRL_NACK];
	wire ctrl_transmit 		= control[I2C_CTRL_TRANSMIT];
	wire ctrl_restart 		= control[I2C_CTRL_RESTART];
	wire ctrl_stop 			= control[I2C_CTRL_STOP];
	
	/***************************************************************************
    * Status Register locations
    ***************************************************************************/	
	localparam I2C_STS_UNUSED6		= 8'h6;	
	localparam I2C_STS_LOST_ARB		= 8'h5;	
	localparam I2C_STS_BUSY			= 8'h4;	
	localparam I2C_STS_UNUSED3		= 8'h3;	
	localparam I2C_STS_UNUSED2		= 8'h2;	
	localparam I2C_STS_LRB			= 8'h1;
	localparam I2C_STS_BYTE_COMPLETE= 8'h0;

	wire	[6:0]	status;			        	/* Status Register Input */
	assign status[6] = 1'h0;
	assign status[I2C_STS_LOST_ARB] = lost_arb; 	// sync pulse, sticky
	assign status[I2C_STS_BUSY] = busy; 			// transparent
	assign status[3:2] = 2'h0;
    assign status[I2C_STS_LRB] = lrb; 				// transparent
	assign status[I2C_STS_BYTE_COMPLETE] = byte_complete;  // transparent

    /* Instantiate the status register and interrupt hook*/
    cy_psoc3_statusi #(.cy_force_order(1), .cy_md_select(7'h08), .cy_int_mask(7'h00)) 
    stsreg(
        /* input          */ .clock(clk),
        /* input  [06:00] */ .status(status),
        /* output         */ .interrupt(irq)
    );

	
//////////////////////////////////////////////////////////////////////////////
// Start/Stop Detect (multimaster only)
// Not generated for simple master
//////////////////////////////////////////////////////////////////////////////	
	
wire sda_negedge_detect;
wire sda_posedge_detect;
wire scl_negedge_detect;
wire scl_posedge_detect;

// need edge detect for start and stop

reg sda_in_dly;
always @(posedge clk)
begin
	sda_in_dly <= sda_in;
end

reg scl_in_dly;
always @(posedge clk)
begin
  	scl_in_dly <= scl_in;
end

    
assign sda_negedge_detect = ~sda_in & sda_in_dly;
assign sda_posedge_detect = sda_in & ~sda_in_dly;
assign scl_negedge_detect = ~scl_in & scl_in_dly;
assign scl_posedge_detect = scl_in & ~scl_in_dly;

// Compute start and stop
wire start_detect = (multimaster) ? scl_in & sda_negedge_detect : 1'b0;
wire stop_detect  = (multimaster) ? scl_in & sda_posedge_detect : 1'b0;


//////////////////////////////////////////////////////////////////////////////
// State decodes for convenience
//////////////////////////////////////////////////////////////////////////////

assign rxdata = 
	state == I2C_RX_D0 |
	state == I2C_RX_D1 |
	state == I2C_RX_D2 |
	state == I2C_RX_D3 |
	state == I2C_RX_D4 |
	state == I2C_RX_D5 |
	state == I2C_RX_D6 |
	state == I2C_RX_D7;

assign txdata = 
	state == I2C_TX_D0 |
	state == I2C_TX_D1 |
	state == I2C_TX_D2 |
	state == I2C_TX_D3 |
	state == I2C_TX_D4 |
	state == I2C_TX_D5 |
	state == I2C_TX_D6 |
	state == I2C_TX_D7;

assign stalled = (state == I2C_TX_STALL | state == I2C_RX_STALL);



//////////////////////////////////////////////////////////////////////////////
// ASYNCHRONOUS FIRMWARE RESET
//////////////////////////////////////////////////////////////////////////////

// Compute asynchronous reset
//
assign i2c_reset = ~ctrl_en_master;


//////////////////////////////////////////////////////////////////////////////
// LRB (last received bit, which is ACK/NACK from the slave on WRITE command
//////////////////////////////////////////////////////////////////////////////
//

always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) lrb <= 1'b0;
	else
	begin
		if (clkgen_tc1 & state == I2C_TX_ACK)
			lrb <= sda_in;
	end
end
		

    /**************************************************************************/
    /* SDA output                                                             */
    /**************************************************************************/
always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) sda_out <= 1'b1;
	else
	begin
		if (clkgen_tc2)     /* Maybe this is latch !!!!!!*/
		begin
			if ( (state == I2C_START | state == I2C_STOP)
			   | (state == I2C_RX_ACK & ~ctrl_nack) ) 
			   	sda_out <= 1'b0;
			else
			if (txdata) 
				sda_out <= shift_data_out;
			else
				sda_out <= 1'b1;
		end
	end
end


//////////////////////////////////////////////////////////////////////////////
// Busy computation
// Simple decode of IDLE for single master, use start and stop detect for multi master mode
//////////////////////////////////////////////////////////////////////////////

reg bus_busy;
always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) 
		bus_busy <= 1'b0;
	else
		bus_busy <= (start_detect | bus_busy) & !stop_detect; 
end

assign busy = (multimaster) ? bus_busy : (state != I2C_IDLE);

//////////////////////////////////////////////////////////////////////////////
// Contention (multimaster only)
// Optimized out for simple master
//////////////////////////////////////////////////////////////////////////////

// check for contention at the sample points, when the master is transmitting
wire contention = 	clkgen_tc
					& (txdata | state == I2C_RX_ACK)
					& (sda_in != sda_out);

// Do positive edge detect on contention
// goes to FSM (to IDLE), and sticky status register
reg contention1;
always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) 
		contention1 <= 1'b0;
	else
		contention1 <= contention; 
end

assign lost_arb = (multimaster) ? contention & ~contention1 : 1'b0;

//////////////////////////////////////////////////////////////////////////////
// Byte complete
// Need it to match the Fixed Function implementation
// So instead of a pulse that is capture by sticky register
// we have a sticky register in PLD that is cleared by GO
//////////////////////////////////////////////////////////////////////////////

always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) byte_complete <= 1'b0;
	else
	begin
		if (load_dummy) byte_complete <= 1'b0;
		else
		if (state == I2C_TX_STALL | state == I2C_RX_STALL) byte_complete <= 1'b1;
	end
end

//////////////////////////////////////////////////////////////////////////////
// PROTOCOL STATE MACHINE
//////////////////////////////////////////////////////////////////////////////
// Includes states and byte complete computation
// State changes are aligned with input shifting which is on the positive edge of SCL
// There are two flows: RX data/address, and TX data, and the address state bit
// distinguishes between RX data and address

always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) 
	begin
  		state <= I2C_IDLE;
	end
	else
	begin


	if (clkgen_tc1)
	begin

	if (lost_arb)	state <= I2C_IDLE;
	else
    case (state)

    I2C_IDLE: 
	begin
        if (~tx_reg_empty & ~busy) 	state <= I2C_PRE_START;
        else       					state <= I2C_IDLE;
	end

	I2C_PRE_START:
	begin
        state <= I2C_START;
	end

	I2C_START:
	begin
        state <= I2C_TX_D0;
	end

	I2C_TX_D0:
	begin
        state <= I2C_TX_D1;
	end

	I2C_TX_D1:
	begin
        state <= I2C_TX_D2;
	end

	I2C_TX_D2:
	begin
        state <= I2C_TX_D3;
	end

	I2C_TX_D3:
	begin
        state <= I2C_TX_D4;
	end

	I2C_TX_D4:
	begin
        state <= I2C_TX_D5;
	end

	I2C_TX_D5:
	begin
        state <= I2C_TX_D6;
	end

	I2C_TX_D6:
	begin
        state <= I2C_TX_D7;
	end

	I2C_TX_D7:
	begin
        state <= I2C_TX_ACK;
	end

    I2C_TX_ACK: 
	begin
		state <= I2C_TX_STALL;
    end

	// This could be on address ACK or data ACK
	I2C_TX_STALL:
	begin
		if (~tx_reg_empty)
		begin
			if (ctrl_stop)
				state <= I2C_STOP;
			else
			if (ctrl_restart)
				state <= I2C_RESTART;
			else
			begin
				if (ctrl_transmit) 	state <= I2C_TX_D0;
				else 				state <= I2C_RX_D0;
			end
		end
		else 
			state <= I2C_TX_STALL;
	end


	I2C_RX_D0:
	begin
        state <= I2C_RX_D1;
	end

	I2C_RX_D1:
	begin
        state <= I2C_RX_D2;
	end

	I2C_RX_D2:
	begin
        state <= I2C_RX_D3;
	end

	I2C_RX_D3:
	begin
        state <= I2C_RX_D4;
	end

	I2C_RX_D4:
	begin
        state <= I2C_RX_D5;
	end

	I2C_RX_D5:
	begin
        state <= I2C_RX_D6;
	end

	I2C_RX_D6:
	begin
        state <= I2C_RX_D7;
	end

    I2C_RX_D7: 
	begin
		state <= I2C_RX_STALL;
    end
    
    I2C_RX_STALL: 
	begin
        if (~tx_reg_empty) 
			state <= I2C_RX_ACK;
		else
			state <= I2C_RX_STALL;
    end 

    I2C_RX_ACK: 
	begin
		if (ctrl_nack) 	
		begin
			if (ctrl_restart) 	state <= I2C_RESTART;
			else				state <= I2C_STOP;
		end
		else 			
			state <= I2C_RX_D0;
    end 

	// Need this state to setup the start
	// SDA goes high from whereever it was

    I2C_RESTART: 
	begin
		state <= I2C_START;
    end 

	// Generate STOP hold time

    I2C_STOP: 
	begin
		state <= I2C_IDLE; 
	end

	endcase
	end
  end
end

//////////////////////////////////////////////////////////////////////////////
// SHIFTER - Datapath
//////////////////////////////////////////////////////////////////////////////
// Datapath is shifting into and out of A0
// User writes TX data to A0 and reads TX data from A0, always stalled
// F1 status is used to synchronize start of transmission

// Compute when to enable shifting
//
assign shift_enable = clkgen_tc & (rxdata | txdata);

// Compute when to continue transfer, CPU signals this with a write to FIFO F1
// this is dummy data, it isn't used, its loaded into A1
//
assign load_dummy = (~tx_reg_empty & clkgen_tc1);


wire [2:0] cs_addr_shifter = {1'b0, load_dummy,  shift_enable };        
    cy_psoc3_dp8 #(.cy_dpconfig_a(
    {
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
    	`CS_SHFT_OP___SL, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG0 Comment:IDLE */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
    	`CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG1 Comment:SHIFT A0 */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
    	`CS_SHFT_OP___SL, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG2 Comment:LOAD A1 from F1 */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG3 Comment: */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG4 Comment: */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG5 Comment: */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG6 Comment: */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG7 Comment: */
    	  8'hFF, 8'h00,	/*SC_REG4	Comment: */
    	  8'hFF, 8'h7F,	/*SC_REG5	Comment:Address Mask */
    	`SC_CMPB_A0_D1, `SC_CMPA_A0_D1, `SC_CI_B_ARITH,
    	`SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_ENBL,
    	`SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
    	`SC_SI_A_ROUTE, /*SC_REG6 Comment: */
    	`SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'b0,
    	1'b0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
    	`SC_MSB_DSBL, `SC_MSB_BIT7, `SC_MSB_NOCHN,
    	`SC_FB_NOCHN, `SC_CMP1_NOCHN,
    	`SC_CMP0_NOCHN, /*SC_REG7 Comment: */
    	 10'h0, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
    	`SC_FIFO__EDGE,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,
    	`SC_WRK16CAT_DSBL /*SC_REG8 Comment: */
    })) shifter(
    /*  input                   */  .clk(clk),
    /*  input   [02:00]         */  .cs_addr(cs_addr_shifter),
    /*  input                   */  .route_si(sda_in),
    /*  input                   */  .route_ci(1'b0),
    /*  input                   */  .f0_load(1'b0),
    /*  input                   */  .f1_load(1'b0),
    /*  input                   */  .d0_load(1'b0),
    /*  input                   */  .d1_load(1'b0),
    /*  output                  */  .ce0(),
    /*  output                  */  .cl0(),
    /*  output                  */  .z0(),
    /*  output                  */  .ff0(),
    /*  output                  */  .ce1(),
    /*  output                  */  .cl1(),
    /*  output                  */  .z1(),
    /*  output                  */  .ff1(),
    /*  output                  */  .ov_msb(),
    /*  output                  */  .co_msb(),
    /*  output                  */  .cmsb(),
    /*  output                  */  .so(shift_data_out),
    /*  output                  */  .f0_bus_stat(), 
    /*  output                  */  .f0_blk_stat(),
    /*  output                  */  .f1_bus_stat(), 
    /*  output                  */  .f1_blk_stat(tx_reg_empty)
    );

//////////////////////////////////////////////////////////////////////////////
// SCL OUT
//////////////////////////////////////////////////////////////////////////////
// Datapath is counting samples at divide by 8
// State changes on "tc"
// Output clock is "cmpl0" (compare < 4)


wire enable_clkgen = (scl_out == scl_in) & ((state == I2C_IDLE & ~tx_reg_empty) | (state != I2C_IDLE));
					

wire [2:0] cs_addr_clkgen = {1'b0, clkgen_tc,  enable_clkgen };        

    cy_psoc3_dp8 #(
	.d0_init_a(8'd15),
	.d1_init_a(8'd8),
    .cy_dpconfig_a(
    {
    	`CS_ALU_OP__DEC, `CS_SRCA_A0, `CS_SRCB_A0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG0 Comment:IDLE */
    	`CS_ALU_OP__DEC, `CS_SRCA_A0, `CS_SRCB_A0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG1 Comment:DEC A0 */
    	`CS_ALU_OP__DEC, `CS_SRCA_A0, `CS_SRCB_A0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG2 Comment:IDLE */
    	`CS_ALU_OP__DEC, `CS_SRCA_A0, `CS_SRCB_A0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC___D0, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG3 Comment:LOAD A0 from D0 */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG4 Comment: */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG5 Comment: */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG6 Comment: */
    	`CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    	`CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    	`CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    	`CS_CMP_SEL_CFGA, /*CS_REG7 Comment: */
    	  8'hFF, 8'h00,	/*SC_REG4	Comment: */
    	  8'hFF, 8'h7F,	/*SC_REG5	Comment:Address Mask */
    	`SC_CMPB_A0_D1, `SC_CMPA_A0_D1, `SC_CI_B_ARITH,
    	`SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_ENBL,
    	`SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
    	`SC_SI_A_ROUTE, /*SC_REG6 Comment: */
    	`SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'b0,
    	1'b0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
    	`SC_MSB_DSBL, `SC_MSB_BIT7, `SC_MSB_NOCHN,
    	`SC_FB_NOCHN, `SC_CMP1_NOCHN,
    	`SC_CMP0_NOCHN, /*SC_REG7 Comment: */
    	 10'h0, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
    	`SC_FIFO__EDGE,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,
    	`SC_WRK16CAT_DSBL /*SC_REG8 Comment: */
    })) clkgen(
    /*  input                   */  .clk(clk),
    /*  input   [02:00]         */  .cs_addr(cs_addr_clkgen),
    /*  input                   */  .route_si(1'b0),
    /*  input                   */  .route_ci(1'b0),
    /*  input                   */  .f0_load(1'b0),
    /*  input                   */  .f1_load(1'b0),
    /*  input                   */  .d0_load(1'b0),
    /*  input                   */  .d1_load(1'b0),
    /*  output                  */  .ce0(),
    /*  output                  */  .cl0(),
    /*  output                  */  .z0(clkgen_tc),
    /*  output                  */  .ff0(),
    /*  output                  */  .ce1(clkgen_ce1),
    /*  output                  */  .cl1(clkgen_cl1),
    /*  output                  */  .z1(),
    /*  output                  */  .ff1(),
    /*  output                  */  .ov_msb(),
    /*  output                  */  .co_msb(),
    /*  output                  */  .cmsb(),
    /*  output                  */  .so(),
    /*  output                  */  .f0_bus_stat(), 
    /*  output                  */  .f0_blk_stat(),
    /*  output                  */  .f1_bus_stat(), 
    /*  output                  */  .f1_blk_stat()
    );

// Need to manually register TC until we can have both 
// registered and combo version out
always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) 	clkgen_tc1 <= 1'b1;
	else 			clkgen_tc1 <= clkgen_tc;
end

// Delayed so data can be changed one cycle after clock for hold
always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) 	clkgen_tc2 <= 1'b1;
	else 			clkgen_tc2 <= clkgen_tc1;
end

// Registered  SCL_OUT

always @(posedge clk or posedge i2c_reset)
begin
	if (i2c_reset) 
		scl_out <= 1'b1;
	else
	if (stalled)
		scl_out <= 1'b0;
	else
	if (state == I2C_RESTART & clkgen_tc1)
		scl_out <= 1'b1;
	else
	if (state == I2C_START & clkgen_tc1)
		scl_out <= clkgen_cl1;
	else
	if (state == I2C_STOP & clkgen_tc1)
		scl_out <= 1'b1;
	else
	if (state != I2C_IDLE & state != I2C_START & state != I2C_PRE_START) 
		scl_out <= clkgen_cl1;
	else
		scl_out <= 1'b1;
end



		
endmodule //B_I2CM_UDB_v1_20
`endif /* B_I2CM_UDB_V_v1_0_ALREADY_INCLUDED */
