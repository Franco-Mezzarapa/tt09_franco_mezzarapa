module serializer #(parameter MSG_SIZE = 64) (  // MSG_SIZE set to 8 for 8-bit data
    input wire [MSG_SIZE - 1: 0] iData_in,      // Parallel data input (e.g., A5 in this case)
    input wire [$clog2(MSG_SIZE):0] iCounter,   // Counter that triggers serialization
    input wire clk,                             // Clock
    input wire ena,                             // Enable
    input wire rst_n,                           // Reset (active low)
    
    output reg oData_flag,                      // Flag to indicate valid serial data
    output reg oData_out                        // Serial data output
);

    integer serial_counter;                     // Counter for serialization
    reg done_serializing;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            oData_out <= 0;
            oData_flag <= 0;
            serial_counter <= MSG_SIZE - 1;     // Start from MSB position
            done_serializing <= 0;
        end 
        else if (ena && iCounter == MSG_SIZE && !done_serializing) begin  // Check if deserialization is complete
            oData_flag <= 1'b1;                  // Set flag during serialization
            if (serial_counter >= 0) begin
                oData_out <= iData_in[serial_counter]; // Output the current bit (MSB first)
                serial_counter <= serial_counter - 1;  // Decrement the counter
            end 
            else if (serial_counter == -1) begin
                // After the last bit has been output, set oData_out to 0
                oData_out <= 0;                        
                oData_flag <= 1'b0;
                done_serializing <= 1;
            end
        end
    end

endmodulemodule serializer #(parameter MSG_SIZE = 64) (  // MSG_SIZE set to 8 for 8-bit data
    input wire [MSG_SIZE - 1: 0] iData_in,      // Parallel data input (e.g., A5 in this case)
    input wire [$clog2(MSG_SIZE):0] iCounter,   // Counter that triggers serialization
    input wire clk,                             // Clock
    input wire ena,                             // Enable
    input wire rst_n,                           // Reset (active low)
    
    output reg oData_flag,                      // Flag to indicate valid serial data
    output reg oData_out                        // Serial data output
);

    integer serial_counter;                     // Counter for serialization
    reg done_serializing;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            oData_out <= 0;
            oData_flag <= 0;
            serial_counter <= MSG_SIZE - 1;     // Start from MSB position
            done_serializing <= 0;
        end 
        else if (ena && iCounter == MSG_SIZE && !done_serializing) begin  // Check if deserialization is complete
            oData_flag <= 1'b1;                  // Set flag during serialization
            if (serial_counter >= 0) begin
                oData_out <= iData_in[serial_counter]; // Output the current bit (MSB first)
                serial_counter <= serial_counter - 1;  // Decrement the counter
            end 
            else if (serial_counter == -1) begin
                // After the last bit has been output, set oData_out to 0
                oData_out <= 0;                        
                oData_flag <= 1'b0;
                done_serializing <= 1;
            end
        end
    end

endmodule
