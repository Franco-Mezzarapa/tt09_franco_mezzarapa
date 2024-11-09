module tt_um_franco_mezzarapa(
    input  wire [7:0] ui_in,    // Dedicated inputs
    output wire [7:0] uo_out,   // Dedicated outputs
    input  wire [7:0] uio_in,   // IOs: Input path
    output wire [7:0] uio_out,  // IOs: Output path
    output wire [7:0] uio_oe,   // IOs: Enable path (active high: 0=input, 1=output)
    
    input  wire       ena,      
    input  wire       clk,      // clock
    input  wire       rst_n    // reset_n - low to reset
);

localparam MSG_SIZE = 64;
localparam KEY_SIZE = 8;

wire [$clog2(8): 0] oBit_counter_key;
wire [$clog2(64): 0] oBit_counter_msg;
wire [$clog2(64): 0] oBit_counter_ciphertext;
    
wire [63:0] output_message;
wire [63:0] output_ciphertext;
wire [7:0] key;
    

// Unused Wires
assign uio_out = 8'b0;
assign uio_oe  = 8'b0;

// unused output wires.
assign uo_out[3] = 1'b0;
assign uo_out[4] = 1'b0;
assign uo_out[5] = 1'b0;
assign uo_out[6] = 1'b0;
assign uo_out[7] = 1'b0;

 wire _unused = &{uio_in,ui_in[3],ui_in[4],ui_in[5],ui_in[6],ui_in[7],1'b0};

//assign output_message = message;

deserializer #(.MSG_SIZE(8)) deserializer_key(
     .iData_in  (ui_in[0]),              // Data coming in serially
     .iData_flag(ui_in[1]),              // Flag that determines when data is being loaded
    
     .clk   (clk),                       // Clock
     .ena   (ena),                       // Enable
     .rst_n (rst_n),                     // Reset
    
     .oBit_counter (oBit_counter_key),   // Bit counter for key
     .oData_out(key)                     // Output for deserialized key
);

deserializer #(.MSG_SIZE(64)) deserializer_msg(
     .iData_in  (ui_in[0]),              // Data coming in serially
     .iData_flag(ui_in[2]),              // Flag that determines when data is being loaded
    
     .clk   (clk),                       // Clock
     .ena   (ena),                       // Enable
     .rst_n (rst_n),                     // Reset
    
     .oBit_counter (oBit_counter_msg),   // Bit counter for message
     .oData_out(output_message)                 // Output for deserialized message
);

xor_encrypt xor_message(
    .clk(clk),
    .ena(ena),
    .rst_n(rst_n),
    
    .iMessage(output_message),
    .iKey(key),
    
    .iMessage_bit_counter(oBit_counter_msg),
    .iKey_bit_counter(oBit_counter_key),
    
    .encryption_status(uo_out[2]),                   //uo_out 3 is the encryption signal for the CW.
    .oCiphertext_counter(oBit_counter_ciphertext),   //Counter for the ciphertext
    .oCiphertext(output_ciphertext)                  // ciphertext output
);

serializer serialize_ciphertext(
    .iData_in(output_ciphertext),
    .iCounter(oBit_counter_ciphertext),
    
    .clk(clk),
    .ena(ena),
    .rst_n(rst_n),
    
    .oData_flag(uo_out[1]),
    .oData_out(uo_out[0])
);
endmodule
