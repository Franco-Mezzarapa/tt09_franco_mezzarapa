module shift_register(
    input clk,
    input rst_n,
    input ena,
    
    input [3:0] key_counter,           // 4-bit key counter (change as needed)
    input [6:0] message_counter,       // 7-bit message counter
    input [6:0] ciphertext_counter,    // 7-bit ciphertext counter
    
    input debug_wire,
    
    output reg data_out,
    output reg data_flag
);

    // 26 bits - 4 for the starting sequence, 4 for the key counter,
    // 7 for the message counter, 7 for the ciphertext counter, 4 for the ending sequence
    reg [25:0] content_register;
    integer content_counter;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            data_out  <= 0;
            data_flag <= 0;
            content_register <= 0;
            content_counter  <= 25;
        end else if (ena && debug_wire) begin
            if (content_counter == 25) begin
                // Load data into content_register at the start of each transmission
                data_flag <= 1;
                content_register[25:22] <= 4'b1111;               // Start sequence
                content_register[21:18] <= key_counter;           // 4-bit key counter
                content_register[17:11] <= message_counter;       // 7-bit message counter
                content_register[10:4]  <= ciphertext_counter;    // 7-bit ciphertext counter
                content_register[3:0]   <= 4'b1001;               // Ending sequence
                
                // Output the MSB first
                data_out <= content_register[25];
                content_counter <= content_counter - 1;
            end else if (content_counter >= 0) begin
                // Shift out the bits one by one
                data_out <= content_register[content_counter];
                content_counter <= content_counter - 1;
            end else begin
                // Reset the counter after completing the transmission to start over
                content_counter <= 25; // Restart transmission from the MSB
                data_flag <= 0;        // Clear data_flag after the last bit is sent
            end
        end else begin
            // If not enabled or debug_wire is low, reset outputs and counter
            data_out <= 0;
            data_flag <= 0;
            content_counter <= 25; // Reset counter to start at the beginning
        end
    end
endmodule
