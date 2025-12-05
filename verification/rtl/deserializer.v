module deserializer #(parameter MSG_SIZE = 64)(
    input wire iData_in,            //data coming in serially
    input wire iData_flag,          //the flag that determines what that data is.
    
    input wire clk,                                      // clock
    input wire ena,                                      // enable
    input wire rst_n,                                    // reset
    
    output reg [$clog2(MSG_SIZE):0] oBit_counter,       // bit counter
    output reg [MSG_SIZE - 1: 0]         oData_out
);
    
always @(posedge clk or negedge rst_n) begin
    
    if (!rst_n) begin
        oData_out <= 0;
        oBit_counter <= 0;
    end else if (ena && iData_flag) begin           // if enabled and the flag is active.
        
        //I might be able to optimize here.
        if ( oBit_counter < MSG_SIZE) begin             // Check to see if all the bits have been shifted.
            oData_out <= {oData_out[MSG_SIZE-2:0], iData_in}; // Shift in the new bit
            oBit_counter <=  oBit_counter + 1;           // Update the counter. 
        end
   end            
end
endmodule
